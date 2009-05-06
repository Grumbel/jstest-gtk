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

#include "main.hpp"
#include "device_property_dialog.hpp"
#include "joystick_list_widget.hpp"

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

  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
  Gtk::TreeModelColumn<std::string>   path;
  Gtk::TreeModelColumn<Glib::ustring> name;

private:
  DeviceListColumns() {
    add(icon);
    add(path);
    add(name);
  }
};

DeviceListColumns* DeviceListColumns::instance_ = 0;

JoystickListWidget::JoystickListWidget()
  : label("Below is a list of available joysticks on the system. Press Refresh to "
          "update the list, press Properties to get a seperate device dialog. The "
          "devices listed are only joystick devices, not evdev devices or SDL "
          "devices, you can view the other ones via the top tab."),
    //frame("Device List"),
    buttonbox(Gtk::BUTTONBOX_SPREAD),
    refresh_button(Gtk::Stock::REFRESH),
    properties_button(Gtk::Stock::PROPERTIES)
{
  //frame.set_border_width(5);
  label.set_line_wrap();
  //pack_start(label, Gtk::PACK_SHRINK, 16);

  scrolled.set_border_width(5);
  scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scrolled.add(treeview);
  //frame.add(scrolled);
  //frame.add(treeview);
  //add(frame);
  add(scrolled);

  buttonbox.add(refresh_button);
  buttonbox.add(properties_button);
  buttonbox.set_border_width(5);
  pack_start(buttonbox, Gtk::PACK_SHRINK);

  refresh_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickListWidget::on_refresh));
  properties_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickListWidget::on_properties));

  // Set model
  device_list = Gtk::ListStore::create(DeviceListColumns::instance());
  treeview.set_model(device_list);
  treeview.set_headers_visible(false);
  treeview.append_column("Icon", DeviceListColumns::instance().icon);
  treeview.append_column("Name", DeviceListColumns::instance().name);
  //treeview.append_column("Path", DeviceListColumns::instance().path);

  on_refresh();
}

void
JoystickListWidget::on_refresh()
{
  const std::vector<JoystickDescription>& joysticks = Joystick::get_joysticks();

  device_list->clear();

  for(std::vector<JoystickDescription>::const_iterator i = joysticks.begin(); i != joysticks.end(); ++i)
    {
      Gtk::ListStore::iterator it = device_list->append();
      (*it)[DeviceListColumns::instance().icon] = Gdk::Pixbuf::create_from_file("data/xbox360_small.png");
      (*it)[DeviceListColumns::instance().path] = i->filename;
      (*it)[DeviceListColumns::instance().name] = "\n  " + i->name + "\n    Device: " + i->filename + "\n";
    }

  if (!joysticks.empty())
    treeview.get_selection()->select(device_list->children().begin());
}

void
JoystickListWidget::on_properties()
{
  Gtk::TreeModel::iterator it = treeview.get_selection()->get_selected();
  if (it)
    {
      Main::current()->show_device_property_dialog((*it)[DeviceListColumns::instance().path]);
    }
}

/* EOF */
