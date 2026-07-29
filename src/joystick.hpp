// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_JOYSTICK_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_HPP

#include <sigc++/signal.h>
#include <sigc++/connection.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <linux/joystick.h>
#include <libudev.h>
#include "joystick_description.hpp"
#include "joystick_config_files.hpp"

std::string get_js_dev_id_from_filename(const std::string& filename_);

class Joystick
{
public:
  struct CalibrationData {
    bool calibrate;
    bool invert;
    int  center_min;
    int  center_max;
    int  range_min;
    int  range_max;
  };

private:
  int fd = -1;

  std::string filename;
  std::string orig_name;
  Glib::ustring name;
  std::string js_id;
  std::string js_type;
  std::string vendor_id;
  std::string product_id;
  std::string usb_id;
  int axis_count = 0;
  int button_count = 0;

  void connect_js();
  int get_new_joystick_fd();
  bool try_adopt_device(const std::string& path, const std::string& new_js_id);
  std::pair<std::string, std::string> get_usb_id_pair_from_udev(const std::string& sysname);

  std::vector<int> axis_state;
  std::vector<CalibrationData> orig_calibration_data;

  sigc::connection connection;

public:
  Joystick(const std::string& filename, const std::string& js_id);
  ~Joystick();

  JoystickConfig js_cfg;
  int get_fd() const { return fd; }

  void update();
  bool on_in(Glib::IOCondition cond);
  bool reconnected();

  std::string get_filename() const    { return filename; }
  Glib::ustring get_name() const      { return name; }
  std::string get_js_id() const       { return js_id; }
  std::string get_js_type() const     { return js_type; }
  std::string get_vendor_id() const   { return vendor_id; }
  std::string get_product_id() const  { return product_id; }
  std::string get_usb_id() const      { return usb_id; }
  int get_axis_count() const          { return axis_count; }
  int get_button_count() const        { return button_count; }

  std::string get_js_type_from_config(const JoystickConfig& js_cfg);

  sigc::signal<void, int, int>  axis_move;
  sigc::signal<void, int, bool> button_press;

  int get_axis_state(int id);

  static std::vector<JoystickDescription> get_joysticks();

  std::vector<CalibrationData> get_calibration();
  void set_calibration(const std::vector<CalibrationData>& data);
  void reset_calibration();

  /** Clears all calibration data, note that this will mean raw USB
      input values, not values scaled to -32767/32767 */
  void clear_calibration();

  std::vector<int> get_button_mapping();
  std::vector<int> get_axis_mapping();

  void set_button_mapping(const std::vector<int>& mapping);
  void set_axis_mapping(const std::vector<int>& mapping);

  /** Corrects calibration data after remaping axes */
  void correct_calibration(const std::vector<int>& mapping_old, const std::vector<int>& mapping_new);

  /** Get the evdev that this joystick device is based on. This call
      is just a guess, not guranteed to be the exact same device, but
      for our uses that should be enough. */
  std::string get_evdev() const;

private:
  Joystick(const Joystick&);
  Joystick& operator=(const Joystick&);
};

#endif

/* EOF */
