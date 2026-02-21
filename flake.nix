{
  description = "Nix flake template for c liibrary";

  # Flake inputs
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    inputs@{ self, ... }:
    let
      # List of all supported systems
      supportedSystems = inputs.nixpkgs.lib.systems.flakeExposed;

      # Function for providing system-specific attributes
      forEachSupportedSystem =
        f:
        inputs.nixpkgs.lib.genAttrs supportedSystems (
          system:
          f {
            # Nixpkgs configured per system
            pkgs = import inputs.nixpkgs {
              inherit system;
              # Allow usage of unfree packages
              config.allowUnfree = true;
            };
            inherit system;
          }
        );

    in
    {
      # Generate devShell for each system
      devShells = forEachSupportedSystem ({ pkgs, ... }: import ./nix/devshell.nix { inherit pkgs; });

      # Set formatter for Nix
      formatter = forEachSupportedSystem ({ pkgs, ... }: pkgs.nixfmt);
    };
}
