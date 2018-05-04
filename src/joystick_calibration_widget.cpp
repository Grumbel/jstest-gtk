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
#include <assert.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/stock.h>

#include "joystick.hpp"
#include "calibrate_maximum_dialog.hpp"
#include "joystick_calibration_widget.hpp"

JoystickCalibrationWidget::JoystickCalibrationWidget(Joystick& joystick)
  : Gtk::Dialog("Calibration: " + joystick.get_name()),
    joystick(joystick),
    label("The <i>center</i> values are the minimum and the maximum values of the deadzone.\n"
          "The <i>min</i> and <i>max</i> values refer to the outer values. You have to unplug\n"
          "your joystick or reboot to reset the values to their original default.\n"
          "\n"
          "To run the calibration wizard, press the <i>Calibrate</i> button."),
    axis_frame("Axes"),
    axis_table(joystick.get_axis_count() + 1, 5),
    buttonbox(Gtk::BUTTONBOX_SPREAD),
    calibration_button("Start Calibration")
{
  set_border_width(5);
  axis_frame.set_border_width(5);
  axis_table.set_border_width(5);

  label.set_use_markup(true);
  label.set_line_wrap();
  get_vbox()->pack_start(label, Gtk::PACK_SHRINK);

  calibration_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_calibrate));

  buttonbox.set_border_width(5);
  buttonbox.add(calibration_button);
  get_vbox()->pack_start(buttonbox, Gtk::PACK_SHRINK);

  axis_table.attach(*Gtk::manage(new Gtk::Label("Axes")), 0, 1, 0, 1);

  axis_table.attach(*Gtk::manage(new Gtk::Label("CenterMin")), 1, 2, 0, 1);
  axis_table.attach(*Gtk::manage(new Gtk::Label("CenterMax")), 2, 3, 0, 1);

  axis_table.attach(*Gtk::manage(new Gtk::Label("RangeMin")), 3, 4, 0, 1);
  axis_table.attach(*Gtk::manage(new Gtk::Label("RangeMax")), 4, 5, 0, 1);

  axis_table.attach(*Gtk::manage(new Gtk::Label("Invert")), 5, 6, 0, 1);

  axis_table.set_col_spacing(2, 8);
  for(int i = 0; i < joystick.get_axis_count(); ++i)
  {
    CalibrationData data;

    Gtk::SpinButton&  center_min = *Gtk::manage(new Gtk::SpinButton((data.center_min = Gtk::Adjustment::create(0, -32768, 32767))));
    Gtk::SpinButton&  center_max = *Gtk::manage(new Gtk::SpinButton((data.center_max = Gtk::Adjustment::create(0, -32768, 32767))));
    Gtk::SpinButton&  range_min  = *Gtk::manage(new Gtk::SpinButton((data.range_min  = Gtk::Adjustment::create(0, -32768, 32767))));
    Gtk::SpinButton&  range_max  = *Gtk::manage(new Gtk::SpinButton((data.range_max  = Gtk::Adjustment::create(0, -32768, 32767))));
    Gtk::CheckButton& invert     = *(data.invert = Gtk::manage(new Gtk::CheckButton()));

    center_min.signal_value_changed().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_apply));
    center_max.signal_value_changed().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_apply));
    range_min.signal_value_changed().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_apply));
    range_max.signal_value_changed().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_apply));
    invert.signal_clicked().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_apply));

    center_min.set_tooltip_text("The minimal value of the dead zone");
    center_max.set_tooltip_text("The maximum value of the dead zone");
    range_min.set_tooltip_text("The minimal position reachable");
    range_max.set_tooltip_text("The maximum position reachable");

    calibration_data.push_back(data);

    std::ostringstream str;
    str << i;
    axis_table.attach(*Gtk::manage(new Gtk::Label(str.str())), 0, 1, i+1, i+2);

    axis_table.attach(center_min, 1, 2, i+1, i+2);
    axis_table.attach(center_max, 2, 3, i+1, i+2);

    axis_table.attach(range_min, 3, 4, i+1, i+2);
    axis_table.attach(range_max, 4, 5, i+1, i+2);

    axis_table.attach(invert, 5, 6, i+1, i+2, Gtk::SHRINK, Gtk::SHRINK);
  }

  add_button(Gtk::Stock::REVERT_TO_SAVED,  2);
  add_button("Raw Events", 1);
  Gtk::Widget* close_button = add_button(Gtk::Stock::CLOSE, 0);

  scroll.add(axis_table);
  scroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
  scroll.set_size_request(-1, 300);
  axis_frame.add(scroll);

  get_vbox()->pack_start(axis_frame, Gtk::PACK_EXPAND_WIDGET);

  signal_response().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_response));

  close_button->grab_focus();

  update_with(joystick.get_calibration());
}

void
JoystickCalibrationWidget::on_clear()
{
  joystick.clear_calibration();
  update_with(joystick.get_calibration());
}

void
JoystickCalibrationWidget::update_with(const std::vector<Joystick::CalibrationData>& data)
{
  assert(data.size() == calibration_data.size());

  for(int i = 0; i < (int)data.size(); ++i)
  {
    calibration_data[i].invert->set_active(data[i].invert);
    calibration_data[i].center_min->set_value(data[i].center_min);
    calibration_data[i].center_max->set_value(data[i].center_max);
    calibration_data[i].range_min->set_value(data[i].range_min);
    calibration_data[i].range_max->set_value(data[i].range_max);
  }
}

void
JoystickCalibrationWidget::on_apply()
{
  std::vector<Joystick::CalibrationData> data(calibration_data.size());

  for(int i = 0; i < (int)data.size(); ++i)
  {
    data[i].calibrate  = true;
    data[i].invert     = calibration_data[i].invert->get_active();
    data[i].center_min = calibration_data[i].center_min->get_value();
    data[i].center_max = calibration_data[i].center_max->get_value();
    data[i].range_min  = calibration_data[i].range_min->get_value();
    data[i].range_max  = calibration_data[i].range_max->get_value();
  }

  joystick.set_calibration(data);
}

void
JoystickCalibrationWidget::on_calibrate()
{
  CalibrateMaximumDialog dialog(joystick);
  dialog.show_all();
  dialog.run();
  update_with(joystick.get_calibration());
}

void
JoystickCalibrationWidget::on_response(int i)
{
  if (i == 0)
  {
    hide();
  }
  else if (i == 1)
  {
    on_clear();
  }
  else if (i == 2)
  {
    joystick.reset_calibration();
    update_with(joystick.get_calibration());
  }
}

/* EOF */
