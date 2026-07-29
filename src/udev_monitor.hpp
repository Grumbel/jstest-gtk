// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

