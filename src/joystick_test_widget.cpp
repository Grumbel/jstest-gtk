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

#include <sstream>
#include <iostream>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include <gtkmm/dialog.h>
#include <gtkmm/image.h>

#include "main.hpp"
#include "joystick.hpp"
#include "button_widget.hpp"
#include "joystick_map_widget.hpp"
#include "joystick_calibration_widget.hpp"
#include "joystick_test_widget.hpp"

JoystickTestWidget::JoystickTestWidget(Joystick& joystick_)
  : Gtk::Dialog(joystick_.get_name()),
    joystick(joystick_),
    label("<b>" + joystick.get_name() + "</b>\nDevice: " + joystick.get_filename() , 
          Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER),
    axis_frame("Axes"),
    button_frame("Buttons"),
    axis_table(joystick.get_axis_count(), 2),
    button_table((joystick.get_button_count()-1) / 8 + 1, 8),
    mapping_button("Mapping"),
    calibration_button("Calibration"),
    buttonbox(Gtk::BUTTONBOX_SPREAD),
    stick1_widget(96, 96),
    stick2_widget(96, 96),
    stick3_widget(96, 96),
    rudder_widget(96, 16),
    throttle_widget(16, 96)
{
  label.set_use_markup(true);

  axis_frame.set_border_width(5);
  axis_table.set_border_width(5);
  axis_table.set_spacings(5);
  button_frame.set_border_width(5);
  button_table.set_border_width(5);
  button_table.set_spacings(8);
  buttonbox.set_border_width(5);

  for(int i = 0; i < joystick.get_axis_count(); ++i)
    {
      std::ostringstream str;
      str << "Axis " << i << ": ";
      Gtk::Label& label = *Gtk::manage(new Gtk::Label(str.str()));
      axis_table.attach(label, 0, 1, i, i+1, Gtk::SHRINK, Gtk::FILL);

      Gtk::ProgressBar& progressbar = *Gtk::manage(new Gtk::ProgressBar());
      progressbar.set_fraction(0.5);
      axis_table.attach(progressbar, 1, 2, i, i+1, Gtk::FILL|Gtk::EXPAND, Gtk::EXPAND);

      axes.push_back(&progressbar);
    }

  for(int i = 0; i < joystick.get_button_count(); ++i)
    {
      int x = i % 8;
      int y = i / 8;

      ButtonWidget& button = *Gtk::manage(new ButtonWidget(24, 24));
      button_table.attach(button, x, x+1, y, y+1, Gtk::EXPAND, Gtk::EXPAND);
      buttons.push_back(&button);
    }
  
  alignment.set_padding(8, 8, 8, 8);
  alignment.add(label);
  get_vbox()->pack_start(alignment, Gtk::PACK_SHRINK);

  buttonbox.add(mapping_button);
  buttonbox.add(calibration_button);

  get_vbox()->pack_start(axis_frame,   Gtk::PACK_EXPAND_WIDGET);
  get_vbox()->pack_start(button_frame, Gtk::PACK_EXPAND_WIDGET);
  get_vbox()->pack_start(buttonbox, Gtk::PACK_SHRINK);

  stick_hbox.set_border_width(5);
  if (0)
    {
      Gtk::Table& table = *Gtk::manage(new Gtk::Table(2, 2));
      
      table.attach(stick1_widget, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
      table.attach(rudder_widget,   0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
      table.attach(throttle_widget, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

      stick_hbox.pack_start(table);
      stick_hbox.pack_start(stick2_widget, Gtk::PACK_EXPAND_PADDING);
    }
  else
    {
      stick_hbox.pack_start(stick1_widget, Gtk::PACK_EXPAND_PADDING);
      stick_hbox.pack_start(stick2_widget, Gtk::PACK_EXPAND_PADDING);
      stick_hbox.pack_start(stick3_widget, Gtk::PACK_EXPAND_PADDING);
    }

  axis_vbox.pack_start(stick_hbox, Gtk::PACK_SHRINK);
  axis_vbox.add(axis_table);
  axis_frame.add(axis_vbox);

  button_frame.add(button_table);

  add_button(Gtk::Stock::CLOSE, 0);

  signal_response().connect(sigc::mem_fun(this, &JoystickTestWidget::on_response));

  joystick.axis_move.connect(sigc::mem_fun(this, &JoystickTestWidget::axis_move));
  joystick.button_move.connect(sigc::mem_fun(this, &JoystickTestWidget::button_move));

  calibration_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickTestWidget::on_calibrate));
  mapping_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickTestWidget::on_mapping));
}

void
JoystickTestWidget::axis_move(int number, int value)
{
  axes.at(number)->set_fraction((value + 32767) / (double)(2*32767));

  std::ostringstream str;
  str << value;
  axes.at(number)->set_text(str.str());

  if (number == 0)
    {
      stick1_widget.set_x_axis(value / 32767.0);
    }
  else if (number == 1)
    {
      stick1_widget.set_y_axis(value / 32767.0);
    }
  else if (number == 2)
    {
      stick2_widget.set_x_axis(value / 32767.0);
      rudder_widget.set_pos(value / 32767.0);
    }
  else if (number == 3)
    {
      stick2_widget.set_y_axis(value / 32767.0);
      throttle_widget.set_pos(value / 32767.0);
    }
  else if (number == 4)
    {
      stick3_widget.set_x_axis(value / 32767.0);
    }
  else if (number == 5)
    {
      stick3_widget.set_y_axis(value / 32767.0);
    }
}

void
JoystickTestWidget::button_move(int number, bool value)
{
  if (value)
    buttons.at(number)->set_down(true);
  else
    buttons.at(number)->set_down(false);
}

void
JoystickTestWidget::on_calibrate()
{
  Main::current()->show_calibration_dialog(joystick);
}

void
JoystickTestWidget::on_mapping()
{
  Main::current()->show_mapping_dialog(joystick);
}

void
JoystickTestWidget::on_response(int v)
{
  hide();
}

/* EOF */
