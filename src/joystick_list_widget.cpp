/*
**  jstest-gtk - A graphical joystick tester
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include <gtkmm.h>

#include "main.hpp"
#include "joystick.hpp"
#include "joystick_description.hpp"
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

JoystickListWidget::JoystickListWidget() :
  Gtk::Window(),
  m_vbox(),
  m_buttonbox(),
  m_refresh_button(Gtk::Stock::REFRESH),
  m_properties_button(Gtk::Stock::PROPERTIES),
  m_close_button(Gtk::Stock::CLOSE)

{
  set_title("Joystick Preferences");
  set_icon_from_file(Main::current()->get_data_directory() + "generic.png");
  set_default_size(450, 310);
  //set_border_width(5);

  scrolled.set_vexpand(true);
  //scrolled.set_border_width(5);
  scrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scrolled.add(treeview);

  m_buttonbox.set_border_width(5);
  m_buttonbox.pack_end(m_refresh_button);
  m_buttonbox.pack_end(m_properties_button);
  m_buttonbox.pack_end(m_close_button);

  m_vbox.add(scrolled);
  m_vbox.pack_end(m_buttonbox, Gtk::PACK_SHRINK);

  add(m_vbox);

  // Set model
  device_list = Gtk::ListStore::create(DeviceListColumns::instance());
  treeview.set_model(device_list);
  treeview.set_headers_visible(false);
  treeview.append_column("Icon", DeviceListColumns::instance().icon);
  treeview.append_column("Name", DeviceListColumns::instance().name);

  // Signals
  treeview.signal_row_activated().connect(sigc::mem_fun(this, &JoystickListWidget::on_row_activated));
  m_refresh_button.signal_clicked().connect([this]{ on_refresh_button(); });
  m_properties_button.signal_clicked().connect([this]{ on_properties_button(); });
  m_close_button.signal_clicked().connect([this]{ hide(); });

  m_close_button.grab_focus();

  on_refresh_button();
}

void
JoystickListWidget::on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
  Gtk::TreeModel::iterator it = treeview.get_model()->get_iter(path);
  if (it)
  {
    Main::current()->show_device_property_dialog((*it)[DeviceListColumns::instance().path], this);
  }
}

void
JoystickListWidget::on_refresh_button()
{
  const std::vector<JoystickDescription>& joysticks = Joystick::get_joysticks();

  device_list->clear();

  for(std::vector<JoystickDescription>::const_iterator i = joysticks.begin(); i != joysticks.end(); ++i)
  {
    Gtk::ListStore::iterator it = device_list->append();

	const Glib::ustring& name = i->name;
	Glib::ustring icon_filename;
	//Playstation icon for ps3 controller
	if(name == "Sony PLAYSTATION(R)3 Controller") icon_filename = "PS3.png";
	//Xbox icon for xbox controller
	else if(name.find("X-Box") != Glib::ustring::npos) icon_filename = "xbox360_small.png";
	//General icon for the rest
	else icon_filename = "generic.png";

	(*it)[DeviceListColumns::instance().icon] = Gdk::Pixbuf::create_from_file(Main::current()->get_data_directory() + icon_filename);
    (*it)[DeviceListColumns::instance().path] = i->filename;

    std::ostringstream out;
	out << name << "\n"
        << "Device: " << i->filename << "\n"
        << "Axes: " << i->axis_count << "\n"
        << "Buttons: " << i->button_count;
    (*it)[DeviceListColumns::instance().name] = out.str();
  }

  if (!joysticks.empty())
    treeview.get_selection()->select(device_list->children().begin());
}

void
JoystickListWidget::on_properties_button()
{
  Gtk::TreeModel::iterator it = treeview.get_selection()->get_selected();
  if (it)
  {
    Main::current()->show_device_property_dialog((*it)[DeviceListColumns::instance().path], this);
  }
}

/* EOF */
