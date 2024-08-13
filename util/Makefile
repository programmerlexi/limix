bin/font.lime: font.asm
	@echo "Building font"
	@mkdir -p bin
	@nasm -f bin -o bin/font.lime font.asm

pci.ids:
	@wget https://pci-ids.ucw.cz/v2.2/pci.ids

bin/pci_devices.reg: pci_devices.asm generate_device_db.py pci.ids Makefile
	@echo "Creating PCI device table"
	@mkdir -p bin
	@python generate_device_db.py pci.ids > pci_devices_gen.asm
	@nasm -f bin -o bin/pci_devices_gen.reg pci_devices_gen.asm
	@nasm -f bin -o bin/pci_devices_w.reg pci_devices.asm
	@cat bin/pci_devices_gen.reg bin/pci_devices_w.reg > bin/pci_devices.reg
	#@gzip -f bin/pci_devices.reg.uncompressed
	#@mv bin/pci_devices.reg.uncompressed.gz bin/pci_devices.reg

bin/pci_vendors.reg: generate_vendor_db.py pci.ids Makefile
	@echo "Creating PCI vendor table"
	@mkdir -p bin
	@python generate_vendor_db.py pci.ids > pci_vendors.asm
	@nasm -f bin -o bin/pci_vendors.reg pci_vendors.asm
	#@gzip -f bin/pci_vendors.reg.uncompressed
	#@mv bin/pci_vendors.reg.uncompressed.gz bin/pci_vendors.reg

clean:
	@rm -rf bin pci_vendors.asm pci_devices_gen.asm
