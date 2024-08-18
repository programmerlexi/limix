{ pkgs ? import <nixpkgs> {}, limine-bootloader }:
  pkgs.mkShell {
    name = "limix";
    packages = with pkgs; [ gcc binutils gnumake python3 nasm xorriso mtools gptfdisk git qemu bear c3c ];
    LIMINE = "${limine-bootloader}";
}
