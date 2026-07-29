// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_MAIN_HPP
#define HEADER_JSTEST_GTK_MAIN_HPP

#include <vector>
#include <gtkmm.h>
#include <map>

extern bool m_verbose;

class Joystick;
class JoystickListWidget;
class JoystickTestWidget;
class JoystickMapWidget;
class JoystickCalibrationWidget;

class JoystickGui
{
private:
  std::unique_ptr<Joystick> m_joystick;
  std::unique_ptr<JoystickTestWidget> m_test_widget;
  std::unique_ptr<JoystickMapWidget> m_mapping_widget;
  std::unique_ptr<JoystickCalibrationWidget> m_calibration_widget;

public:
  JoystickGui(std::unique_ptr<Joystick> joystick,
              bool simple_ui,
              Gtk::Window* parent = nullptr);

  JoystickTestWidget* get_test_widget() const { return m_test_widget.get(); }

  void show_calibration_dialog();
  void show_mapping_dialog();
};


class Main : public Gtk::Application
{
private:
  static Main* current_;
public:
  static Main* current() { return current_; }

private:
  std::string datadir;
  bool m_simple_ui;

  std::map<std::string, std::unique_ptr<JoystickGui> > m_joystick_guis;

public:
  Main();
  ~Main();

  JoystickTestWidget* show_device_property_dialog(const std::string& filename, Gtk::Window* parent = nullptr);

  static Glib::RefPtr<Main> create();

  int run(int argc, char** argv) /* override only since gtkmm 3.4 ! */;

  std::string get_data_directory() const { return datadir; }
};

#endif

/* EOF */
