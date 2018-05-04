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

#ifndef HEADER_JSTEST_GTK_MAIN_HPP
#define HEADER_JSTEST_GTK_MAIN_HPP

#include <vector>
#include <gtkmm.h>

class Joystick;
class JoystickListWidget;
class JoystickTestWidget;

class Main : public Gtk::Application
{
private:
  static Main* current_;
public:
  static Main* current() { return current_; }

private:
  std::string datadir;
  bool m_simple_ui;

  std::vector<Joystick*>    joysticks;
  std::vector<Gtk::Dialog*> dialogs;

  void on_dialog_hide(Gtk::Dialog* dialog);

public:
  Main();
  ~Main();

  JoystickTestWidget* show_device_property_dialog(const std::string& filename, Gtk::Window* parent = nullptr);
  void show_calibration_dialog(Joystick& joystick);
  void show_mapping_dialog(Joystick& joystick);

  static Glib::RefPtr<Main> create();

  int run(int argc, char** argv) /* override only since gtkmm 3.4 ! */;

  std::string get_data_directory() const { return datadir; }
};

#endif

/* EOF */
