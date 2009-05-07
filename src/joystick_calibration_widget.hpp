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

#ifndef HEADER_JOYSTICK_CALIBRATION_WIDGET_HPP
#define HEADER_JOYSTICK_CALIBRATION_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/dialog.h>

#include "joystick.hpp"

class JoystickCalibrationWidget : public Gtk::Dialog
{
private:
  Joystick& joystick;

  Gtk::Label label;
  Gtk::Frame axis_frame;
  Gtk::Table  axis_table;
  Gtk::Button calibration_button;

  struct CalibrationData {
    Gtk::CheckButton* invert;
    Gtk::Adjustment* center_min;
    Gtk::Adjustment* center_max;
    Gtk::Adjustment* range_min;
    Gtk::Adjustment* range_max;
  };

  std::vector<CalibrationData> calibration_data;
  std::vector<Joystick::CalibrationData> orig_calibration_data;

public:
  JoystickCalibrationWidget(Joystick& joystick, Gtk::Window& parent);

  void update_with(const std::vector<Joystick::CalibrationData>& data);

  void on_clear();
  void on_apply();

  void on_response(int i);
private:
  JoystickCalibrationWidget(const JoystickCalibrationWidget&);
  JoystickCalibrationWidget& operator=(const JoystickCalibrationWidget&);
};

#endif

/* EOF */
