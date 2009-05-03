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
#include "device_select_window.hpp"

DeviceSelectWindow::DeviceSelectWindow()
{
  set_has_separator(false);
  set_default_size(480, 640);

  get_vbox()->add(notebook);
  
  notebook.append_page(jsdev_tab, "jsdev");
  notebook.set_border_width(5);
  add_button(Gtk::Stock::CLOSE, 0);
}

DeviceSelectWindow::~DeviceSelectWindow()
{
}

void 
DeviceSelectWindow::on_response(int response_id)
{
  hide();
}

/* EOF */
