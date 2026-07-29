# AGENTS.md

## Project overview

**jstest-gtk** is a Linux GTK+ (gtkmm-3) graphical joystick tester and configurator. It talks to the classic Linux joystick API (`/dev/input/js*`), shows live axis/button state, and can remap axes/buttons and adjust calibration (deadzone, range, invert) via `ioctl` on the joydev device.

License: GPL-3.0-or-later. Original author: Ingo Ruhnke. Later work includes USB-ID-based layout configs and libudev hotplug (e.g. Raphael Rosch).

### What it does

- Lists attached joysticks (`JoystickListWidget`)
- Live test UI with progress bars, optional stick/trigger drawings (`JoystickTestWidget`)
- Axis/button remapping (`JoystickMapWidget` / `RemapWidget`) — applied to the kernel joydev mapping (not persistent across reboot)
- Calibration editor and a simple “move sticks to extremes” wizard (`JoystickCalibrationWidget`, `CalibrateMaximumDialog`)
- Optional per-controller config files under `data/` matched by USB vendor:product ID (via libudev)
- Hotplug: `UdevMonitor` refreshes the list and marks devices disconnected/reconnected in the test window

### Non-goals / platform limits

- Linux-only (joydev + udev)
- Calibration/mapping apply to **joydev**, not raw evdev; some SDL builds ignore joydev unless forced (see `README.md`)
- No Windows/macOS support

## Layout

| Path | Role |
|------|------|
| `src/main.cpp` / `main.hpp` | App entry, CLI (`--datadir`, `--simple`, `--verbose`), owns GUIs |
| `src/joystick.*` | Device open, events, calibration/mapping ioctls, USB ID via udev |
| `src/joystick_list_widget.*` | Device list, icons, activity highlight, udev refresh |
| `src/joystick_test_widget.*` | Per-device test UI, type-specific stick layouts |
| `src/joystick_calibration_widget.*` | Manual calibration UI |
| `src/joystick_map_widget.*` / `remap_widget.*` | Reorder axes/buttons |
| `src/joystick_config_files.*` | Load `data/*-mapping.config` |
| `src/udev_monitor.*` | Netlink udev watch for `/js` nodes |
| `src/evdev_helper.*` | Name helpers for ABS_/BTN_ codes (mapping labels) |
| `src/*_widget.*` | Cairo stick/rudder/throttle drawing; buttons are `Gtk::ToggleButton` |
| `data/` | Icons + mapping configs |
| `CMakeLists.txt` | Build/install; uses `GNUInstallDirs` |
| `jstest-gtk.sh.in` | Installed wrapper: runs libexec binary with `--datadir` |

### Data directory

- Compile-time default: `JSTEST_GTK_DATADIR` = `${CMAKE_INSTALL_FULL_DATADIR}/jstest-gtk/` (from CMake)
- Runtime override: `--datadir DIR`
- Icons and configs are expected under that directory
- **Note:** config loading currently still calls `load_all_configs("data")` in places (CWD-relative); should use `Main::current()->get_data_directory()` for installed runs

### Mapping configs (`data/*-mapping.config`)

Simple `key=value` files (`#` comments). Important keys:

- `js_type` — selects graphical layout (e.g. `xbox360`, `ps4-dualshock4`, `gamecube`)
- `usb_id` — one or more `vvvv:pppp` lines to match
- `icon_filename` — PNG under the data dir
- `axis_N` / `button_N` — display labels

Unknown devices fall back to axis-count heuristics and `generic.png`.

## Build

Dependencies: CMake ≥ 3.5, gtkmm-3.0, sigc++-2.0, X11, **libudev**, threads.

```bash
mkdir build && cd build
cmake ..
make
```

Install layout:

- Binary → `${CMAKE_INSTALL_LIBEXECDIR}/jstest-gtk`
- Wrapper → `${CMAKE_INSTALL_BINDIR}/jstest-gtk` (passes `--datadir`)
- Data → `${CMAKE_INSTALL_DATADIR}/jstest-gtk/`

Optional Nix flake is present (`flake.nix`).

## CLI

```
jstest-gtk [OPTIONS] [DEVICE]
  -h, --help
  -v, --version
  --simple          Hide stick graphics
  --verbose         Extra logging (`m_verbose`)
  --datadir DIR     Data/config/icon directory
  DEVICE            e.g. /dev/input/js0 (skip list, open test UI)
```

## Working guidelines

- Each change shall be followed by a detailed git commit message at the end.
- Prefer compile-time datadir + explicit `--datadir`; do not add multi-path “guess where data lives” logic unless asked.
- Keep joydev calibration/mapping behavior intact when touching device code; test paths go through `Joystick`.
- New controller types: add a `data/*-mapping.config`, icon if needed, document `js_type` in `data/README.txt`, and a `setup_*_equiv()` path in `JoystickTestWidget` if a custom stick layout is required.
