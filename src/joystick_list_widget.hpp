// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

#include "udev_monitor.hpp"

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

  std::unique_ptr<UdevMonitor> udev_monitor;
  std::vector<std::unique_ptr<Joystick>> m_joysticks;
  sigc::connection m_refresh_timeout;

public:
  JoystickListWidget();

  void on_refresh_button();
  void schedule_refresh();
  bool on_refresh_timeout();
  void on_properties_button();
  void on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

private:
  JoystickListWidget(const JoystickListWidget&);
  JoystickListWidget& operator=(const JoystickListWidget&);
  void js_activity_highlight(int value, Gtk::TreeModel::Path path);
  void js_activity_analog(int number, int value, Gtk::TreeModel::Path path);
  void js_activity_bool(int number, bool value, Gtk::TreeModel::Path path);

};


#endif

/* EOF */
