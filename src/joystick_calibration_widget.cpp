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

#include <iostream>
#include <assert.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/stock.h>

#include "joystick.hpp"
#include "joystick_calibration_widget.hpp"

JoystickCalibrationWidget::JoystickCalibrationWidget(Joystick& joystick)
  : joystick(joystick),
    label("The <i>center</i> values are the minimum and the maximum values of the deadzone. "
          "The <i>min</i> and <i>max</i> values refer to the outer values."),
    axis_frame("Axis"),
    axis_table(joystick.get_axis_count() + 1, 5),
    refresh_button(Gtk::Stock::REFRESH),
    raw_button(Gtk::Stock::CLEAR),
    calibration_button(Gtk::Stock::PROPERTIES),
    apply_button(Gtk::Stock::APPLY)
{
  set_border_width(5);
  axis_frame.set_border_width(5);
  axis_table.set_border_width(5);

  label.set_use_markup(true);
  label.set_line_wrap();
  pack_start(label, Gtk::PACK_SHRINK);

  axis_table.attach(*Gtk::manage(new Gtk::Label("Axis")), 0, 1, 0, 1);

  axis_table.attach(*Gtk::manage(new Gtk::Label("Center Min")), 1, 2, 0, 1);
  axis_table.attach(*Gtk::manage(new Gtk::Label("Center Max")), 2, 3, 0, 1);
  
  axis_table.attach(*Gtk::manage(new Gtk::Label("Range Min")), 3, 4, 0, 1);
  axis_table.attach(*Gtk::manage(new Gtk::Label("Range Max")), 4, 5, 0, 1);
  
  axis_table.set_col_spacing(2, 8);
  for(int i = 0; i < joystick.get_axis_count(); ++i)
    {
      CalibrationData data;
      
      Gtk::SpinButton& center_min = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(data.center_min = new Gtk::Adjustment(0, -32768, 32767))));
      Gtk::SpinButton& center_max = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(data.center_max = new Gtk::Adjustment(0, -32768, 32767))));
      Gtk::SpinButton& outer_min  = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(data.range_min  = new Gtk::Adjustment(0, -32768, 32767))));
      Gtk::SpinButton& outer_max  = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(data.range_max  = new Gtk::Adjustment(0, -32768, 32767))));

      calibration_data.push_back(data);

      std::ostringstream str;
      str << i;
      axis_table.attach(*Gtk::manage(new Gtk::Label(str.str())), 0, 1, i+1, i+2);

      axis_table.attach(center_min, 1, 2, i+1, i+2);
      axis_table.attach(center_max, 2, 3, i+1, i+2);

      axis_table.attach(outer_min, 3, 4, i+1, i+2);
      axis_table.attach(outer_max, 4, 5, i+1, i+2);
    }

  buttonbox.add(refresh_button);
  buttonbox.add(raw_button);
  //buttonbox.add(calibration_button);
  buttonbox.add(apply_button);

  axis_frame.add(axis_table);

  pack_start(axis_frame, Gtk::PACK_EXPAND_WIDGET);
  pack_start(buttonbox, Gtk::PACK_SHRINK);

  raw_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_clear));
  refresh_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_refresh));
  apply_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_calibrate));
}

void
JoystickCalibrationWidget::on_clear()
{
  joystick.clear_calibration();  
}

void
JoystickCalibrationWidget::on_refresh()
{
  const std::vector<Joystick::CalibrationData>& data = joystick.get_calibration();
  assert(data.size() == calibration_data.size());
  
  std::cout << data.size()<< std::endl;

  for(int i = 0; i < (int)data.size(); ++i)
    {
      calibration_data[i].center_min->set_value(data[i].center_min);
      calibration_data[i].center_max->set_value(data[i].center_max);
      calibration_data[i].range_min->set_value(data[i].range_min);
      calibration_data[i].range_max->set_value(data[i].range_max);
    }
}

void
JoystickCalibrationWidget::on_calibrate()
{
  std::vector<Joystick::CalibrationData> data(calibration_data.size());
  
  for(int i = 0; i < (int)data.size(); ++i)
    {
      data[i].calibrate  = true;
      data[i].center_min = calibration_data[i].center_min->get_value();
      data[i].center_max = calibration_data[i].center_max->get_value();
      data[i].range_min  = calibration_data[i].range_min->get_value();
      data[i].range_max  = calibration_data[i].range_max->get_value();
    }

  joystick.set_calibration(data);
}

/* EOF */
