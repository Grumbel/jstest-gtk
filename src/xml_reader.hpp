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

#ifndef HEADER_JSTEST_GTK_XML_READER_HPP
#define HEADER_JSTEST_GTK_XML_READER_HPP

#include <string>

class XMLListNode;
class XMLNode;

class XMLReader
{
private:
  XMLListNode* root;
  
public:
  XMLReader(XMLListNode* node);

  operator bool() const { return root; }

  std::string get_name() const;
  XMLNode*  get_node(const std::string& name) const;
  XMLReader get_section(const std::string& name) const;

  bool read(const std::string& name, bool& value) const;
  bool read(const std::string& name, int& value) const;
  bool read(const std::string& name, std::string& value) const;
};

#endif

/* EOF */
