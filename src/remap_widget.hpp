// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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
