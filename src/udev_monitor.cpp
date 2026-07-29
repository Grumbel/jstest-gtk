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


#include "udev_monitor.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>

UdevMonitor::UdevMonitor()
{
  udev = udev_new();
  if (!udev) throw std::runtime_error("udev_new() failed");

  monitor = udev_monitor_new_from_netlink(udev, "udev");
  if (!monitor) throw std::runtime_error("udev_monitor_new_from_netlink() failed");
  if (udev_monitor_filter_add_match_subsystem_devtype(monitor, "input", nullptr) < 0) {
    udev_monitor_unref(monitor);
    udev_unref(udev);
    throw std::runtime_error("Failed to add subsystem filter");
  }
  if (udev_monitor_enable_receiving(monitor) < 0) {
    udev_monitor_unref(monitor);
    udev_unref(udev);
    throw std::runtime_error("udev monitor enable receiving failed");
  }

  fd = udev_monitor_get_fd(monitor);
  if (fd < 0) {
      udev_monitor_unref(monitor);
      udev_unref(udev);
      throw std::runtime_error("Invalid file descriptor from udev monitor");
    }

  auto channel = Glib::IOChannel::create_from_fd(fd);
  channel->set_encoding();  // Binary
  channel->set_buffered(false);

  watch = Glib::signal_io().connect(
    sigc::mem_fun(*this, &UdevMonitor::on_io_event),
    channel,
    Glib::IO_IN);
}

UdevMonitor::~UdevMonitor()
{
  if (watch) {
    watch.disconnect();  // disconnect GLib watch
  }
  if (monitor) {
    udev_monitor_unref(monitor);
    monitor = nullptr;
  }
  if (udev) {
    udev_unref(udev);
    udev = nullptr;
  }
}

bool UdevMonitor::on_io_event(Glib::IOCondition)
{
  struct udev_device* dev = udev_monitor_receive_device(monitor);
  if (dev) {
    const char* action = udev_device_get_action(dev);
    const char* devnode = udev_device_get_devnode(dev);
    if (devnode && strstr(devnode, "/js")) {
      signal_joystick_event.emit(action ? action : "", devnode);
    }
    udev_device_unref(dev);
  }
  return true;  // Keep watching
}
