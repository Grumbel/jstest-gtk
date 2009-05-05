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
#include "remap_widget.hpp"

class RemapWidgetColumns : public Gtk::TreeModel::ColumnRecord
{
public:
  static RemapWidgetColumns* instance_;

public:
  static RemapWidgetColumns& instance() {
    if (instance_)
      return *instance_;
    else
      return *(instance_ = new RemapWidgetColumns());
  }

  Gtk::TreeModelColumn<int> id;
  Gtk::TreeModelColumn<Glib::ustring> name;

private:
  RemapWidgetColumns() {
    add(id);
    add(name);
  }
};

RemapWidgetColumns* RemapWidgetColumns::instance_ = 0;

RemapWidget::RemapWidget(const std::string& name)
  : clear_button(Gtk::Stock::CLEAR),
    apply_button(Gtk::Stock::APPLY)
{
  map_list = Gtk::ListStore::create(RemapWidgetColumns::instance());
  treeview.set_model(map_list);
  treeview.append_column(name, RemapWidgetColumns::instance().name);

  set_border_width(5);
  treeview.set_border_width(5);

  buttonbox.add(clear_button);
  buttonbox.add(apply_button);

  pack_start(treeview,  Gtk::PACK_EXPAND_WIDGET);
  pack_start(buttonbox, Gtk::PACK_SHRINK);

  treeview.set_reorderable();

  clear_button.signal_clicked().connect(sigc::mem_fun(this, &RemapWidget::on_clear));
}

void
RemapWidget::add(int id, const std::string& str)
{
  Gtk::ListStore::iterator it = map_list->append();
  (*it)[RemapWidgetColumns::instance().id] = id;
  (*it)[RemapWidgetColumns::instance().name] = str;
}

void
RemapWidget::on_clear()
{
  // resort the list
}

/* EOF */
