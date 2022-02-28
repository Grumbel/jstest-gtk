{
  description = "A simple joystick tester based on Gtk+";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          jstest-gtk = pkgs.stdenv.mkDerivation {
            pname = "jstest-gtk";
            version = "0.1.1";
            src = nixpkgs.lib.cleanSource ./.;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkgconfig
            ];
            buildInputs = [
              pkgs.gtkmm3
            ];
           };
        };
        defaultPackage = packages.jstest-gtk;
      });
}
