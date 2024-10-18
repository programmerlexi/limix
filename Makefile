COMMON_QEMU_FLAGS=-machine q35 -m 4G -usb -device qemu-xhci -serial stdio

INCLUDES=$(shell find kernel/include -type f) $(shell find libk/include -type f)

SYSROOT=sysroot

JOBS?=$(shell nproc --all)

OVMFDIR=/usr/share/edk2-ovmf/x64
UEFI_OPTIONS=-drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE.fd",readonly=on

ifeq ($(LIMINE),)
LIMINE=limine
endif

LIMINE_BINARY=limine/limine

all: hdd iso

limine/limine.h: $(LIMINE)

$(SYSROOT): $(INCLUDES) lai $(LIMINE)/limine.h
	@mkdir -p $@/usr/include
	@rm -rf $@/usr/include/*
	@cp -f $(LIMINE)/limine.h $(SYSROOT)/usr/include
	@cp -rf lai/include/* $(SYSROOT)/usr/include/
	@cp -rf kernel/include/* $(SYSROOT)/usr/include/
	@cp -rf libk/include/* $(SYSROOT)/usr/include/

.PHONY: libk kernel util
libk: $(SYSROOT)
	@SYSROOT=$(SYSROOT) make -j$(JOBS) -C libk

release:
	STRIP=strip make image.iso image.hdd

kernel: libk $(SYSROOT) lai/build/liblai.a
	@SYSROOT=$(SYSROOT) make -j$(JOBS) -C kernel

lai:
	@git clone https://github.com/managarm/lai.git --depth 1
	@patch -i patches/lai_meson_patch.diff lai/meson.build

lai/build/liblai.a: lai
	@mkdir -p $(dir $@)
	@cd lai && meson setup --cross-file ../patches/meson_cross.ini build && cd build && meson compile

util:
	@make -j $(JOBS) -C util bin/font.lime bin/pci_devices.reg bin/pci_vendors.reg

base: kernel util

hdd: image.hdd
iso: image.iso

image.hdd: base $(LIMINE) $(LIMINE_BINARY)
	@echo "Making image.hdd"
	@dd if=/dev/zero bs=1M count=0 seek=64 of=image.hdd
	@dd if=/dev/zero bs=1M count=1 of=image.hdd
	@dd if=/dev/zero bs=1M count=1 seek=63 of=image.hdd
	@sgdisk image.hdd -n 1:2048 -t 1:ef00
	@$(LIMINE_BINARY) bios-install image.hdd
	@mformat -i image.hdd@@1M
	@mmd -i image.hdd@@1M ::/boot
	@mcopy -i image.hdd@@1M kernel/bin/limix util/bin/font.lime util/bin/pci_devices.reg util/bin/pci_vendors.reg ::/boot
	@mmd -i image.hdd@@1M ::/boot/limine
	@mcopy -i image.hdd@@1M boot/limine.conf $(LIMINE)/limine-bios.sys ::/boot/limine
	@mmd -i image.hdd@@1M ::/EFI
	@mmd -i image.hdd@@1M ::/EFI/BOOT
	@mcopy -i image.hdd@@1M $(LIMINE)/BOOTX64.EFI ::/EFI/BOOT
	@mcopy -i image.hdd@@1M $(LIMINE)/BOOTIA32.EFI ::/EFI/BOOT

image.iso: base $(LIMINE) $(LIMINE_BINARY)
	@echo "Making image.iso"
	@mkdir -p iso_root
	@mkdir -p iso_root/boot
	@cp -vf kernel/bin/limix util/bin/font.lime util/bin/pci_devices.reg util/bin/pci_vendors.reg iso_root/boot/
	@mkdir -p iso_root/boot/limine
	@cp -vf boot/limine.conf $(LIMINE)/limine-bios.sys $(LIMINE)/limine-bios-cd.bin \
			$(LIMINE)/limine-uefi-cd.bin iso_root/boot/limine/
	@mkdir -p iso_root/EFI/BOOT
	@cp -vf $(LIMINE)/BOOTX64.EFI iso_root/EFI/BOOT/
	@cp -vf $(LIMINE)/BOOTIA32.EFI iso_root/EFI/BOOT/
	@xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
    		-no-emul-boot -boot-load-size 4 -boot-info-table \
    		--efi-boot boot/limine/limine-uefi-cd.bin \
    		-efi-boot-part --efi-boot-image --protective-msdos-label \
        	iso_root -o image.iso
	@$(LIMINE_BINARY) bios-install image.iso

limine:
	@git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
	#@make -j$(JOBS) -C limine

$(LIMINE_BINARY): $(LIMINE)
	mkdir -p $(dir $@)
	gcc -g -O2 -pipe -Wall -Wextra -std=c99 $(LIMINE)/limine.c -o $@

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
	qemu-system-x86_64 -hda image.hdd $(COMMON_QEMU_FLAGS) -s -S -no-reboot -d int,cpu_reset

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
	@rm -rf limine compile_commands.json lai

genComp:
	@mkdir -p build
	@bear -- make

rebuild:
	@make clean
	@make
