// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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
