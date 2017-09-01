[![Build Status](https://travis-ci.org/Grumbel/jstest-gtk.svg?branch=master)](https://travis-ci.org/Grumbel/jstest-gtk)

jstest-gtk
==========

jstest-gtk is a simple joystick tester based on Gtk+. It provides you
with a list of attached joysticks, a way to display which buttons and
axis are pressed, a way to remap axis and buttons and a way to
calibrate your joystick.

You can find the latest version at:

 * http://github.com/Grumbel/jstest-gtk/tree/master

Questions, comments and bug reports can be given to:

 * Ingo Ruhnke <grumbel@gmail.com>


Compiling
---------

Make sure you have the following pieces of software installed (they
are part of most distributions):

 * cmake
 * sigc++
 * gtkmm

You can compile it by typing:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make


Usage
-----

Once compiled you can start jstest-gtk with:

    $ ./jstest-gtk

or if you know the device you want to test with:

    $ ./jstest-gtk /dev/input/js0

The main window will provide you with a list of currently available
joysticks. When you double click a joystick it will display the Test
dialog, allowing you to test buttons and axes on the joystick.

The mapping dialog allows you to reorder the buttons and axis. This is
useful for example when a game expects the dpad to be on axis 0 and 1,
while in reality it is often on axis 4 and 5 on a dual-analogstick
gamepad.

The calibration dialog allows you to manipulate some low level details
on how the raw axis values are mapped to the joystick device. For
example you can inverse an axis, disable an axis or remove its
deadzone, to gain more fine control on small movements. This is
especially useful as the default calibration values for most joysticks
give it a far bigger deadzone then needed, thus reducing your ability
for fine movements.


SDL Notes
---------

Depending on what version and what compilation flags where used, your
version of SDL might not use the joystick interface at all, but
instead use the /dev/input/event* device directly. That means that any
calibration or button/axis mapping will be ignored, as those apply to
the joystick interface, not evdev.

To work around that you have to set an environment variable that
forces SDL to use the joystick device:

    $ SDL_JOYSTICK_DEVICE="/dev/input/js0"
    $ export SDL_JOYSTICK_DEVICE

When you need two or more joysticks this will not work and you might
need to recompile SDL and disable the evdev support.

More info can be found at:

 * ftp://ptah.lnf.kth.se/pub/misc/sdl-env-vars

`SDL_LINUX_JOYSTICK`:
  Special joystick configuration string for linux. The format is
  `name numaxes numhats numballs`
  where name is the name string of the joystick (possibly in single
  quotes), and the rest are the number of axes, hats and balls
  respectively.

`SDL_JOYSTICK_DEVICE`:
  Joystick device to use in the linux joystick driver, in addition to
  the usual: `/dev/js*`, `/dev/input/event*`, `/dev/input/js*`
