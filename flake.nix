{
  description = "Limix build env";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    limine-bootloader = { url = "github:limine-bootloader/limine?ref=v8.x-binary"; flake = false; };
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = { flake-parts, limine-bootloader, ... }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      perSystem = { pkgs, system, ... }:
        {
          devShells.default = import ./shell.nix { inherit pkgs; inherit limine-bootloader; };
        };
      };
}
