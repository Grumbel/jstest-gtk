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

#include "joystick_device_properties.hpp"

JoystickDeviceProperties::JoystickDeviceProperties(const std::string& name)
  : label(name),
    test_tab(6, 8),
    buttonbox(Gtk::BUTTONBOX_END),
    close_button(Gtk::Stock::CLOSE)
{
  set_default_size(480, 640);

  vbox.pack_start(label, Gtk::PACK_SHRINK);
  
  notebook.append_page(test_tab, "Joystick Test");
  vbox.pack_start(notebook, Gtk::PACK_EXPAND_WIDGET);

  buttonbox.add(close_button);
  vbox.pack_start(buttonbox, Gtk::PACK_SHRINK);

  add(vbox);

  // Signals
  close_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickDeviceProperties::on_close));
}

void
JoystickDeviceProperties::on_close()
{
  hide();
}

/* EOF */
