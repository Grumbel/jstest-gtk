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

            meta = with lib; {
              description = "Simple GTK joystick tester and calibrator for the Linux joydev interface";
              homepage = "https://github.com/Grumbel/jstest-gtk";
              license = licenses.gpl3Plus;
              platforms = platforms.linux;
              mainProgram = "jstest-gtk";
            };
          };
        };

        apps = rec {
          default = jstest-gtk;

          jstest-gtk = {
            type = "app";
            program = "${self.packages.${system}.jstest-gtk}/bin/jstest-gtk";
            meta = {
              description = self.packages.${system}.jstest-gtk.meta.description;
            };
          };
        };

        checks = {
          reuse = pkgs.runCommand "reuse-lint" {
            src = lib.cleanSource ./.;
            nativeBuildInputs = [ pkgs.reuse ];
          } ''
            cp -rT "$src" src
            chmod -R u+w src
            cd src
            reuse lint
            mkdir "$out"
          '';
        };
      }
    );
}
