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
#include <stdint.h>
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

#include "evdev_helper.hpp"
#include "xml_writer.hpp"
#include "xml_reader.hpp"
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
          orig_name = name_c_str;
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
Joystick::correct_calibration(const std::vector<int>& mapping_old, const std::vector<int>& mapping_new)
{
  int axes[ABS_MAX + 1]; // axes[name] -> old_idx
  for(std::vector<int>::const_iterator i = mapping_old.begin(); i != mapping_old.end(); ++i)
    {
      axes[*i] = i - mapping_old.begin();
    }

  std::vector<CalibrationData> callib_old = get_calibration();
  std::vector<CalibrationData> callib_new;
  for(std::vector<int>::const_iterator i = mapping_new.begin(); i != mapping_new.end(); ++i)
    {
      callib_new.push_back(callib_old[axes[*i]]);
    }
  
  set_calibration(callib_new);
}

void
Joystick::write(XMLWriter& out)
{
  out.start_section("joystick");
  out.write("name",   name);
  out.write("device", filename);
  
  { // write CalibrationData
    std::vector<CalibrationData> data = get_calibration();

    out.start_section("calibration");
    for(std::vector<CalibrationData>::iterator i = data.begin(); i != data.end(); ++i)
      {
        out.start_section("axis");
        //out.write("id",         i - data.begin());
        out.write("calibrate",  i->calibrate);
        out.write("center-min", i->center_min);
        out.write("center-max", i->center_max);
        out.write("range-min",  i->range_min);
        out.write("range-max",  i->range_max);
        out.write("invert",     i->invert);
        out.end_section("axis");
      }
    out.end_section("calibration");
  }

  {
    std::vector<int> mapping = get_axis_mapping();
    out.start_section("axis-map");
    for(std::vector<int>::iterator i = mapping.begin(); i != mapping.end(); ++i)
      {
        out.write("axis", abs2str(*i));
      }
    out.end_section("axis-map");
  }

  {
    std::vector<int> mapping = get_button_mapping();
    out.start_section("button-map");
    for(std::vector<int>::iterator i = mapping.begin(); i != mapping.end(); ++i)
      {
        out.write("button", btn2str(*i));
      }   
    out.end_section("button-map");
  }

  out.end_section("joystick");
}

void
Joystick::load(const XMLReader& root_reader)
{
  std::string cfg_name;
  if (root_reader.read("name", cfg_name) && name == cfg_name)
    {
      // Read calibration data
      if (XMLReader reader = root_reader.get_section("calibration"))
        {
          std::vector<CalibrationData> calibration_data;
          const std::vector<XMLReader>& sections = reader.get_sections();
          for(std::vector<XMLReader>::const_iterator i = sections.begin(); i != sections.end(); ++i)
            {
              CalibrationData data;

              //i->read("axis", );
              //i->read("precision", );
              i->read("invert",     data.invert);
              i->read("center-min", data.center_min);
              i->read("center-max", data.center_max);
              i->read("range-min",  data.range_min);
              i->read("range-max",  data.range_max);

              calibration_data.push_back(data);
            }

          set_calibration(calibration_data);
        }

      { // Read axis mapping
        const std::vector<std::string>& cfg_axis_map = root_reader.get_string_list("axis-map");
        std::vector<int> mapping;
        
        for(std::vector<std::string>::const_iterator i = cfg_axis_map.begin(); i != cfg_axis_map.end(); ++i)
          {
            int type = 0;
            int code = 0;
            str2event(*i, type, code);
            mapping.push_back(code);
          }

        set_axis_mapping(mapping);
      }

      { // Read button mapping
        const std::vector<std::string>& cfg_button_map = root_reader.get_string_list("button-map");
        std::vector<int> mapping;
        
        for(std::vector<std::string>::const_iterator i = cfg_button_map.begin(); i != cfg_button_map.end(); ++i)
          {
            int type = 0;
            int code = 0;
            str2event(*i, type, code);
            mapping.push_back(code);
          }

        set_button_mapping(mapping);
      }
    }
}

std::string
Joystick::get_evdev() const
{
  // See /usr/share/doc/linux-doc-2.6.28/devices.txt.gz
  for(int i = 0; i < 32; ++i)
    {
      std::ostringstream out;
      out << "/dev/input/event" << i;
      
      int evdev_fd;
      if ((evdev_fd = open(out.str().c_str(), O_RDONLY)) < 0)
        {
          // ignore
        }
      else
        {
          char evdev_name[256];
          if (ioctl(evdev_fd, EVIOCGNAME(sizeof(evdev_name)), evdev_name) < 0)
            {
              std::cout << out.str() << ": " << strerror(errno) << std::endl;
            }
          else
            {
              if (orig_name == evdev_name)
                {
                  // Found a device that matches, so return it
                  close(evdev_fd);
                  return out.str();
                }
            }

          close(evdev_fd);
        }
    }

  throw std::runtime_error("couldn't find evdev for " + filename);
}

#ifdef __TEST__

// g++ -D__TEST__ joystick.cpp evdev_helper.cpp xml_writer.cpp xml_reader.cpp -o joystick-test `pkg-config --cflags --libs gtkmm-2.4 sigc++-2.0`  

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
    {
      Joystick joystick(argv[i]);

      std::cout << "Filename: '" << joystick.get_filename() << "'\n";
      std::cout << "Name:     '" << joystick.get_name() << "'\n";
      std::cout << "Axis:     " << joystick.get_axis_count() << "\n";
      std::cout << "Button:   " << joystick.get_button_count() << "\n";
      std::cout << "Evdev:    '" << joystick.get_evdev() << "'\n";
    }
  return 0;
}
#endif

/* EOF */
