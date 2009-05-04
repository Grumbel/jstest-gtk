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

#ifndef HEADER_JSTEST_GTK_JOYSTICK_TEST_WIDGET_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_TEST_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/table.h>

class Joystick;

class JoystickTestWidget : public Gtk::VBox
{
private:
  Gtk::Frame axis_frame;
  Gtk::Frame button_frame;
  Gtk::Table axis_table;
  Gtk::Table button_table;

  std::vector<Gtk::ProgressBar*> axes;
  std::vector<Gtk::Image*>       buttons;

  Glib::RefPtr<Gdk::Pixbuf> button_on;
  Glib::RefPtr<Gdk::Pixbuf> button_off;

public:
  JoystickTestWidget(Joystick& joystick);

  void axis_move(int number, int value);
  void button_move(int number, bool value);

private:
  JoystickTestWidget(const JoystickTestWidget&);
  JoystickTestWidget& operator=(const JoystickTestWidget&);
};

#endif

/* EOF */
