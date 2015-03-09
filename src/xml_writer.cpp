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

#include <stdexcept>

#include "xml_writer.hpp"

XMLWriter::XMLWriter(const std::string& filename)
  : out(new std::ofstream(filename.c_str())),
    depth(0)
{
  if (!*out)
  {
    throw std::runtime_error("couldn't open " + filename);
  }
  else
  {
    (*out) << "<?xml version=\"1.0\"?>" << std::endl;
  }
}

void
XMLWriter::indent()
{
  for(int i = 0; i < depth; ++i)
    *out << "  ";
}

void
XMLWriter::start_section(const std::string& name)
{
  indent();
  (*out.get()) << "<" << name << ">" << std::endl;
  depth += 1;
}

void
XMLWriter::end_section(const std::string& name)
{
  depth -= 1;
  indent();
  *out << "</" << name << ">" << std::endl;
}

void
XMLWriter::write(const std::string& name, const std::string& value)
{
  indent();
  // FIXME: Implement magic to convert things to UTF-8
  *out << "<" << name << ">";
  *out << value;
  *out << "</" << name << ">" << std::endl;
}

void
XMLWriter::write(const std::string& name, int value)
{
  indent();
  *out << "<" << name << ">";
  *out << value;
  *out << "</" << name << ">" << std::endl;
}

void
XMLWriter::write(const std::string& name, bool value)
{
  indent();
  *out << "<" << name << ">";
  *out << value;
  *out << "</" << name << ">" << std::endl;
}

/* EOF */
