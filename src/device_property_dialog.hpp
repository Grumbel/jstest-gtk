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

#ifndef HEADER_JSTEST_GTK_DEVICE_PROPERTY_DIALOG_HPP
#define HEADER_JSTEST_GTK_DEVICE_PROPERTY_DIALOG_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <gtkmm/notebook.h>
#include <gtkmm/frame.h>

#include "joystick.hpp"
#include "joystick_test_widget.hpp"
#include "joystick_map_widget.hpp"

class DevicePropertyDialog : public Gtk::Dialog
{
private:
  Gtk::Alignment alignment;
  Gtk::Label label;

  Gtk::Notebook notebook;
  JoystickTestWidget test_tab;
  JoystickMapWidget  btnmap_tab;
  JoystickMapWidget  axismap_tab;
  
public:
  DevicePropertyDialog(Joystick& joystick);

  void on_response(int response_id);

private:
  DevicePropertyDialog(const DevicePropertyDialog&);
  DevicePropertyDialog& operator=(const DevicePropertyDialog&);
};

#endif

/* EOF */
