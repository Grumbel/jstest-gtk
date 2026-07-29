// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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
