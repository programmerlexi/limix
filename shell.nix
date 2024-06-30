{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs.buildPackages; [ gcc binutils gnumake python3 nasm xorriso mtools gptfdisk git qemu ];
}
