// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <unistd.h>
#include <linux/joystick.h>
#include <glibmm.h>

#include "evdev_helper.hpp"
#include "joystick.hpp"
#include "main.hpp"

std::string get_js_dev_id_from_filename(const std::string& filename)
{
  size_t pos = filename.find_last_of('/');
  if (pos == std::string::npos)
  {
      return filename;  // No '/' found, return whole string
  }
  return filename.substr(pos + 1);
}

Joystick::Joystick(const std::string& filename_, const std::string& js_id_)
  : filename(filename_),
    js_id(js_id_)
{
  fd = get_new_joystick_fd(); // throws on failure

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

  orig_name = name_c_str;
  try {
    name = Glib::convert_with_fallback(name_c_str, "UTF-8", "ISO-8859-1");
  } catch(Glib::ConvertError& err) {
    std::cout << err.what() << std::endl;
    name = name_c_str;
  }

  axis_state.resize(axis_count);

  auto tmp_usb_id_pair = get_usb_id_pair_from_udev();
  if (!tmp_usb_id_pair.first.empty() && !tmp_usb_id_pair.second.empty()) {
    vendor_id = tmp_usb_id_pair.first;
    product_id = tmp_usb_id_pair.second;
    usb_id = vendor_id + ":" + product_id;
    js_cfg = get_config_for_usb_id(usb_id);
    js_type = get_js_type_from_config(js_cfg);
  }
  connect_js();
  orig_calibration_data = get_calibration();
}

Joystick::~Joystick()
{
  connection.disconnect();
  if (fd >= 0)
  {
    close(fd);
    fd = -1;
  }
}

int
Joystick::get_new_joystick_fd()
{
  int tmp_fd = open(filename.c_str(), O_RDONLY);
  if (tmp_fd < 0)
  {
    std::ostringstream str;
    str << filename << ": " << strerror(errno);
    throw std::runtime_error(str.str());
  }
  return tmp_fd;
}

void
Joystick::connect_js()
{
  connection = Glib::signal_io().connect(sigc::mem_fun(this, &Joystick::on_in), fd,
                                        Glib::IO_IN | Glib::IO_HUP | Glib::IO_ERR);
}

bool
Joystick::reconnected()
{
  // Still assumes the same /dev/input/jsN path is the same device after
  // replug; different enumeration order remains a known limitation.
  if (m_verbose)
    std::cout << "attempting joystick reconnect... " << std::endl;

  try {
    int tmp_fd = get_new_joystick_fd();

    char name_c_str[1024];
    if (ioctl(tmp_fd, JSIOCGNAME(sizeof(name_c_str)), name_c_str) < 0)
    {
      if (m_verbose)
        std::cout << "could not get name from fd" << std::endl;
      close(tmp_fd);
      return false;
    }
    if (orig_name != name_c_str)
    {
      if (m_verbose)
        std::cout << "name mismatch" << std::endl;
      close(tmp_fd);
      return false;
    }
    auto tmp_usb_id_pair = get_usb_id_pair_from_udev();
    std::string tmp_usb_id = tmp_usb_id_pair.first + ":" + tmp_usb_id_pair.second;
    if (tmp_usb_id != usb_id)
    {
      if (m_verbose)
        std::cout << "usb_id mismatch" << std::endl;
      close(tmp_fd);
      return false;
    }
    std::string tmp_js_type = get_js_type_from_config(js_cfg);
    if (tmp_js_type != js_type)
    {
      if (m_verbose)
        std::cout << "js_type mismatch" << std::endl;
      close(tmp_fd);
      return false;
    }
    if (fd >= 0)
      close(fd);
    fd = tmp_fd;
    connect_js();
  } catch(std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    return false;
  }

  return true;
}


std::pair<std::string, std::string>
Joystick::get_usb_id_pair_from_udev()
{
  struct udev* udev = udev_new();
  if (!udev)
  {
    std::cout << "udev: Cannot create udev" << std::endl;
    return {};
  }

  struct udev_device* input_dev =
    udev_device_new_from_subsystem_sysname(udev, "input", js_id.c_str());
  if (!input_dev)
  {
    std::cout << filename << ": udev: Unable to find input device " << js_id << std::endl;
    udev_unref(udev);
    return {};
  }

  // Borrowed reference; do not unref
  struct udev_device* dev =
    udev_device_get_parent_with_subsystem_devtype(input_dev, "usb", "usb_device");
  if (!dev)
  {
    std::cout << filename << ": udev: Unable to find parent USB device" << std::endl;
    udev_device_unref(input_dev);
    udev_unref(udev);
    return {};
  }

  const char* vid = udev_device_get_sysattr_value(dev, "idVendor");
  const char* pid = udev_device_get_sysattr_value(dev, "idProduct");
  std::string tmp_vendor_id = vid ? vid : "";
  std::string tmp_product_id = pid ? pid : "";

  udev_device_unref(input_dev);
  udev_unref(udev);
  return {tmp_vendor_id, tmp_product_id};
}

bool
Joystick::on_in(Glib::IOCondition cond)
{
  if (cond & Glib::IO_IN)
  {
    update();
  }

  if (cond & Glib::IO_HUP)
  {
    std::cout << filename << ": joystick got disconnected" << std::endl;
    connection.disconnect();
  }

  if (cond & Glib::IO_ERR)
  {
    std::cout << filename << ": error while reading from joystick" << std::endl;
    connection.disconnect();
  }

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
      button_press(event.number, event.value);
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
      std::string js_id = std::string("js") + std::to_string(i);
      std::string str = std::string("/dev/input/") + js_id;

      if (Glib::file_test(str, Glib::FILE_TEST_EXISTS))
      {
      Joystick joystick(str, js_id);

      joysticks.push_back(JoystickDescription(joystick.get_filename(),
                                              joystick.get_name(),
                                              joystick.get_js_id(),
                                              joystick.get_vendor_id(),
                                              joystick.get_product_id(),
                                              joystick.get_usb_id(),
                                              joystick.get_axis_count(),
                                              joystick.get_button_count()));
      }
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

std::string
Joystick::get_js_type_from_config(const JoystickConfig& js_cfg)
{
  return js_cfg.js_type;
}

#ifdef __TEST__

// g++ -D__TEST__ joystick.cpp evdev_helper.cpp xml_writer.cpp xml_reader.cpp -o joystick-test `pkg-config --cflags --libs gtkmm-2.4 sigc++-2.0`

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    Joystick joystick(argv[i]);

    std::cout << "Filename:   '" << joystick.get_filename() << "'\n";
    std::cout << "Name:       '" << joystick.get_name() << "'\n";
    std::cout << "js_id:      "  << joystick.get_js_id() << "\n";
    std::cout << "Vendor_id:  "  << joystick.get_vendor_id() << "\n";
    std::cout << "Product_id: "  << joystick.get_product_id() << "\n";
    std::cout << "usb_id:     "  << joystick.get_usb_id() << "\n";
    std::cout << "Axis:       "  << joystick.get_axis_count() << "\n";
    std::cout << "Button:     "  << joystick.get_button_count() << "\n";
    std::cout << "Evdev:      '" << joystick.get_evdev() << "'\n";
  }
  return 0;
}
#endif

/* EOF */
