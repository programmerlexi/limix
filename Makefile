COMMON_QEMU_FLAGS=-machine q35 -m 4G -smp 8 -serial stdio -audio driver=alsa,model=hda,id=audio

INCLUDES=$(shell find kernel/include -type f) $(shell find libk/include -type f)

JOBS=$(shell nproc --all)

OVMFDIR=/usr/share/edk2-ovmf/x64
UEFI_OPTIONS=-drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE.fd",readonly=on

all: hdd iso

limine/limine.h: limine

include: $(INCLUDES)
	@mkdir -p include
	@cp limine/limine.h include
	@touch -m include
	@$(foreach head,$?,mkdir -p $(dir $(patsubst kernel/include/%.h,include/kernel/%.h,$(patsubst libk/include/%,include/libk/%,$(head))));cp $(head) $(patsubst kernel/include/%.h,include/kernel/%.h,$(patsubst libk/include/%,include/libk/%,$(head)));)
	@touch -m include

.PHONY: libk kernel util
libk: include
	@make -j$(JOBS) -C libk

kernel: libk include
	@make -j$(JOBS) -C kernel

util:
	@make -j $(JOBS) -C util bin/font.lime bin/pci_devices.reg bin/pci_vendors.reg

base: kernel util

hdd: image.hdd
iso: image.iso

image.hdd: base limine
	@echo "Making image.hdd"
	@dd if=/dev/zero bs=1M count=0 seek=64 of=image.hdd
	@dd if=/dev/zero bs=1M count=1 of=image.hdd
	@dd if=/dev/zero bs=1M count=1 seek=63 of=image.hdd
	@sgdisk image.hdd -n 1:2048 -t 1:ef00
	@./limine/limine bios-install image.hdd
	@mformat -i image.hdd@@1M
	@mmd -i image.hdd@@1M ::/boot
	@mcopy -i image.hdd@@1M kernel/bin/limix.gz util/bin/font.lime util/bin/pci_devices.reg util/bin/pci_vendors.reg ::/boot
	@mmd -i image.hdd@@1M ::/boot/limine
	@mcopy -i image.hdd@@1M boot/limine.cfg limine/limine-bios.sys ::/boot/limine
	@mmd -i image.hdd@@1M ::/EFI
	@mmd -i image.hdd@@1M ::/EFI/BOOT
	@mcopy -i image.hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	@mcopy -i image.hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

image.iso: base limine
	@echo "Making image.iso"
	@mkdir -p iso_root
	@mkdir -p iso_root/boot
	@cp -v kernel/bin/limix.gz util/bin/font.lime util/bin/pci_devices.reg util/bin/pci_vendors.reg iso_root/boot/
	@mkdir -p iso_root/boot/limine
	@cp -v boot/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin \
		limine/limine-uefi-cd.bin iso_root/boot/limine/
	@mkdir -p iso_root/EFI/BOOT
	@cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	@cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	@xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
    		-no-emul-boot -boot-load-size 4 -boot-info-table \
    		--efi-boot boot/limine/limine-uefi-cd.bin \
    		-efi-boot-part --efi-boot-image --protective-msdos-label \
        	iso_root -o image.iso
	@./limine/limine bios-install image.iso

limine:
	@git clone https://github.com/limine-bootloader/limine.git --branch=v7.x-binary --depth=1
	@make -j$(JOBS) -C limine

run-hdd: image.hdd
	qemu-system-x86_64 -hda image.hdd $(COMMON_QEMU_FLAGS)

run-iso: image.iso
	qemu-system-x86_64 -cdrom image.iso $(COMMON_QEMU_FLAGS)

run-iso-uefi: image.iso
	qemu-system-x86_64 $(UEFI_OPTIONS) -cdrom image.iso $(COMMON_QEMU_FLAGS)

run-hdd-uefi: image.hdd
	qemu-system-x86_64 $(UEFI_OPTIONS) -hda image.hdd $(COMMON_QEMU_FLAGS)

run-kvm: image.hdd image.iso
	qemu-system-x86_64 -hda image.hdd -cdrom image.iso $(COMMON_QEMU_FLAGS) -enable-kvm

run-kvm-uefi: image.hdd image.iso
	qemu-system-x86_64 $(UEFI_OPTIONS) -hda image.hdd -cdrom image.iso $(COMMON_QEMU_FLAGS) -enable-kvm


run-debug: image.iso
	qemu-system-x86_64 -cdrom image.iso $(UEFI_OPTIONS) $(COMMON_QEMU_FLAGS) -s -S -no-reboot -d int,cpu_reset

run-debug-kvm: image.iso
	qemu-system-x86_64 -cdrom image.iso $(UEFI_OPTIONS) $(COMMON_QEMU_FLAGS) -enable-kvm -s -S -no-reboot

debug:
	gdb -s kernel/bin/limix -ex "target remote localhost:1234"

clean:
	@make -C kernel clean
	@make -C libk clean
	@make -C util clean
	@rm -rf image.* iso_root build
	@rm -rf include

cleanAll: clean
	@rm -rf limine compile_commands.json

genComp:
	@mkdir -p build
	@bear -- make

rebuild:
	@make clean
	@make
