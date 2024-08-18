{
  description = "Limix build env";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    limine-bootloader = { url = "github:limine-bootloader/limine?ref=v8.x-binary"; flake = false; };
  };

  outputs = { self, nixpkgs, limine-bootloader }:
    let pkgs = nixpkgs.legacyPackages."x86_64-linux"; in
    {
      devShells."x86_64-linux".default = import ./shell.nix { inherit pkgs; inherit limine-bootloader; };
    };
}
