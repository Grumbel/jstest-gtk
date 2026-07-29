// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtkmm.h>

#include "main.hpp"
#include "joystick.hpp"
#include "joystick_description.hpp"
#include "joystick_list_widget.hpp"

#include "joystick_config_files.hpp"


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
  Gtk::TreeModelColumn<int> font_weight;

private:
  DeviceListColumns() {
    add(icon);
    add(path);
    add(name);
    add(font_weight);
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
  set_icon_from_file(Main::current()->get_data_directory() + "icons/generic.png");
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
  auto name_renderer = Gtk::manage(new Gtk::CellRendererText);

  int name_col = treeview.append_column("Name", *name_renderer);
  auto column = treeview.get_column(name_col - 1);
  column->add_attribute(
    name_renderer->property_text(),
    DeviceListColumns::instance().name);
  column->add_attribute(
    name_renderer->property_weight(),
    DeviceListColumns::instance().font_weight);

  // Signals
  treeview.signal_row_activated().connect(sigc::mem_fun(this, &JoystickListWidget::on_row_activated));
  m_refresh_button.signal_clicked().connect([this]{ on_refresh_button(); });
  m_properties_button.signal_clicked().connect([this]{ on_properties_button(); });
  m_close_button.signal_clicked().connect([this]{ hide(); });
  m_close_button.grab_focus();

  udev_monitor.reset(new UdevMonitor());
  udev_monitor->signal_joystick_event.connect([this](const std::string& action, const std::string& devnode) {
    m_verbose and std::cout << "Joystick " << action << ": " << devnode << std::endl;
    on_refresh_button();
  });

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
  m_joysticks.clear();

  device_list->clear();

  // RELOAD CONFIG FILES
  joystick_configs = load_all_configs(Main::current()->get_data_directory() + "mappings");

  for(std::vector<JoystickDescription>::const_iterator i = joysticks.begin(); i != joysticks.end(); ++i)
  {
    Gtk::ListStore::iterator it = device_list->append();

    const Glib::ustring& name = i->name;
    Glib::ustring icon_filename;
    JoystickConfig js_cfg = get_config_for_usb_id(i->usb_id);

    icon_filename = js_cfg.icon_filename;

    if (! js_cfg.icon_filename_is_good) icon_filename = "generic.png"; // NOTE: you can set the icon_filename in a config file for the controller

    (*it)[DeviceListColumns::instance().icon] = Gdk::Pixbuf::create_from_file(
      Main::current()->get_data_directory() + "icons/" + icon_filename);
    (*it)[DeviceListColumns::instance().path] = i->filename;
    (*it)[DeviceListColumns::instance().font_weight] = Pango::WEIGHT_NORMAL;

    std::ostringstream out;
    out << name << "\n"
        << "Device: " << i->filename << "\n"
        // << "js_id: " << i->js_id << "\n"
        // << "vendor_id: " << i->vendor_id << "\n"
        // << "product_id: " << i->product_id << "\n"
        << "usb_id: " << i->usb_id << "\n"
        << "Axes: " << i->axis_count << "\n"
        << "Buttons: " << i->button_count;
    (*it)[DeviceListColumns::instance().name] = out.str();

    std::unique_ptr<Joystick> joystick(new Joystick(i->filename, i->js_id));

    joystick->axis_move.connect(sigc::bind(sigc::mem_fun(this, &JoystickListWidget::js_activity_analog), device_list->get_path(it)));
    joystick->button_press.connect(sigc::bind(sigc::mem_fun(this, &JoystickListWidget::js_activity_bool), device_list->get_path(it)));
    m_joysticks.push_back(std::move(joystick));
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

void JoystickListWidget::js_activity_highlight(int value, Gtk::TreeModel::Path path)
{
  if (Gtk::TreeModel::iterator it = device_list->get_iter(path))
  {
    Gtk::TreeModel::Row row = *it;
    if (value)
    {
      row[DeviceListColumns::instance().font_weight] = Pango::WEIGHT_BOLD;
    }
    else
    {
      row[DeviceListColumns::instance().font_weight] = Pango::WEIGHT_NORMAL;
    }
  }
}

void JoystickListWidget::js_activity_bool(int number, bool value, Gtk::TreeModel::Path path)
{
  js_activity_highlight(static_cast<int>(value), path);
}

void JoystickListWidget::js_activity_analog(int number, int value, Gtk::TreeModel::Path path)
{
  js_activity_highlight(value, path);
}

/* EOF */
