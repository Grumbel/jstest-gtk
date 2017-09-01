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

#ifndef HEADER_JSTEST_GTK_REMAP_WIDGET_HPP
#define HEADER_JSTEST_GTK_REMAP_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/scrolledwindow.h>

class Joystick;

class RemapWidget : public Gtk::VBox
{
public:
  enum Mode { REMAP_AXIS, REMAP_BUTTON };

private:
  Joystick& joystick;
  Mode mode;

  Gtk::TreeView treeview;
  Glib::RefPtr<Gtk::ListStore> map_list;
  Gtk::ScrolledWindow scroll;

public:
  RemapWidget(Joystick& joystick_, Mode mode);

  void add_entry(int id, const std::string& str);
  void on_clear();
  void on_apply();
  void on_my_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
  void on_my_row_deleted(const Gtk::TreeModel::Path& path);
  void on_my_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order);

private:
  RemapWidget(const RemapWidget&);
  RemapWidget& operator=(const RemapWidget&);
};

#endif

/* EOF */
