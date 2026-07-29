// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JOYSTICK_CALIBRATION_WIDGET_HPP
#define HEADER_JOYSTICK_CALIBRATION_WIDGET_HPP

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/dialog.h>
#include <gtkmm/scrolledwindow.h>

#include "joystick.hpp"

class JoystickCalibrationWidget : public Gtk::Dialog
{
private:
  Joystick& joystick;

  Gtk::Label label;
  Gtk::Frame axis_frame;
  Gtk::Table  axis_table;
  Gtk::HButtonBox buttonbox;
  Gtk::Button calibration_button;
  Gtk::ScrolledWindow scroll;

  struct CalibrationData {
    Gtk::CheckButton* invert;
    Glib::RefPtr<Gtk::Adjustment> center_min;
    Glib::RefPtr<Gtk::Adjustment> center_max;
    Glib::RefPtr<Gtk::Adjustment> range_min;
    Glib::RefPtr<Gtk::Adjustment> range_max;
  };

  std::vector<CalibrationData> calibration_data;

public:
  JoystickCalibrationWidget(Joystick& joystick);

  void update_with(const std::vector<Joystick::CalibrationData>& data);

  void on_clear();
  void on_apply();
  void on_response(int i) override;
  void on_calibrate();

private:
  JoystickCalibrationWidget(const JoystickCalibrationWidget&);
  JoystickCalibrationWidget& operator=(const JoystickCalibrationWidget&);
};

#endif

/* EOF */
