/*
**  jstest-gtk - A graphical joystick tester
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_JOYSTICK_MAP_WIDGET_HPP
#define HEADER_JOYSTICK_MAP_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>

#include "remap_widget.hpp"

class Joystick;

class JoystickMapWidget : public Gtk::Dialog
{
private:
  Gtk::Label label;
  Gtk::HBox  hbox;
  RemapWidget axis_map;
  RemapWidget button_map;

public:
  JoystickMapWidget(Joystick& joystick);

  void on_response(int v) override;

private:
  JoystickMapWidget(const JoystickMapWidget&);
  JoystickMapWidget& operator=(const JoystickMapWidget&);
};

#endif

/* EOF */
