// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "joystick_config_files.hpp"
#include "main.hpp"

std::vector<JoystickConfig> joystick_configs;

bool file_is_good(const std::string& filename) {
  std::ifstream file(filename);
  return file.good();
}

JoystickConfig get_config_for_usb_id(const std::string& usb_id) {
  JoystickConfig ret;
  for (const auto& config : joystick_configs) {
    for (const auto& cur_usb_id : config.usb_ids) {
      //std::cout << "  USB ID: " << cur_usb_id << "\n";
      if (cur_usb_id == usb_id) {
          m_verbose and std::cout << " FOUND usb_id: " << usb_id << "\n";
          return config;
      }
    }
  }
  return ret;
}

JoystickConfig load_config(const std::string& filename) {
  JoystickConfig config;
  std::ifstream file(filename);
  std::string line;

  while (std::getline(file, line)) {
    // Remove comments
    size_t comment_pos = line.find('#');
    if (comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }

    if (line.empty()) continue;

    // Parse name=value
    size_t equals_pos = line.find('=');
    if (equals_pos == std::string::npos) continue;

    std::string name = line.substr(0, equals_pos);
    std::string value = line.substr(equals_pos + 1);

    if (name == "icon_filename")
    {
      config.icon_filename = value;
      config.icon_filename_is_good = file_is_good(
        Main::current()->get_data_directory() + "icons/" + value);
    }

    if (name == "js_type")
    {
      config.js_type = value;
    }

    if (name == "usb_id")
    {
      m_verbose and std::cout << "   registering usb_id: " << value << std::endl;
      config.usb_ids.push_back(value);
    }
    else
    {
      config.values[name] = value;

      size_t underscore_pos = name.find('_');
      if (name.rfind("axis_", 0) == 0)
      {
        config.axes.insert(config.axes.begin() + std::stoi(name.substr(underscore_pos + 1)), value);
      }
      else if (name.rfind("button_", 0) == 0)
      {
        int new_pos = std::stoi(name.substr(underscore_pos + 1));
        if (config.buttons.size() < new_pos) {
            config.buttons.resize(new_pos);
        }
        config.buttons.insert(config.buttons.begin() + new_pos, value);
        if (value.size() > config.button_maxlen) config.button_maxlen = value.size();
      }
    }
  }

  return config;
}

std::vector<JoystickConfig> load_all_configs(const std::string& directory) {
  m_verbose and std::cout << "loading config files in: " << directory  << std::endl;

  std::vector<JoystickConfig> conf_files;
  DIR* dir = opendir(directory.c_str());
  if (!dir) return conf_files;

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    std::string name = entry->d_name;
    if (name.size() >= 7 && name.substr(name.size() - 7) == ".config") {
      m_verbose and std::cout << "loading config: " << directory + "/" + name << std::endl;
      conf_files.push_back(load_config(directory + "/" + name));
    }
  }

  closedir(dir);
  return conf_files;
}
