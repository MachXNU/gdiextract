{
  description = "gdiextract";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "gdiextract";
          version = "1.0.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            ninja
          ];

          buildInputs = with pkgs; [
            argparse
          ];

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Debug"
          ];
        };

        apps.default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/gdiextract";
        };

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
            ninja
            gcc
            gdb
            clang-tools
            argparse
          ];
        };
      });
}