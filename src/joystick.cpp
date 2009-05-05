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
          name = name_c_str;
        }

      // Axis Mapping
      uint8_t  axismap[ABS_MAX + 1];
      if (ioctl(fd, JSIOCGAXMAP, axismap) < 0)
        {
          std::ostringstream str;
          str << filename << ": " << strerror(errno);
          throw std::runtime_error(str.str());
        }
      else
        {
          //for(int i = 0; i < num_axis; ++i)
          // std::cout << "Axis: " << i << " -> " << (int)axismap[i] << std::endl;
        }

      // Button Mapping
      uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
      if (ioctl(fd, JSIOCGBTNMAP, btnmap) < 0)
        {
          std::ostringstream str;
          str << filename << ": " << strerror(errno);
          throw std::runtime_error(str.str());
        }
      else
        {
          //for(int i = 0; i < num_button; ++i)
          //std::cout << "Button: " << i << " -> " << (int)btnmap[i] << std::endl;
        }

      int axis_count;
      int button_count;

      if (0)
        { // reverse button mapping
          uint16_t new_btnmap[KEY_MAX - BTN_MISC + 1];
          for(int i = 0; i < num_button; ++i)
            new_btnmap[i] = 0; //btnmap[num_button - i - 1];
          ioctl(fd, JSIOCSBTNMAP, new_btnmap);
        }
    }
  
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

std::vector<Joystick::CalibrationData>
Joystick::get_calibration()
{
  std::vector<CalibrationData> data(get_axis_count());
  if (ioctl(fd, JSIOCGCORR, &*data.begin()) < 0)
    {
      std::ostringstream str;
      str << filename << ": " << strerror(errno);
      throw std::runtime_error(str.str());
    }
  else
    {
      return data;
    }
}

void
Joystick::set_calibration(const std::vector<CalibrationData>& data)
{
  if (ioctl(fd, JSIOCSCORR, &*data.begin()) < 0)
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

      cal.type = 0;
      cal.prec = 0;
      memset(cal.coef, 0, sizeof(cal.coef));
     
      data.push_back(cal);
    }

  set_calibration(data);
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
      for(int i = 0; i < button_count; ++i)
        {
          std::cout << "MAP: " << i << " -> " << (int)btnmap[i] << std::endl;
        }

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

/* EOF */
