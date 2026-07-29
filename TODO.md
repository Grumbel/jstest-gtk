# TODO

Issues from source QA (2026-07). Ordered roughly by priority.

## Critical / high

- [x] **Joystick ctor swallows open failures** (`src/joystick.cpp`)
- [x] **Mapping dialog hide clears the wrong widget** (`src/main.cpp`)
- [x] **udev USB-ID lookup leaks / incomplete cleanup**
- [x] **Config axis/button parsing uses `vector::insert` at index**
- [x] **Missing PS4 icon** (commented; generic fallback)

## Medium

- [x] **Devices opened twice on list refresh**
  - List enumerates and opens each `/dev/input/jsN` once for both display and activity.
- [x] **Reconnect assumes stable `jsN`**
  - `reconnected()` tries the original node, then scans `js0..js31` matching name + USB id; updates `filename`/`js_id` on success.
- [x] **`--datadir` trailing `/`**
- [x] **CMake links bare `udev`**
- [x] **Global `m_verbose and std::cout` style**
  - Replaced with `if (m_verbose)`.
- [x] **Xbox 360 mapping label**
- [x] **Unused `xbox360_small.png`**

## Low / cleanup

- [x] **Dead XML API on `Joystick`**
- [x] **Form-feed characters**
- [x] **`button_widget.hpp` includes**
- [x] **Stale path in comment**
- [x] **Unused CMake `GIT_REPO_VERSION` / date**
- [x] **Activity highlight on analog axes** (threshold)
- [x] **Heavy list refresh on every udev js event**
  - 250ms debounced `schedule_refresh()`.

## Notes

- Joydev is legacy; README disclaimer stands. Prefer evtest / evtest-qt for modern stacks.
- Version source of truth remains top-level `VERSION`.
- Datadir: compile-time `JSTEST_GTK_DATADIR` + `--datadir` override; no multi-path guessing.
