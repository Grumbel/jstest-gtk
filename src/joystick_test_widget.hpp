// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_JOYSTICK_TEST_WIDGET_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_TEST_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/table.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/alignment.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/liststore.h>

#include "throttle_widget.hpp"
#include "rudder_widget.hpp"
#include "axis_widget.hpp"

#include "udev_monitor.hpp"

class Joystick;
class JoystickGui;
class ButtonWidget;

class JoystickTestWidget : public Gtk::Window
{
private:
  JoystickGui& m_gui;
  Joystick& joystick;
  bool m_simple_ui;

  bool connected;

  Gtk::VBox m_vbox;
  Gtk::Alignment alignment;
  Gtk::Label label;
  Glib::ustring label_base;

  Gtk::Frame axis_frame;
  Gtk::VBox  axis_vbox;
  Gtk::Frame button_frame;
  Gtk::Table axis_table;
  Gtk::Table button_table;
  Gtk::HBox  test_hbox;
  Gtk::HBox  stick_hbox;

  Gtk::Button mapping_button;
  Gtk::Button calibration_button;
  Gtk::Button close_button;
  Gtk::HButtonBox buttonbox;

  AxisWidget stick1_widget;
  AxisWidget stick2_widget;
  AxisWidget stick3_widget;

  RudderWidget   rudder_widget;
  ThrottleWidget throttle_widget;

  ThrottleWidget left_trigger_widget;
  ThrottleWidget right_trigger_widget;

  std::vector<Gtk::ProgressBar*> axes;
  std::vector<ButtonWidget*>     buttons;

  Glib::RefPtr<Gdk::Pixbuf> button_on;
  Glib::RefPtr<Gdk::Pixbuf> button_off;

  std::vector<sigc::signal<void, double> > axis_callbacks;

  std::unique_ptr<UdevMonitor> udev_monitor;

public:
  JoystickTestWidget(JoystickGui& gui, Joystick& joystick, bool simple_ui);

  void axis_move(int number, int value);
  void button_press(int number, bool value);

  void on_calibrate();
  void on_mapping();

private:
  JoystickTestWidget(const JoystickTestWidget&);
  JoystickTestWidget& operator=(const JoystickTestWidget&);
  void setup_joystick_widgets(const u_int sticks, const std::vector<u_int>& axes, const std::vector<u_int>& triggers);
  void setup_sixaxis_equiv();
  void setup_dualshock2_equiv();
  void setup_dualshock4_equiv();
  void setup_xbox360_equiv();
  void setup_gamecube_equiv();

  void on_udev_js_event(const std::string& action, const std::string& devnode);
};

#endif

/* EOF */
