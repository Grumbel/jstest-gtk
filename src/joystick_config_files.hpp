/*
**  jstest-gtk - A graphical joystick tester
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
**  Copyright (C) 2025 Raphael Rosch <jstest-bugs@insaner.com>
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


#ifndef HEADER_JSTEST_GTK_JOYSTICK_CONFIG_FILES_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_CONFIG_FILES_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <vector>
#include <string>

#include <dirent.h>
// #include <filesystem>

// namespace fs = std::filesystem;

struct JoystickConfig {
    std::unordered_map<std::string, std::string> values;
    std::vector<std::string> usb_ids;
    std::string icon_filename;
    std::string js_type;
    bool icon_filename_is_good = false;
    std::vector<std::string> axes;
    std::vector<std::string> buttons;
    int button_maxlen = 0;
};


JoystickConfig load_config(const std::string& filename);
JoystickConfig get_config_for_usb_id(const std::string& usb_id);

std::vector<JoystickConfig> load_all_configs(const std::string& directory);

extern std::vector<JoystickConfig> joystick_configs;



#endif

/* EOF */
