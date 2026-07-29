[![Build Status](https://travis-ci.org/Grumbel/jstest-gtk.svg?branch=master)](https://travis-ci.org/Grumbel/jstest-gtk)

jstest-gtk
==========

> **Important: obsolete Linux joystick interface (joydev)**
>
> jstest-gtk talks to the old Linux **joydev** layer (`/dev/input/js*`).
> That interface is a compatibility shim. On modern systems the real
> driver path is **evdev** (`/dev/input/event*`), which is what the
> kernel, libinput, most desktop stacks, and almost all current games
> and engines actually use.
>
> **What that means in practice**
>
> - The live button/axis display can still be useful for a quick check
>   that a pad is seen as a joystick device.
> - **Calibration, axis remapping, and button remapping done in this
>   tool only change joydev.** They do **not** change the underlying
>   evdev device. Steam, SDL2 (in its usual configuration), Proton,
>   native games, and most other software will ignore those settings.
> - If a game “does not pick up” a calibration you set here, that is
>   expected: the game is almost certainly reading evdev (or a higher
>   abstraction built on it), not joydev.
>
> **Background (short)**
>
> Linux exposes input hardware through the input subsystem. Each
> physical device typically shows up as an **event** node
> (`/dev/input/eventN`) with rich, typed events (keys, absolute axes,
> hats, and so on). For older joystick-centric apps, the kernel can
> also present a simplified **js** node (`/dev/input/jsN`) with a fixed
> “axes + buttons” model and optional in-kernel correction coefficients.
> jstest-gtk configures that simplified view. It never rewrites the
> modern event stream.
>
> **Prefer these tools for modern testing**
>
> - [**evtest**](https://cgit.freedesktop.org/evtest/) — command-line
>   monitor for `/dev/input/event*` (often `sudo evtest`)
> - [**evtest-qt**](https://github.com/Grumbel/evtest-qt) — Qt GUI along
>   the same lines as jstest-gtk, but against **evdev**
>
> Use jstest-gtk only when you deliberately care about the legacy
> joydev nodes (old engines, specialized software, or debugging the
> compatibility layer itself).

jstest-gtk is a simple joystick tester based on Gtk+. The main window
lets you pick an attached joystick from a drop-down, shows button and
axis activity, and can remap axes/buttons and calibrate the device
(Linux joydev only; see the disclaimer above).

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

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```


Usage
-----

Once compiled you can start jstest-gtk with:

```bash
$ ./jstest-gtk
```

or if you know the device you want to test with:

```bash
$ ./jstest-gtk /dev/input/js0
```

The main window opens on the joystick tester. Use the drop-down at the top to switch devices; when none are connected the selector is disabled.


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

```bash
$ SDL_JOYSTICK_DEVICE="/dev/input/js0"
$ export SDL_JOYSTICK_DEVICE
```

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
