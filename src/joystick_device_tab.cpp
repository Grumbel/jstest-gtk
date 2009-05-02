/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <gtkmm/stock.h>

#include "joystick_device_properties.hpp"
#include "joystick_device_tab.hpp"

class DeviceListColumns : public Gtk::TreeModel::ColumnRecord
{
private:
  static DeviceListColumns* instance_;

public:
  static DeviceListColumns& instance() {
    if (instance_)
      return *instance_;
    else
      return *(instance_ = new DeviceListColumns());
  }

  Gtk::TreeModelColumn<Glib::ustring> path;
  Gtk::TreeModelColumn<Glib::ustring> name;

private:
  DeviceListColumns() {
    add(path);
    add(name);
  }
};

DeviceListColumns* DeviceListColumns::instance_ = 0;

JoystickDeviceTab::JoystickDeviceTab()
  : label("Below is a list of available joysticks on the system. Press Refresh to "
          "update the list, press Properties to get a seperate device dialog."),
    frame("Device List"),
    buttonbox(Gtk::BUTTONBOX_SPREAD),
    refresh_button(Gtk::Stock::REFRESH),
    properties_button(Gtk::Stock::PROPERTIES)
{
  label.set_line_wrap();
  pack_start(label, Gtk::PACK_SHRINK, 16);

  scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scrolled.add(treeview);
  frame.add(scrolled);
  add(frame);

  buttonbox.add(refresh_button);
  buttonbox.add(properties_button);
  pack_start(buttonbox, Gtk::PACK_SHRINK, 16);

  refresh_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickDeviceTab::on_refresh));
  properties_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickDeviceTab::on_properties));

  // Set model
  device_list = Gtk::ListStore::create(DeviceListColumns::instance());
  treeview.set_model(device_list);

  treeview.append_column("Path", DeviceListColumns::instance().path);
  treeview.append_column("Name", DeviceListColumns::instance().name);
}

void
JoystickDeviceTab::on_refresh()
{
  Gtk::ListStore::iterator it;

  it = device_list->append();
  (*it)[DeviceListColumns::instance().path] = "/dev/input/js0";
  (*it)[DeviceListColumns::instance().name] = "Microsoft Sidewinder";

  it = device_list->append();
  (*it)[DeviceListColumns::instance().path] = "/dev/input/js1";
  (*it)[DeviceListColumns::instance().name] = "Logitech Joystick";
}

void
JoystickDeviceTab::on_properties()
{
  // FIXME: We need to cleanup this memleak
  JoystickDeviceProperties* properties = new JoystickDeviceProperties();
  properties->show();
}

/* EOF */
