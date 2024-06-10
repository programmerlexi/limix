bin/font.lime: font.asm
	@echo "Building font"
	@mkdir -p bin
	@nasm -f bin -o bin/font.lime font.asm

bin/pci_devices.reg: pci_devices.asm
	@echo "Creating PCI device table"
	@mkdir -p bin
	@nasm -f bin -o bin/pci_devices.reg pci_devices.asm

clean:
	@rm -rf bin
