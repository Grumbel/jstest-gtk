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

#ifndef HEADER_JSTEST_GTK_JOYSTICK_CONFIGURATION_HPP
#define HEADER_JSTEST_GTK_JOYSTICK_CONFIGURATION_HPP

#include <string>

class JoystickConfiguration
{
private:

public:
  JoystickConfiguration(const std::string& filename);
  ~JoystickConfiguration();

  void on_start_element(const char* el, const char** attr);
  void on_end_element(const char* el);
  void on_character_data(const char* s, int len);
  
private:
  static void start_element(void* userdata, const char* el, const char** attr);
  static void end_element(void* userdata, const char* el);
  static void character_data(void* userdata, const char* s, int len);

private:
  JoystickConfiguration(const JoystickConfiguration&);
  JoystickConfiguration& operator=(const JoystickConfiguration&);
};

#endif

/* EOF */
