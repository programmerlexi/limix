{ pkgs ? import <nixpkgs> {}, limine-bootloader }:
  pkgs.pkgsCross.x86_64-embedded.mkShell {
    name = "limix";
    packages = with pkgs; [ wget gcc binutils gnumake python3 nasm xorriso mtools gptfdisk git qemu meson ninja gdb ];
    LIMINE = "${limine-bootloader}";
}
