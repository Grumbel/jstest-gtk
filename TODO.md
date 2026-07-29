# TODO

Issues from source QA (2026-07). Ordered roughly by priority.

## Critical / high

- [ ] **Joystick ctor swallows open failures** (`src/joystick.cpp`)
  - On `runtime_error`, prints and continues with uninitialized `fd` / counts.
  - Destructor still `close(fd)` → UB if open failed.
  - Fix: initialize `fd = -1`, `axis_count` / `button_count` to 0; either rethrow or expose an invalid state; only `close` when `fd >= 0`.

- [ ] **Mapping dialog hide clears the wrong widget** (`src/main.cpp`)
  - `show_mapping_dialog`: `signal_hide` resets `m_calibration_widget` instead of `m_mapping_widget`.
  - Fix: reset `m_mapping_widget` on mapping dialog hide.

- [ ] **udev USB-ID lookup leaks / incomplete cleanup** (`Joystick::get_usb_id_pair_from_udev`)
  - Failure path (`!dev`) does not `udev_device_unref(input_dev)`.
  - No null check if `udev_device_new_from_subsystem_sysname` fails.
  - Fix: always unref devices created; handle null `input_dev`.

- [ ] **Config axis/button parsing uses `vector::insert` at index** (`load_config` in `joystick_config_files.cpp`)
  - Shifts elements instead of assigning slot N; sparse / out-of-order keys break labels.
  - Fix: `resize` to fit, then `operator[]` assign for `axis_N` / `button_N`.

- [ ] **Missing PS4 icon**
  - `data/mappings/ps4-dualshock4-mapping.config` references `ps4-dualshock4.png`; file absent under `data/icons/`.
  - Fix: add the asset or change/remove `icon_filename` (generic fallback already works).

## Medium

- [ ] **Devices opened twice on list refresh**
  - `get_joysticks()` opens each device, then the list opens again for activity highlight.
  - Fix: reuse one open path or lighter probe for enumeration.

- [ ] **Reconnect assumes stable `jsN`**
  - Unplug/replug order can change `js0`/`js1`; `reconnected()` may fail or bind wrong device.
  - Fix: match on USB id / name / phys path, not only device node name.

- [ ] **`--datadir` does not normalize trailing `/`**
  - Compile-time path ends with `/`; user path without slash breaks `icons/` and `mappings/` joins.
  - Fix: ensure datadir always ends with `/` after CLI parse (and document).

- [ ] **CMake links bare `udev`**
  - No `pkg_search_module` / find for libudev.
  - Fix: detect via pkg-config and use its cflags/libs.

- [ ] **Global `m_verbose` and `m_verbose and std::cout` style**
  - Works but is awkward and non-idiomatic.
  - Fix: prefer `if (m_verbose)` or a small log helper; consider non-global ownership later.

- [ ] **Xbox 360 mapping label**
  - `button_12=PS_Home` is a PlayStation label on an Xbox config.
  - Fix: correct or clear the label.

- [ ] **Unused `xbox360_small.png`**
  - Leftover from pre-config icon selection.
  - Fix: remove or use deliberately.

## Low / cleanup

- [ ] **Dead XML API on `Joystick`**
  - `write(XMLWriter&)`, `load(XMLReader&)`, and forward declares with no implementations.
  - Fix: remove stubs or implement if profiles are still desired.

- [ ] **Form-feed (`\\f`) page-break characters** in many `src/*` files
  - Fix: strip them.

- [ ] **`button_widget.hpp` missing gtkmm includes**
  - Relies on transitive includes.
  - Fix: include the ToggleButton / gdk headers it needs.

- [ ] **Stale path in comment** (`joystick_test_widget.cpp`)
  - Refers to `data/README.txt`; file is `data/mappings/README.txt`.
  - Fix: update comment.

- [ ] **Unused CMake `GIT_REPO_VERSION` / date**
  - Computed but not applied to the binary version (VERSION / `JSTEST_GTK_VERSION` is source of truth).
  - Fix: remove or wire into metadata if still wanted.

- [ ] **Activity highlight on analog axes**
  - Any non-zero axis bolds the list row; sticks rarely sit at exact 0.
  - Fix: deadzone threshold or only highlight on buttons / large deltas.

- [ ] **Heavy list refresh on every udev js event**
  - Full config reload + reopen all devices.
  - Fix: incremental add/remove or debounce.

## Notes

- Joydev is legacy; README disclaimer stands. Prefer evtest / evtest-qt for modern stacks.
- Version source of truth remains top-level `VERSION`; do not hardcode version strings.
- Datadir: compile-time `JSTEST_GTK_DATADIR` + `--datadir` override; no multi-path guessing.
