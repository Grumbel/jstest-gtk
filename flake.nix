{
  description = "A simple joystick tester based on Gtk+";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;
        versionBase = lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
        gitRev = "${self.shortRev or self.dirtyShortRev or "dirty"}";
        version = "${versionBase}+g${gitRev}";
      in {
        packages = rec {
          default = jstest-gtk;

          jstest-gtk = pkgs.stdenv.mkDerivation {
            pname = "jstest-gtk";
            inherit version;

            src = nixpkgs.lib.cleanSource ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ];

            buildInputs = with pkgs; [
              gtkmm3
              udev
            ];

            cmakeFlags = [
              "-DPROJECT_VERSION_FULL=${version}"
            ];
          };
        };
      }
    );
}
