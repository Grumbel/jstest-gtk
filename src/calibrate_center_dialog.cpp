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

#include <iostream>
#include <gtkmm/stock.h>

#include "joystick.hpp"
#include "calibrate_center_dialog.hpp"

CalibrateCenterDialog::CalibrateCenterDialog(Joystick& joystick_)
  : joystick(joystick_),
    label("Leave all axes untouched in their center and press ok.")
{
  set_border_width(5);
  label.set_line_wrap();
  get_vbox()->add(label);
  add_button(Gtk::Stock::OK, 0);
  add_button(Gtk::Stock::CANCEL, 1);
  connection = joystick.axis_move.connect(sigc::mem_fun(this, &CalibrateCenterDialog::on_axis_move));
}

void
CalibrateCenterDialog::on_response()
{
  hide();
}

void
CalibrateCenterDialog::on_axis_move(int id, int value)
{
  std::cout << "AxisMove: " << id << " " << value << std::endl;
}

/* EOF */
