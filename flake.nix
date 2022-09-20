{
  description = "A simple joystick tester based on Gtk+";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = jstest-gtk;

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
      }
    );
}
