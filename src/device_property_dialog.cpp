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

#include "device_property_dialog.hpp"

DevicePropertyDialog::DevicePropertyDialog(Joystick& joystick)
  : Gtk::Dialog(joystick.get_name()),
    label("<b>" + joystick.get_name() + "</b>\nDevice: " + joystick.get_filename() , 
          Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER),
    test_tab(joystick)
{
  label.set_use_markup(true);
  set_has_separator(false);

  alignment.set_padding(8, 8, 8, 8);
  alignment.add(label);
  get_vbox()->pack_start(alignment, Gtk::PACK_SHRINK);

  get_vbox()->add(test_tab);

  add_button(Gtk::Stock::CLOSE, 0);
}

void 
DevicePropertyDialog::on_response(int response_id)
{
  hide();
}


/* EOF */
