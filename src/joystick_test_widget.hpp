// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_JOYSTICK_TEST_WIDGET_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_TEST_WIDGET_HPP

#include <memory>
#include <vector>

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
#include <gtkmm/window.h>

#include "throttle_widget.hpp"
#include "rudder_widget.hpp"
#include "axis_widget.hpp"
#include "udev_monitor.hpp"

class Joystick;
class JoystickMapWidget;
class JoystickCalibrationWidget;
class ButtonWidget;

class JoystickTestWidget : public Gtk::Window
{
private:
  bool m_simple_ui;
  std::unique_ptr<Joystick> m_joystick;
  std::unique_ptr<JoystickMapWidget> m_mapping_widget;
  std::unique_ptr<JoystickCalibrationWidget> m_calibration_widget;

  bool m_connected = false;
  bool m_updating_combo = false;

  Gtk::VBox m_vbox;
  Gtk::ComboBoxText m_device_combo;
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

  std::vector<sigc::signal<void, double> > axis_callbacks;

  std::unique_ptr<UdevMonitor> udev_monitor;
  sigc::connection m_axis_conn;
  sigc::connection m_button_conn;
  sigc::connection m_refresh_timeout;

public:
  /** optional_device: path to prefer (e.g. CLI arg); empty means auto-pick first */
  JoystickTestWidget(bool simple_ui, const std::string& optional_device = std::string());
  ~JoystickTestWidget() override;

  void axis_move(int number, int value);
  void button_press(int number, bool value);

  void on_calibrate();
  void on_mapping();

private:
  void clear_device_ui();
  void build_device_ui();
  void set_empty_state();
  void refresh_device_list(const std::string& prefer_path = std::string());
  void schedule_refresh();
  bool on_refresh_timeout();
  void on_device_changed();
  void on_udev_js_event(const std::string& action, const std::string& devnode);

  void setup_joystick_widgets(const unsigned sticks,
                              const std::vector<unsigned>& axes,
                              const std::vector<unsigned>& triggers);
  void setup_sixaxis_equiv();
  void setup_dualshock2_equiv();
  void setup_dualshock4_equiv();
  void setup_xbox360_equiv();
  void setup_gamecube_equiv();

  JoystickTestWidget(const JoystickTestWidget&);
  JoystickTestWidget& operator=(const JoystickTestWidget&);
};

#endif

/* EOF */
