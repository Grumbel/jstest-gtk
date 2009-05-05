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

#ifndef HEADER_JSTEST_GTK_REMAP_WIDGET_HPP
#define HEADER_JSTEST_GTK_REMAP_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

class RemapWidget : public Gtk::VBox
{
private:
  Gtk::HButtonBox buttonbox;
  Gtk::Button clear_button;
  Gtk::Button apply_button;
  Gtk::TreeView treeview;
  Glib::RefPtr<Gtk::ListStore> map_list;

public:
  RemapWidget(const std::string& name);

  void add(int id, const std::string& str);
  void on_clear();

private:
  RemapWidget(const RemapWidget&);
  RemapWidget& operator=(const RemapWidget&);
};

#endif

/* EOF */
