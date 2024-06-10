[bits 64]

device_80862922:
.start:
dw 0x8086, 0x2922
dw .end-.start
db "6 port SATA Controller [AHCI mode]"
.end:

device_808629c0:
.start:
dw 0x8086, 0x29c0
dw .end-.start
db "Express DRAM Controller"
.end:

device_80862918:
.start:
dw 0x8086, 0x2918
dw .end-.start
db "LPC Interface Controller"
.end:

device_80862930:
.start:
dw 0x8086, 0x2930
dw .end-.start
db "SMBus Controller"
.end:

device_808610d3:
.start:
dw 0x8086, 0x10d3
dw .end-.start
db "82574L Gigabit Network Connection"
.end:

device_80862934:
.start:
dw 0x8086, 0x2934
dw .end-.start
db "82801I (ICH9 Family) USB UHCI Controller #1"
.end:

device_80862935:
.start:
dw 0x8086, 0x2935
dw .end-.start
db "82801I (ICH9 Family) USB UHCI Controller #2"
.end:

device_80862936:
.start:
dw 0x8086, 0x2936
dw .end-.start
db "82801I (ICH9 Family) USB UHCI Controller #3"
.end:

device_8086293a:
.start:
dw 0x8086, 0x293a
dw .end-.start
db "82801I (ICH9 Family) USB EHCI Controller #1"
.end:

device_12341111:
.start:
dw 0x1234, 0x1111
dw .end-.start
db "QEMU stdvga"
.end:

device_1b36000d:
.start:
dw 0x1b36, 0x000d
dw .end-.start
db "QEMU XHCI Host Controller"
.end:

device_1b360001:
.start:
dw 0x1b36, 0x0001
dw .end-.start
db "QEMU PCI-PCI Bridge"
.end:
