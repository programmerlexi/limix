bin/font.lime: font.asm
	@echo "Building font"
	@mkdir -p bin
	@nasm -f bin -o bin/font.lime font.asm

bin/pci_devices.reg: pci_devices.asm Makefile
	@echo "Creating PCI device table"
	@mkdir -p bin
	@python generate_device_db.py /usr/share/hwdata/pci.ids > pci_devices_gen.asm
	@nasm -f bin -o bin/pci_devices_gen.reg pci_devices_gen.asm
	@nasm -f bin -o bin/pci_devices_w.reg pci_devices.asm
	@cat bin/pci_devices_gen.reg > bin/pci_devices.reg

clean:
	@rm -rf bin
