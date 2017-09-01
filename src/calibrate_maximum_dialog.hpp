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
