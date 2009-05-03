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

#include <iostream>
#include <stdexcept>
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
    }

  Glib::signal_io().connect(sigc::mem_fun(this, &Joystick::on_in), fd, Glib::IO_IN);
}

Joystick::~Joystick()
{
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
          std::cout << "Axis: " << event.number << " -> " << event.value << std::endl;
        }
      else if (event.type & JS_EVENT_BUTTON)
        {
          std::cout << "Button: " << event.number << " -> " << event.value << std::endl;
        }
    }
  else
    {
      throw std::runtime_error("Joystick::update(): unknown read error");
    }
}

/* EOF */
