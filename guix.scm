;; jstest-gtk - A graphical joystick tester
;; Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(set! %load-path
  (cons* "/ipfs/QmeUZkZxWMHKNR4HAGU9G2VRFcyiSFt3JNpuF5RBC9wKZa/guix-cocfree_0.0.0-51-gc2bce55"
         %load-path))

(use-modules (guix packages)
             (guix build utils)
             (guix build-system cmake)
             ((guix licenses) #:prefix license:)
             (gnu packages gtk)
             (gnu packages pkg-config)
             (guix-cocfree utils))

(define %source-dir (dirname (current-filename)))

(define-public jstest-gtk
  (package
   (name "jstest-gtk")
   (version (version-from-source %source-dir))
   (source (source-from-source %source-dir #:version version))
   (arguments
    `(#:tests? #f))
   (inputs
    `(("gtkmm" ,gtkmm)
      ("pkg-config" ,pkg-config)))
   (build-system cmake-build-system)
   (synopsis (synopsis-from-source %source-dir))
   (description (description-from-source %source-dir))
   (home-page (homepage-from-source %source-dir))
   (license license:gpl3+)))

jstest-gtk

;; EOF ;;
