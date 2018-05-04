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

#ifndef HEADER_JSTEST_GTK_JOYSTICK_LIST_WIDGET_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_LIST_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/liststore.h>
#include <gtkmm/window.h>

class JoystickListWidget : public Gtk::Window
{
private:
  Gtk::Frame frame;
  Gtk::ScrolledWindow scrolled;
  Gtk::TreeView treeview;

  Gtk::VBox m_vbox;
  Gtk::HButtonBox m_buttonbox;
  Gtk::Button m_refresh_button;
  Gtk::Button m_properties_button;
  Gtk::Button m_close_button;

  Glib::RefPtr<Gtk::ListStore> device_list;

public:
  JoystickListWidget();

  void on_refresh_button();
  void on_properties_button();
  void on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

private:
  JoystickListWidget(const JoystickListWidget&);
  JoystickListWidget& operator=(const JoystickListWidget&);
};

#endif

/* EOF */
