// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <sstream>
#include <iostream>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/dialog.h>
#include <glibmm/markup.h>
#include <glibmm/fileutils.h>

#include "main.hpp"
#include "joystick.hpp"
#include "button_widget.hpp"
#include "joystick_map_widget.hpp"
#include "joystick_calibration_widget.hpp"
#include "joystick_test_widget.hpp"
#include "joystick_config_files.hpp"

JoystickTestWidget::~JoystickTestWidget() = default;

JoystickTestWidget::JoystickTestWidget(bool simple_ui, const std::string& optional_device) :
  Gtk::Window(),
  m_simple_ui(simple_ui),
  label("", Gtk::ALIGN_START, Gtk::ALIGN_START),
  axis_frame("Axes"),
  button_frame("Buttons"),
  mapping_button("Mapping"),
  calibration_button("Calibration"),
  close_button(Gtk::Stock::CLOSE),
  stick1_widget(128, 128),
  stick2_widget(128, 128),
  stick3_widget(128, 128),
  rudder_widget(128, 32),
  throttle_widget(32, 128),
  left_trigger_widget(32, 128, true),
  right_trigger_widget(32, 128, true)
{
  set_title("jstest-gtk");
  set_icon_from_file(Main::current()->get_data_directory() + "icons/generic.png");
  set_default_size(520, 400);

  label.set_use_markup(true);
  label.set_selectable(true);

  axis_frame.set_border_width(5);
  axis_table.set_border_width(5);
  axis_table.set_spacings(5);
  button_frame.set_border_width(5);
  button_table.set_border_width(5);
  button_table.set_spacings(8);
  buttonbox.set_border_width(5);
  stick_hbox.set_border_width(5);

  m_device_combo.set_hexpand(true);
  m_device_combo.signal_changed().connect(
    sigc::mem_fun(this, &JoystickTestWidget::on_device_changed));

  alignment.set_padding(8, 8, 8, 8);
  alignment.add(label);

  buttonbox.add(mapping_button);
  buttonbox.add(calibration_button);
  buttonbox.add(close_button);

  test_hbox.pack_start(axis_frame,   Gtk::PACK_EXPAND_WIDGET);
  test_hbox.pack_start(button_frame, Gtk::PACK_EXPAND_WIDGET);

  m_vbox.pack_start(m_device_combo, Gtk::PACK_SHRINK);
  m_vbox.pack_start(alignment, Gtk::PACK_SHRINK);
  m_vbox.pack_start(test_hbox, Gtk::PACK_EXPAND_WIDGET);
  m_vbox.pack_end(buttonbox, Gtk::PACK_SHRINK);
  add(m_vbox);

  axis_vbox.pack_start(stick_hbox, Gtk::PACK_SHRINK);
  axis_vbox.add(axis_table);
  axis_frame.add(axis_vbox);
  button_frame.add(button_table);

  calibration_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickTestWidget::on_calibrate));
  mapping_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickTestWidget::on_mapping));
  close_button.signal_clicked().connect([this]{ hide(); });

  udev_monitor.reset(new UdevMonitor());
  udev_monitor->signal_joystick_event.connect(
    sigc::mem_fun(this, &JoystickTestWidget::on_udev_js_event));

  refresh_device_list(optional_device);
  close_button.grab_focus();
}

void
JoystickTestWidget::clear_device_ui()
{
  m_axis_conn.disconnect();
  m_button_conn.disconnect();

  if (m_mapping_widget)
    m_mapping_widget.reset();
  if (m_calibration_widget)
    m_calibration_widget.reset();

  // Detach children from tables/hboxes (managed widgets are deleted with parents)
  for (auto* child : axis_table.get_children())
    axis_table.remove(*child);
  for (auto* child : button_table.get_children())
    button_table.remove(*child);
  for (auto* child : stick_hbox.get_children())
    stick_hbox.remove(*child);

  axes.clear();
  buttons.clear();
  axis_callbacks.clear();
}

void
JoystickTestWidget::set_empty_state()
{
  clear_device_ui();
  m_joystick.reset();
  m_connected = false;

  m_updating_combo = true;
  m_device_combo.remove_all();
  m_device_combo.append("none", "No joystick connected");
  m_device_combo.set_active_id("none");
  m_device_combo.set_sensitive(false);
  m_updating_combo = false;

  set_title("jstest-gtk");
  label_base = "<i>No joystick connected</i>";
  label.set_markup(label_base);
  mapping_button.set_sensitive(false);
  calibration_button.set_sensitive(false);
}

void
JoystickTestWidget::build_device_ui()
{
  if (!m_joystick)
    return;

  Joystick& joystick = *m_joystick;

  set_title(joystick.get_name());
  label_base = "<b>" + Glib::Markup::escape_text(joystick.get_name()) + "</b>\n"
    "Device: " + Glib::Markup::escape_text(joystick.get_filename()) + "\n"
    "USB ID: " + Glib::Markup::escape_text(joystick.get_usb_id());
  label.set_markup(label_base);

  for (int i = 0; i < joystick.get_axis_count(); ++i)
  {
    std::ostringstream str;
    str << "Axis " << i;
    try {
      if (!joystick.js_cfg.axes.empty() &&
          i < static_cast<int>(joystick.js_cfg.axes.size()) &&
          !joystick.js_cfg.axes.at(i).empty())
      {
        str << " [" << joystick.js_cfg.axes[i] << "] ";
      }
    } catch (const std::out_of_range&) {}
    str << ": ";
    auto* lbl = Gtk::manage(new Gtk::Label(str.str()));
    lbl->set_xalign(0.0);

    Gtk::ProgressBar& progressbar = *Gtk::manage(new Gtk::ProgressBar());
    progressbar.set_fraction(0.5);

    int x = (i / 10) * 2;
    int y = i % 10;
    axis_table.attach(*lbl, x, x + 1, y, y + 1, Gtk::FILL, Gtk::SHRINK);
    axis_table.attach(progressbar, x + 1, x + 2, y, y + 1, Gtk::FILL | Gtk::EXPAND, Gtk::EXPAND);
    axes.push_back(&progressbar);
  }

  for (int i = 0; i < joystick.get_button_count(); ++i)
  {
    int x = i / 10;
    int y = i % 10;
    std::ostringstream str;
    str << i;
    try {
      if (!joystick.js_cfg.buttons.empty() &&
          i < static_cast<int>(joystick.js_cfg.buttons.size()) &&
          !joystick.js_cfg.buttons[i].empty())
      {
        str << " - " << joystick.js_cfg.buttons[i];
      }
    } catch (const std::out_of_range&) {}

    auto* button = Gtk::manage(new ButtonWidget());
    auto* blabel = Gtk::manage(new Gtk::Label(str.str()));
    blabel->set_xalign(0.0);
    blabel->set_margin_start(10);
    blabel->set_margin_end(10);
    button->add(*blabel);
    button_table.attach(*button, x, x + 1, y, y + 1, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);
    buttons.push_back(button);
  }

  axis_callbacks.clear();
  for (int i = 0; i < joystick.get_axis_count(); ++i)
    axis_callbacks.push_back(sigc::signal<void, double>());

  if (m_verbose)
  {
    std::cout << "joystick.get_name(): " << joystick.get_name() << std::endl;
    std::cout << "joystick.get_usb_id(): " << joystick.get_usb_id() << std::endl;
    std::cout << "joystick.get_js_type(): " << joystick.get_js_type() << std::endl;
    std::cout << "joystick.get_axis_count(): " << joystick.get_axis_count() << std::endl;
  }

  if (joystick.get_js_type() == "ps4-dualshock4")
    setup_dualshock4_equiv();
  else if (joystick.get_js_type() == "ps3-sixaxis")
    setup_sixaxis_equiv();
  else if (joystick.get_js_type() == "ps2-dualshock2")
    setup_dualshock2_equiv();
  else if (joystick.get_js_type() == "xbox360")
    setup_xbox360_equiv();
  else if (joystick.get_js_type() == "gamecube")
    setup_gamecube_equiv();
  else
  {
    switch (joystick.get_axis_count())
    {
    case 2:
      setup_joystick_widgets(1, {0, 1}, {});
      break;
    case 6:
    {
      Gtk::Table& table = *Gtk::manage(new Gtk::Table(2, 2));
      table.attach(stick1_widget, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
      table.attach(rudder_widget, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
      table.attach(throttle_widget, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
      stick_hbox.pack_start(table, Gtk::PACK_EXPAND_PADDING);
      stick_hbox.pack_start(stick3_widget, Gtk::PACK_EXPAND_PADDING);
      axis_callbacks[0].connect(sigc::mem_fun(stick1_widget, &AxisWidget::set_x_axis));
      axis_callbacks[1].connect(sigc::mem_fun(stick1_widget, &AxisWidget::set_y_axis));
      axis_callbacks[2].connect(sigc::mem_fun(rudder_widget, &RudderWidget::set_pos));
      axis_callbacks[3].connect(sigc::mem_fun(throttle_widget, &ThrottleWidget::set_pos));
      axis_callbacks[4].connect(sigc::mem_fun(stick3_widget, &AxisWidget::set_x_axis));
      axis_callbacks[5].connect(sigc::mem_fun(stick3_widget, &AxisWidget::set_y_axis));
      break;
    }
    case 7:
      setup_joystick_widgets(3, {0, 1, 3, 4, 5, 6}, {});
      break;
    case 8:
      setup_joystick_widgets(3, {0, 1, 2, 3, 6, 7}, {4, 5});
      break;
    default:
      if (m_verbose)
        std::cout << "No graphical stick layout for this axis count." << std::endl;
      break;
    }
  }

  if (m_simple_ui)
  {
    // hide stick graphics by removing from axis_vbox if packed
    if (stick_hbox.get_parent())
      axis_vbox.remove(stick_hbox);
  }
  else if (!stick_hbox.get_parent())
  {
    axis_vbox.pack_start(stick_hbox, Gtk::PACK_SHRINK);
    axis_vbox.reorder_child(stick_hbox, 0);
  }

  axis_table.show_all();
  button_table.show_all();
  stick_hbox.show_all();

  m_axis_conn = joystick.axis_move.connect(
    sigc::mem_fun(this, &JoystickTestWidget::axis_move));
  m_button_conn = joystick.button_press.connect(
    sigc::mem_fun(this, &JoystickTestWidget::button_press));

  m_connected = true;
  mapping_button.set_sensitive(true);
  calibration_button.set_sensitive(true);
}

void
JoystickTestWidget::refresh_device_list(const std::string& prefer_path)
{
  std::string previous;
  if (m_joystick)
    previous = m_joystick->get_filename();
  if (!prefer_path.empty())
    previous = prefer_path;

  struct Entry { std::string path; std::string js_id; std::string name; };
  std::vector<Entry> found;

  for (int i = 0; i < 32; ++i)
  {
    std::string js_id = "js" + std::to_string(i);
    std::string path = "/dev/input/" + js_id;
    if (!Glib::file_test(path, Glib::FILE_TEST_EXISTS))
      continue;
    try
    {
      Joystick probe(path, js_id);
      found.push_back({path, js_id, probe.get_name()});
    }
    catch (const std::exception& err)
    {
      if (m_verbose)
        std::cout << err.what() << std::endl;
    }
  }

  if (found.empty())
  {
    set_empty_state();
    return;
  }

  m_updating_combo = true;
  m_device_combo.set_sensitive(true);
  m_device_combo.remove_all();
  for (const auto& e : found)
  {
    std::string text = e.name + " (" + e.path + ")";
    m_device_combo.append(e.path, text);
  }

  std::string select = found.front().path;
  for (const auto& e : found)
  {
    if (e.path == previous)
    {
      select = e.path;
      break;
    }
  }
  m_device_combo.set_active_id(select);
  m_updating_combo = false;

  // Apply selection (may no-op rebuild if same path still open)
  on_device_changed();
}

void
JoystickTestWidget::on_device_changed()
{
  if (m_updating_combo)
    return;

  Glib::ustring id = m_device_combo.get_active_id();
  if (id.empty() || id == "none")
  {
    set_empty_state();
    return;
  }

  clear_device_ui();
  m_joystick.reset();

  std::string js_id = get_js_dev_id_from_filename(id);
  try
  {
    m_joystick.reset(new Joystick(id, js_id));
    build_device_ui();
  }
  catch (const std::exception& err)
  {
    std::cout << err.what() << std::endl;
    set_empty_state();
  }
}

void
JoystickTestWidget::schedule_refresh()
{
  if (m_refresh_timeout.connected())
    return;
  m_refresh_timeout = Glib::signal_timeout().connect(
    sigc::mem_fun(this, &JoystickTestWidget::on_refresh_timeout), 250);
}

bool
JoystickTestWidget::on_refresh_timeout()
{
  std::string prefer;
  if (m_joystick)
    prefer = m_joystick->get_filename();
  refresh_device_list(prefer);
  return false;
}

void
JoystickTestWidget::on_udev_js_event(const std::string& action, const std::string& devnode)
{
  if (m_verbose)
    std::cout << "joystick_test_widget " << action << ": " << devnode << std::endl;
  schedule_refresh();
}

void
JoystickTestWidget::on_calibrate()
{
  if (!m_joystick || !m_connected)
    return;
  if (m_calibration_widget)
  {
    m_calibration_widget->present();
    return;
  }
  m_calibration_widget.reset(new JoystickCalibrationWidget(*m_joystick));
  m_calibration_widget->signal_hide().connect([this] { m_calibration_widget.reset(); });
  m_calibration_widget->set_transient_for(*this);
  m_calibration_widget->show_all();
}

void
JoystickTestWidget::on_mapping()
{
  if (!m_joystick || !m_connected)
    return;
  if (m_mapping_widget)
  {
    m_mapping_widget->present();
    return;
  }
  m_mapping_widget.reset(new JoystickMapWidget(*m_joystick));
  m_mapping_widget->signal_hide().connect([this] { m_mapping_widget.reset(); });
  m_mapping_widget->set_transient_for(*this);
  m_mapping_widget->show_all();
}

void
JoystickTestWidget::setup_joystick_widgets(const unsigned sticks, const std::vector<unsigned>& axes, const std::vector<u_int>& triggers)
{ // eg: setup_joystick_widgets(2, {0,1,3,4}, {}); => 2 sticks, with axes 0,1 and 3,4, and no triggers
  try {
    if (sticks >= 1) {
      stick_hbox.pack_start(stick1_widget, Gtk::PACK_EXPAND_PADDING);
      axis_callbacks.at(axes.at(0)).connect(sigc::mem_fun(stick1_widget, &AxisWidget::set_x_axis));
      axis_callbacks.at(axes.at(1)).connect(sigc::mem_fun(stick1_widget, &AxisWidget::set_y_axis));
    }
    if (sticks >= 2) {
      stick_hbox.pack_start(stick2_widget, Gtk::PACK_EXPAND_PADDING);
      axis_callbacks.at(axes.at(2)).connect(sigc::mem_fun(stick2_widget, &AxisWidget::set_x_axis));
      axis_callbacks.at(axes.at(3)).connect(sigc::mem_fun(stick2_widget, &AxisWidget::set_y_axis));
    }
    if (sticks >= 3) {
      stick_hbox.pack_start(stick3_widget, Gtk::PACK_EXPAND_PADDING);
      axis_callbacks.at(axes.at(4)).connect(sigc::mem_fun(stick3_widget, &AxisWidget::set_x_axis));
      axis_callbacks.at(axes.at(5)).connect(sigc::mem_fun(stick3_widget, &AxisWidget::set_y_axis));
    }
  }
  catch (const std::out_of_range& e) {
    std::cout << "joystick configuration error. Some axis data missing or out of range." << std::endl;
    if (m_verbose)
      std::cout << e.what() << std::endl;
    label_base = label_base + "\n<span foreground='red'>ERROR: axis config data</span>";
    label.set_label(label_base);
  }

  try {
    if (triggers.size() >= 1) {
      stick_hbox.pack_start(left_trigger_widget, Gtk::PACK_EXPAND_PADDING);
      axis_callbacks.at(triggers.at(0)).connect(sigc::mem_fun(left_trigger_widget, &ThrottleWidget::set_pos));
    }
    if (triggers.size() >= 2) {
      stick_hbox.pack_start(right_trigger_widget, Gtk::PACK_EXPAND_PADDING);
      axis_callbacks.at(triggers.at(1)).connect(sigc::mem_fun(right_trigger_widget, &ThrottleWidget::set_pos));
    }
  }
  catch (const std::out_of_range& e) {
    std::cout << "joystick configuration error. Some trigger data missing or out of range." << std::endl;
    if (m_verbose)
      std::cout << e.what() << std::endl;
    label_base = label_base + "\n<span foreground='red'>ERROR: trigger config data</span>";
    label.set_label(label_base);
  }
}


   // (sticks,  axes,  triggers)
   // eg: setup_joystick_widgets(2, {0,1,3,4}, {}); => 2 sticks, with axes 0,1 and 3,4, and no triggers
void
JoystickTestWidget::setup_sixaxis_equiv()
{
  setup_joystick_widgets(2, {0,1,3,4}, {2,5});
}

void
JoystickTestWidget::setup_dualshock4_equiv()
{
  setup_joystick_widgets(3, {0,1,3,4,6,7}, {2,5});
}

void
JoystickTestWidget::setup_dualshock2_equiv()
{
  setup_joystick_widgets(2, {0,1,3,2}, {});
}

void
JoystickTestWidget::setup_xbox360_equiv()
{
  setup_joystick_widgets(3, {0,1,3,4,6,7}, {2,5});
}

void
JoystickTestWidget::setup_gamecube_equiv()
{
  setup_joystick_widgets(3, {0,1,5,2,6,7}, {3,4});
}


void
JoystickTestWidget::axis_move(int number, int value)
{
  if (number < 0 || number >= static_cast<int>(axes.size()))
    return;
  axes.at(number)->set_fraction((value + 32767) / (double)(2*32767));

  std::ostringstream str;
  str << value;
  axes.at(number)->set_text(str.str());
  axis_callbacks[number](value / 32767.0);
}

void
JoystickTestWidget::button_press(int number, bool value)
{
  if (number < 0 || number >= static_cast<int>(buttons.size()))
    return;
  buttons.at(number)->set_active(value);
}

