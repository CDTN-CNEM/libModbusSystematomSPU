{
  description = "Comunicate with the SystemAtom SPU using MODBUS-RTU";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" ] (system:
      let
        pkgs = import nixpkgs { inherit system; };
        nativeBuildInputs = with pkgs; [
          busybox
          cmake
          pkg-config
          stdenv
        ];
        buildInputs = with pkgs; [
          libmodbus
        ];
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = nativeBuildInputs;
        };
        packages.default = pkgs.stdenv.mkDerivation {
          name = "libModbusSystematomSPU";
          src = ./.;
          inherit nativeBuildInputs;
          inherit buildInputs;
        };
      }
    );
}
