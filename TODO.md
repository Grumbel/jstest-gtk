# TODO

Issues from source QA (2026-07). Ordered roughly by priority.

## Critical / high

- [x] **Joystick ctor swallows open failures** (`src/joystick.cpp`)
  - Fixed: `fd = -1` / counts defaulted; ctor rethrows on open failure; destructor only `close`s when `fd >= 0`.

- [x] **Mapping dialog hide clears the wrong widget** (`src/main.cpp`)
  - Fixed: mapping `signal_hide` resets `m_mapping_widget`.

- [x] **udev USB-ID lookup leaks / incomplete cleanup** (`Joystick::get_usb_id_pair_from_udev`)
  - Fixed: null-check `input_dev`; always unref on failure paths; null-safe sysattr reads.

- [x] **Config axis/button parsing uses `vector::insert` at index** (`load_config`)
  - Fixed: `resize` + index assign for `axis_N` / `button_N`.

- [x] **Missing PS4 icon**
  - Fixed: commented `icon_filename` so UI uses generic until an asset exists.

## Medium

- [ ] **Devices opened twice on list refresh**
  - `get_joysticks()` opens each device, then the list opens again for activity highlight.
  - Fix: reuse one open path or lighter probe for enumeration.

- [ ] **Reconnect assumes stable `jsN`**
  - Partially improved: early failure paths close `tmp_fd`; still matches by path/name/usb id only.
  - Full fix: match on stable identity independent of `jsN` reassignment.

- [x] **`--datadir` does not normalize trailing `/`**
  - Fixed: append `/` when missing after CLI parse.

- [x] **CMake links bare `udev`**
  - Fixed: `pkg_search_module(UDEV REQUIRED libudev)` and use cflags/libs.

- [ ] **Global `m_verbose` and `m_verbose and std::cout` style**
  - Partially cleaned in some paths (`if (m_verbose)`); many call sites remain.

- [x] **Xbox 360 mapping label**
  - Fixed: cleared bogus `button_12=PS_Home`.

- [x] **Unused `xbox360_small.png`**
  - Removed; REUSE annotation updated.

## Low / cleanup

- [x] **Dead XML API on `Joystick`**
  - Removed `write`/`load` declarations and XML forward declares.

- [x] **Form-feed (`\\f`) page-break characters** in many `src/*` files
  - Stripped from sources.

- [x] **`button_widget.hpp` missing gtkmm includes**
  - Includes `gtkmm/togglebutton.h` and gdk headers.

- [x] **Stale path in comment** (`joystick_test_widget.cpp`)
  - Updated to `data/mappings/README.txt`.

- [ ] **Unused CMake `GIT_REPO_VERSION` / date**
  - Still computed; not applied to binary version.

- [x] **Activity highlight on analog axes**
  - Deadzone threshold (abs ≥ 2000) before bolding list rows.

- [ ] **Heavy list refresh on every udev js event**
  - Full config reload + reopen all devices; still open for incremental/debounce work.

## Notes

- Joydev is legacy; README disclaimer stands. Prefer evtest / evtest-qt for modern stacks.
- Version source of truth remains top-level `VERSION`; do not hardcode version strings.
- Datadir: compile-time `JSTEST_GTK_DATADIR` + `--datadir` override; no multi-path guessing.
