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

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iterator>

#include "joystick_configuration.hpp"

JoystickConfiguration::JoystickConfiguration(const std::string& filename)
{
  std::cout << "Trying to read: " << filename << std::endl;

  std::vector<char> data;
  { // Read the file into the vector
    std::ifstream in(filename.c_str(), std::ios::binary);
    in.seekg (0, std::ios::end);
    data.resize(in.tellg());
    in.seekg (0, std::ios::beg);
    in.read(&*data.begin(), data.size());
    in.close();  
  }

  XML_Parser parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, &JoystickConfiguration::start_element, &JoystickConfiguration::end_element);
  XML_SetCharacterDataHandler(parser, &JoystickConfiguration::character_data);

  if (XML_Parse(parser, &*data.begin(), data.size(), 0) == XML_STATUS_ERROR)
    {
      std::cout << filename << ":" << XML_GetCurrentLineNumber(parser)
                << ": parse error: " << XML_ErrorString(XML_GetErrorCode(parser))
                << std::endl;
    }
    
  XML_ParserFree(parser);
}

JoystickConfiguration::~JoystickConfiguration()
{
}

void
JoystickConfiguration::start_element(void* userdata, const char* el, const char** attr)
{
  std::cout << "start_element: " << el << std::endl;
  for(const char** p = attr; *p; ++p)
    {
      std::cout << "  attr: " << *p << std::endl;
    }
}

void
JoystickConfiguration::end_element(void* userdata, const char* el)
{
  std::cout << "end_element: " << el << std::endl;
}

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
    {
      JoystickConfiguration cfg(argv[i]);
      
    }

  return 0;
}

void
JoystickConfiguration::character_data(void* userdata,
                                      const XML_Char* s,
                                      int len)
{
  std::cout << "Data: '";
  std::cout.write(s, len);
  std::cout << "'" << std::endl;
}

/* EOF */
