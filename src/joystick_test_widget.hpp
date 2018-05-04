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

class Joystick;
class JoystickGui;
class ButtonWidget;

class JoystickTestWidget : public Gtk::Window
{
private:
  JoystickGui& m_gui;
  Joystick& joystick;
  bool m_simple_ui;

  Gtk::VBox m_vbox;
  Gtk::Alignment alignment;
  Gtk::Label label;

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

public:
  JoystickTestWidget(JoystickGui& gui, Joystick& joystick, bool simple_ui);

  void axis_move(int number, int value);
  void button_move(int number, bool value);

  void on_calibrate();
  void on_mapping();

private:
  JoystickTestWidget(const JoystickTestWidget&);
  JoystickTestWidget& operator=(const JoystickTestWidget&);
};

#endif

/* EOF */
