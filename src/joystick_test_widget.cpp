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

#include <sstream>
#include <iostream>
#include <gtkmm/label.h>
#include <gtkmm/image.h>

#include "joystick.hpp"
#include "joystick_test_widget.hpp"

JoystickTestWidget::JoystickTestWidget(Joystick& joystick)
  : axis_frame("Axis"),
    button_frame("Button"),
    axis_table(joystick.get_axis_count(), 2),
    button_table(joystick.get_button_count() / 8 + 1, 8)
{
  button_on  = Gdk::Pixbuf::create_from_file("data/button_on.png");
  button_off = Gdk::Pixbuf::create_from_file("data/button_off.png");

  axis_frame.set_border_width(5);
  axis_table.set_border_width(5);
  axis_table.set_spacings(5);

  button_frame.set_border_width(5);

  for(int i = 0; i < joystick.get_axis_count(); ++i)
    {
      std::ostringstream str;
      str << "Axis " << i << ": ";
      Gtk::Label& label = *Gtk::manage(new Gtk::Label(str.str()));
      axis_table.attach(label, 0, 1, i, i+1, Gtk::SHRINK, Gtk::FILL);

      Gtk::ProgressBar& progressbar = *Gtk::manage(new Gtk::ProgressBar());
      progressbar.set_fraction(0.5);
      axis_table.attach(progressbar, 1, 2, i, i+1, Gtk::FILL|Gtk::EXPAND, Gtk::EXPAND);

      axes.push_back(&progressbar);
    }

  button_table.set_homogeneous();

  for(int i = 0; i < joystick.get_button_count(); ++i)
    {
      int x = i % 8;
      int y = i / 8;

      Gtk::Image& image = *Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_file("data/button_off.png")));
      button_table.attach(image, x, x+1, y, y+1);

      buttons.push_back(&image);
    }

  pack_start(axis_frame,   Gtk::PACK_EXPAND_WIDGET);
  pack_start(button_frame, Gtk::PACK_EXPAND_WIDGET);

  axis_frame.add(axis_table);
  button_frame.add(button_table);

  joystick.axis_move.connect(sigc::mem_fun(this, &JoystickTestWidget::axis_move));
  joystick.button_move.connect(sigc::mem_fun(this, &JoystickTestWidget::button_move));
}

void
JoystickTestWidget::axis_move(int number, int value)
{
  axes.at(number)->set_fraction((value + 32767) / (double)(2*32767));

  std::ostringstream str;
  str << value;
  axes.at(number)->set_text(str.str());
}

void
JoystickTestWidget::button_move(int number, bool value)
{
  if (value)
    buttons.at(number)->set(button_on);
  else
    buttons.at(number)->set(button_off);
}

/* EOF */
