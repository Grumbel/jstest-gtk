/*
**  udev monitor helper code
**  Copyright (C) 2025 Raphael Rosch <jstest-bugs@insaner.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_UDEV_MONITOR_HELPER_HPP
#define HEADER_UDEV_MONITOR_HELPER_HPP

#include <glibmm.h>
#include <libudev.h>
#include <string>
#include <sigc++/signal.h>

class UdevMonitor {
public:
  UdevMonitor();
  ~UdevMonitor();

  sigc::signal<void, std::string, std::string> signal_joystick_event;

private:
  struct udev* udev = nullptr;
  struct udev_monitor* monitor = nullptr;
  int fd = -1;
  sigc::connection watch;

  bool on_io_event(Glib::IOCondition);
};

#endif // HEADER_UDEV_MONITOR_HELPER_HPP

/* EOF */

