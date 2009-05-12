/*
**  jstest-gtk - A graphical joystick tester
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

#include <assert.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sstream>
#include <linux/joystick.h>
#include <glibmm/main.h>
#include <glibmm/convert.h>

#include "joystick.hpp"

Joystick::Joystick(const std::string& filename_)
  : filename(filename_)
{
  if ((fd = open(filename.c_str(), O_RDONLY)) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
  else
    {
      // ok
      uint8_t num_axis   = 0;
      uint8_t num_button = 0;
      ioctl(fd, JSIOCGAXES,    &num_axis);
      ioctl(fd, JSIOCGBUTTONS, &num_button);
      axis_count   = num_axis;
      button_count = num_button;

      // Get Name 
      char name_c_str[1024];
      if (ioctl(fd, JSIOCGNAME(sizeof(name_c_str)), name_c_str) < 0)
        {
          std::ostringstream str;
          str << filename << ": " << strerror(errno);
          throw std::runtime_error(str.str());          
        }
      else
        {
          try {
            name = Glib::convert_with_fallback(name_c_str, "UTF-8", "ISO-8859-1");
          } catch(Glib::ConvertError& err) {
            std::cout << err.what() << std::endl;
          }
        }

      axis_state.resize(axis_count);
    }

  orig_calibration_data = get_calibration();
  
  connection = Glib::signal_io().connect(sigc::mem_fun(this, &Joystick::on_in), fd, Glib::IO_IN);
}

Joystick::~Joystick()
{
  connection.disconnect();
  close(fd);
}

bool
Joystick::on_in(Glib::IOCondition cond)
{
  update();
  return true;
}

void
Joystick::update()
{
  struct js_event event;

  ssize_t len = read(fd, &event, sizeof(event));

  if (len < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
  else if (len == sizeof(event))
    { // ok
      if (event.type & JS_EVENT_AXIS)
        {
          //std::cout << "Axis: " << (int)event.number << " -> " << (int)event.value << std::endl;
          axis_state[event.number] = event.value;
          axis_move(event.number, event.value);
        }
      else if (event.type & JS_EVENT_BUTTON)
        {
          //std::cout << "Button: " << (int)event.number << " -> " << (int)event.value << std::endl;
          button_move(event.number, event.value);
        }
    }
  else
    {
      throw std::runtime_error("Joystick::update(): unknown read error");
    }
}

std::vector<JoystickDescription>
Joystick::get_joysticks()
{
  std::vector<JoystickDescription> joysticks;

  for(int i = 0; i < 32; ++i)
    {
      try 
        {
          std::ostringstream str;
          str << "/dev/input/js" << i;
          Joystick joystick(str.str());

          joysticks.push_back(JoystickDescription(joystick.get_filename(),
                                                  joystick.get_name(),                                                  
                                                  joystick.get_axis_count(),
                                                  joystick.get_button_count()));
        }
      catch(std::exception& err)
        {
          // ok
        }
    }

  return joysticks;
}

Joystick::CalibrationData corr2cal(const struct js_corr& corr_)
{
  struct js_corr corr = corr_;

  Joystick::CalibrationData data;

  if (corr.type)
    {
      data.calibrate = true;
      data.invert    = (corr.coef[2] < 0 && corr.coef[3] < 0);
      data.center_min = corr.coef[0];
      data.center_max = corr.coef[1];

      if (data.invert)
        {
          corr.coef[2] = -corr.coef[2];
          corr.coef[3] = -corr.coef[3];
        }

      // Need to use double and rint(), since calculation doesn't end
      // up on clean integer positions (i.e. 0.9999 can happen)
      data.range_min = rint(data.center_min - ((32767.0 * 16384) / corr.coef[2]));
      data.range_max = rint((32767.0 * 16384) / corr.coef[3] + data.center_max);
    }
  else
    {
      data.calibrate  = false;
      data.invert     = false;
      data.center_min = 0;
      data.center_max = 0;
      data.range_min  = 0;
      data.range_max  = 0;
    }

  return data;
}

std::vector<Joystick::CalibrationData>
Joystick::get_calibration()
{
  std::vector<struct js_corr> corr(get_axis_count());

  if (ioctl(fd, JSIOCGCORR, &*corr.begin()) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
  else
    {
      std::vector<CalibrationData> data;
      std::transform(corr.begin(), corr.end(), std::back_inserter(data), corr2cal);
      return data;
    }
}

struct js_corr cal2corr(const Joystick::CalibrationData& data)
{
  struct js_corr corr;

  if (data.calibrate &&
      (data.center_min - data.range_min)  != 0 &&
      (data.range_max  - data.center_max) != 0)
    {
      corr.type = 1;
      corr.prec = 0;
      corr.coef[0] = data.center_min;
      corr.coef[1] = data.center_max;

      corr.coef[2] = (32767 * 16384) / (data.center_min - data.range_min);
      corr.coef[3] = (32767 * 16384) / (data.range_max  - data.center_max);

      if (data.invert)
        {
          corr.coef[2] = -corr.coef[2];
          corr.coef[3] = -corr.coef[3];
        }
    }
  else
    {
      corr.type = 0;
      corr.prec = 0;
      memset(corr.coef, 0, sizeof(corr.coef));
    }

  return corr;
}

void
Joystick::set_calibration(const std::vector<CalibrationData>& data)
{
  std::vector<struct js_corr> corr;

 std::transform(data.begin(), data.end(), std::back_inserter(corr), cal2corr);

  if (ioctl(fd, JSIOCSCORR, &*corr.begin()) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
}

void
Joystick::clear_calibration()
{
  std::vector<CalibrationData> data;
  
  for(int i = 0; i < get_axis_count(); ++i)
    {
      CalibrationData cal;

      cal.calibrate  = false;
      cal.invert     = false;
      cal.center_min = 0;
      cal.center_max = 0;
      cal.range_min  = 0;
      cal.range_max  = 0;
     
      data.push_back(cal);
    }

  set_calibration(data);
}

void
Joystick::reset_calibration()
{
  set_calibration(orig_calibration_data);
}

std::vector<int>
Joystick::get_button_mapping()
{
  uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
  if (ioctl(fd, JSIOCGBTNMAP, btnmap) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
  else
    {
      std::vector<int> mapping;
      std::copy(btnmap, btnmap + button_count, std::back_inserter(mapping));
      return mapping;
    }
}

std::vector<int>
Joystick::get_axis_mapping()
{
  uint8_t axismap[ABS_MAX + 1];
  if (ioctl(fd, JSIOCGAXMAP, axismap) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
  else
    {
      std::vector<int> mapping;
      std::copy(axismap, axismap + axis_count, std::back_inserter(mapping));
      return mapping;
    }
}

void
Joystick::set_button_mapping(const std::vector<int>& mapping)
{
  assert((int)mapping.size() == button_count);

  uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
  memset(btnmap, 0, sizeof(btnmap));
  std::copy(mapping.begin(), mapping.end(), btnmap);

  if (0)
    for(int i = 0; i < button_count; ++i)
      {
        std::cout << i << " -> " << btnmap[i] << std::endl;
      }

  if (ioctl(fd, JSIOCSBTNMAP, btnmap) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
}

int
Joystick::get_axis_state(int id)
{
  if (id >= 0 && id < (int)axis_state.size())
    return axis_state[id];
  else
    return 0;
}

void
Joystick::set_axis_mapping(const std::vector<int>& mapping)
{
  assert((int)mapping.size() == axis_count);

  uint8_t axismap[ABS_MAX + 1];

  std::copy(mapping.begin(), mapping.end(), axismap);
  
  if (ioctl(fd, JSIOCSAXMAP, axismap) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
}

void
Joystick::save(std::ostream& out)
{
}

/* EOF */
