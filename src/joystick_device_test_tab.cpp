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

#include <gtkmm/label.h>
#include <gtkmm/image.h>

#include "joystick_device_test_tab.hpp"

JoystickDeviceTestTab::JoystickDeviceTestTab(int axis_count, int button_count)
  : axis_frame("Axis"),
    button_frame("Button"),
    axis_table(axis_count, 2),
    button_table(button_count % 8 + 1, button_count / 8 + 1)
{
  axis_frame.set_border_width(5);
  button_frame.set_border_width(5);

  for(int i = 0; i < axis_count; ++i)
    {
      Gtk::Label& label = *Gtk::manage(new Gtk::Label("Axis 1"));
      axis_table.attach(label, 0, 1, i, i+1);

      Gtk::Image& image = *Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_file("data/axis.png")));
      axis_table.attach(image, 1, 2, i, i+1);
    }

  button_table.set_homogeneous();

  for(int i = 0; i < button_count; ++i)
    {
      int x = i % 8;
      int y = i / 8;

      Gtk::Image& image = *Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_file("data/button_off.png")));
      button_table.attach(image, x, x+1, y, y+1);
    }

  pack_start(axis_frame,   Gtk::PACK_EXPAND_WIDGET);
  pack_start(button_frame, Gtk::PACK_EXPAND_WIDGET);

  axis_frame.add(axis_table);
  button_frame.add(button_table);
}

/* EOF */
