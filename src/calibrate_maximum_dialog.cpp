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
#include "calibrate_maximum_dialog.hpp"

CalibrateMaximumDialog::CalibrateMaximumDialog(Joystick& joystick_)
  : Gtk::Dialog("CalibrationWizard: " + joystick_.get_name()),
    joystick(joystick_),
    orig_data(joystick.get_calibration()),
    label("1) Rotate your joystick around to move all axis into their extreme positions at least once\n"
          "2) Move all axis back to the center\n"
          "3) Press ok\n")
{
  joystick.clear_calibration();

  set_border_width(5);
  label.set_line_wrap();
  get_vbox()->add(label);
  add_button(Gtk::Stock::CANCEL, 1);
  add_button(Gtk::Stock::OK, 0);

  connection = joystick.axis_move.connect(sigc::mem_fun(this, &CalibrateMaximumDialog::on_axis_move));

  is_init_axis_state.resize(joystick.get_axis_count(), false);
  min_axis_state.resize(joystick.get_axis_count());
  max_axis_state.resize(joystick.get_axis_count());

  for(int i = 0; i < joystick.get_axis_count(); ++i)
  {
    min_axis_state[i] = joystick.get_axis_state(i);
    max_axis_state[i] = joystick.get_axis_state(i);
  }
}

void
CalibrateMaximumDialog::on_response(int v)
{
  if (v == 0)
  {
    // Calculate CalibrationData
    std::vector<Joystick::CalibrationData> data;

    for(int i = 0; i < joystick.get_axis_count(); ++i)
    {
      Joystick::CalibrationData axis;
      axis.calibrate  = true;
      axis.invert     = false;
      axis.center_min = axis.center_max = joystick.get_axis_state(i);
      axis.range_min  = min_axis_state[i];
      axis.range_max  = max_axis_state[i];

      // When the center is the same as the outer edge of an axis,
      // we assume its a throttle control or analog button and
      // calculate the center on our own
      if (axis.center_min == axis.range_min ||
          axis.center_max == axis.range_max)
      {
        axis.center_min = axis.center_max = (axis.range_min + axis.range_max)/2;
      }

      data.push_back(axis);
    }

    joystick.set_calibration(data);

    hide();
  }
  else
  {
    joystick.set_calibration(orig_data);
  }
}

void
CalibrateMaximumDialog::on_axis_move(int id, int value)
{
  // std::cout << "AxisMove: " << id << " " << value << std::endl;
  if (!is_init_axis_state[id])
  {
    min_axis_state[id] = value;
    max_axis_state[id] = value;
    is_init_axis_state[id] = true;
  }
  else
  {
    min_axis_state[id] = std::min(value, min_axis_state[id]);
    max_axis_state[id] = std::max(value, max_axis_state[id]);
  }
}

/* EOF */
