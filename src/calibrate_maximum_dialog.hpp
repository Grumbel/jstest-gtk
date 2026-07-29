// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_CALIBRATE_MAXIMUM_DIALOG_HPP
#define HEADER_JSTEST_GTK_CALIBRATE_MAXIMUM_DIALOG_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>

class Joystick;

class CalibrateMaximumDialog : public Gtk::Dialog
{
private:
  Joystick& joystick;
  std::vector<Joystick::CalibrationData> orig_data;
  Gtk::Label label;
  sigc::connection connection;
  std::vector<bool> is_init_axis_state;
  std::vector<int> min_axis_state;
  std::vector<int> max_axis_state;

public:
  CalibrateMaximumDialog(Joystick& joystick);

  void on_response(int v) override;
  void on_axis_move(int id, int value);

private:
  CalibrateMaximumDialog(const CalibrateMaximumDialog&);
  CalibrateMaximumDialog& operator=(const CalibrateMaximumDialog&);
};

#endif

/* EOF */
