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
#include <gtkmm/main.h>

#include "device_select_window.hpp"
#include "main.hpp"

int
Main::main(int argc, char** argv)
{
  Gtk::Main kit(&argc, &argv);
  
  DeviceSelectWindow window;
  window.show_all();
  Gtk::Main::run(window);
}

int main(int argc, char** argv)
{
  Main app;
  return app.main(argc, argv);
}

/* EOF */
