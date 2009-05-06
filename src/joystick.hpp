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

#ifndef HEADER_JSTEST_GTK_JOYSTICK_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_HPP

#include <sigc++/signal.h>
#include <sigc++/connection.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <linux/joystick.h>
#include "joystick_description.hpp"

class Joystick
{
private:
  int fd;

  std::string filename;
  Glib::ustring name;
  int axis_count;
  int button_count;

  sigc::connection connection;

public:
  struct CalibrationData {
    bool calibrate;
    int  center_min;
    int  center_max;
    int  range_min;
    int  range_max;
  };

  Joystick(const std::string& filename);
  ~Joystick();

  int get_fd() const { return fd; }

  void update();
  bool on_in(Glib::IOCondition cond);

  std::string get_filename() const { return filename; }
  Glib::ustring get_name() const { return name; }
  int get_axis_count() const   { return axis_count; }
  int get_button_count() const { return button_count; }

  sigc::signal<void, int, int>  axis_move;
  sigc::signal<void, int, bool> button_move;

  static std::vector<JoystickDescription> get_joysticks();

  std::vector<CalibrationData> get_calibration();
  void set_calibration(const std::vector<CalibrationData>& data);

  /** Clears all calibration data, note that this will mean raw USB
      input values, not values scaled to -32767/32767 */
  void clear_calibration();

  std::vector<int> get_button_mapping();
  std::vector<int> get_axis_mapping();

private:
  Joystick(const Joystick&);
  Joystick& operator=(const Joystick&);
};

#endif

/* EOF */
