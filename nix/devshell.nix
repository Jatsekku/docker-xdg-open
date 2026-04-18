# This file is responsible for setting dev environment
# It defines binaries that will be provided and logic that will be run
# when 'nix develop .' will be executed
{ pkgs }:
let
  displayToolsVersions = ''
    printf '%.0s-' {1..80}; printf '\n'

    # cargo version
    if command -v cargo >/dev/null 2>&1; then
        CARGO_VER=$(cargo -V | grep -oP '(?<=cargo )\d+\.\d+\.\d+')
        echo "cargo: $CARGO_VER"
    fi

    # clippy version
    if command -v cargo >/dev/null 2>&1; then
        CLIPPY_VER=$(clippy-driver -V | grep -oP '(?<=clippy )\d+\.\d+\.\d+')
        echo "clippy: $CLIPPY_VER"
    fi

    # rustc version
    if command -v rustc >/dev/null 2>&1; then
        RUSTC_VER=$(rustc -V | grep -oP '(?<=rustc )\d+\.\d+\.\d+')
        echo "rustc: $RUSTC_VER"
    fi

    # rustfmt version
    if command -v rustfmt >/dev/null 2>&1; then
        RUSTFMT_VER=$(rustfmt --version | grep -oP '(?<=rustfmt )\d+\.\d+\.\d+')
        echo "rustfmt: $RUSTFMT_VER"
    fi

    # rust-analyzer
    if command -v rust-analyzer >/dev/null 2>&1; then
        RUST_ANALYZER_VER=$(rust-analyzer -V | grep -oP '(?<=rust-analyzer )\d+\-\d+\-\d+')
        echo "rust-analyzer: $RUST_ANALYZER_VER"
    fi

    printf '%.0s-' {1..80}; printf '\n'
  '';

  displayGreetings = ''
    echo "Rust environment 🦀"
  '';
in
{
  default = pkgs.mkShell {
    # Nix packages provided in environment
    buildInputs = with pkgs; [
      cargo
      clippy
      rustc
      rustfmt
      rust-analyzer
    ];

    # Environment variables
    env = {
      RUST_SRC_PATH = pkgs.rust.packages.stable.rustPlatform.rustLibSrc;
    };

    # Logic run on environment activation
    shellHook = ''
      ${displayGreetings}
      ${displayToolsVersions}
    '';
  };

}
