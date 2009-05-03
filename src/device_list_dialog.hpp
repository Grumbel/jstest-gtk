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

#ifndef HEADER_JSTEST_GTK_DEVICE_LIST_DIALOG_HPP
#define HEADER_JSTEST_GTK_DEVICE_LIST_DIALOG_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>

#include "joystick_list_widget.hpp"

class DeviceListDialog : public Gtk::Dialog
{
private:
  Gtk::Notebook   notebook;
  JoystickListWidget jsdev_tab;

public:
  DeviceListDialog();
  ~DeviceListDialog();

  void on_response(int response_id);

private:
  DeviceListDialog(const DeviceListDialog&);
  DeviceListDialog& operator=(const DeviceListDialog&);
};

#endif

/* EOF */
