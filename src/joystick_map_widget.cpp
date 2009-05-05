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

#include <gtkmm/stock.h>

#include "evdev_helper.hpp"
#include "joystick.hpp"
#include "joystick_map_widget.hpp"

JoystickMapWidget::JoystickMapWidget(Joystick& joystick)
  : label("Change the order of axis and button. The order applies directly to the "
          "joystick kernel driver, so it will work in any game, it is however not "
          "persistant across reboots."),
    axis_map("Axis"),
    button_map("Button")
{
  set_border_width(5);
  label.set_line_wrap();

  hbox.add(axis_map);
  hbox.add(button_map);

  pack_start(label, Gtk::PACK_SHRINK);
  pack_start(hbox, Gtk::PACK_EXPAND_WIDGET);

  const std::vector<int>& button_mapping = joystick.get_button_mapping();
  for(std::vector<int>::const_iterator i = button_mapping.begin(); i != button_mapping.end(); ++i)
    {
      button_map.add(btn2str(*i));
    }

  const std::vector<int>& axis_mapping = joystick.get_axis_mapping();
  for(std::vector<int>::const_iterator i = axis_mapping.begin(); i != axis_mapping.end(); ++i)
    {
      axis_map.add(abs2str(*i));
    }
}

/* EOF */
