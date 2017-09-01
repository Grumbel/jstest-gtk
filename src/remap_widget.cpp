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

#include <iostream>
#include <gtkmm/stock.h>

#include "joystick.hpp"
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
  Gtk::TreeModelColumn<std::string> name;

private:
  RemapWidgetColumns() {
    add(id);
    add(name);
  }
};

RemapWidgetColumns* RemapWidgetColumns::instance_ = 0;

RemapWidget::RemapWidget(Joystick& joystick_, Mode mode_)
  : joystick(joystick_),
    mode(mode_)
{
  map_list = Gtk::ListStore::create(RemapWidgetColumns::instance());
  treeview.set_model(map_list);
  if (mode == REMAP_AXIS)
    treeview.append_column("Axes", RemapWidgetColumns::instance().name);
  else
    treeview.append_column("Buttons", RemapWidgetColumns::instance().name);

  set_border_width(5);
  treeview.set_border_width(5);

  scroll.add(treeview);
  scroll.set_size_request(-1, 300);
  scroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
  pack_start(scroll,  Gtk::PACK_EXPAND_WIDGET);

  treeview.set_reorderable();

  map_list->signal_row_inserted().connect(sigc::mem_fun(this, &RemapWidget::on_my_row_inserted));
  map_list->signal_row_deleted().connect(sigc::mem_fun(this, &RemapWidget::on_my_row_deleted));
  map_list->signal_rows_reordered().connect(sigc::mem_fun(this, &RemapWidget::on_my_rows_reordered));
}

void
RemapWidget::add_entry(int id, const std::string& str)
{
  Gtk::ListStore::iterator it = map_list->append();
  (*it)[RemapWidgetColumns::instance().id]   = id;
  (*it)[RemapWidgetColumns::instance().name] = str;
}

struct RemapEntry {
  int id;
  std::string name;

  bool operator<(const RemapEntry& rhs) const {
    return id < rhs.id;
  }
};

void
RemapWidget::on_clear()
{
  // We simple sort the list here by 'id'

  // Convert the ListStore into a vector
  std::vector<RemapEntry> rows;
  for(Gtk::TreeIter i = map_list->children().begin(); i != map_list->children().end(); ++i)
  {
    RemapEntry entry;
    entry.id   = (*i)[RemapWidgetColumns::instance().id];
    entry.name = (*i)[RemapWidgetColumns::instance().name];
    rows.push_back(entry);
  }

  // Sort the vector
  std::sort(rows.begin(), rows.end());

  // Renter the vector into the liststore
  map_list->clear();
  for(std::vector<RemapEntry>::iterator i = rows.begin(); i != rows.end(); ++i)
  {
    add_entry(i->id, i->name);
  }

  on_apply();
}

void
RemapWidget::on_apply()
{
  std::vector<int> mapping;
  std::vector<int> mapping_old;
  for(Gtk::TreeIter i = map_list->children().begin(); i != map_list->children().end(); ++i)
  {
    mapping.push_back((*i)[RemapWidgetColumns::instance().id]);
  }

  if (mode == REMAP_AXIS)
  {
    mapping_old = joystick.get_axis_mapping();
    joystick.set_axis_mapping(mapping);
    joystick.correct_calibration(mapping_old, mapping);
  }
  else if (mode == REMAP_BUTTON)
  {
    joystick.set_button_mapping(mapping);
  }
}

void
RemapWidget::on_my_rows_reordered(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter, int* new_order)
{
  // std::cout << "on_my_rows_reordered" << std::endl;
}

void
RemapWidget::on_my_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
{
  // std::cout << "on_my_rows_inserted" << std::endl;
}

void
RemapWidget::on_my_row_deleted(const Gtk::TreeModel::Path& path)
{
  // std::cout << "on_my_rows_deleted" << std::endl;

  if (mode == REMAP_AXIS)
  {
    if (joystick.get_axis_count() == (int)map_list->children().size())
    {
      on_apply();
    }
  }
  else if (mode == REMAP_BUTTON)
  {
    if (joystick.get_button_count() == (int)map_list->children().size())
    {
      on_apply();
    }
  }
}

/* EOF */
