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

#include "xml_parser.hpp"
#include "xml_reader.hpp"

XMLReader::XMLReader(XMLListNode* root_)
  : root(root_)
{
}

XMLNode*
XMLReader::get_node(const std::string& name)
{
  if (root)
    {
      for(std::vector<XMLNode*>::iterator i = root->children.begin(); i != root->children.end(); ++i)
        {
          if ((*i)->get_name() == name)
            return *i;
        }
    }
  
  return 0;
}

XMLReader
XMLReader::get_section(const std::string& name)
{
  return XMLReader(dynamic_cast<XMLListNode*>(get_node(name)));
}

void
XMLReader::read(const std::string& name, bool& value)
{
  XMLDataNode* node = dynamic_cast<XMLDataNode*>(get_node(name));
  if (node)
    {
      value = (node->data != "0");
    }
}

void
XMLReader::read(const std::string& name, int& value)
{
  XMLDataNode* node = dynamic_cast<XMLDataNode*>(get_node(name));
  if (node)
    {
      value = atoi(node->data.c_str());
    }
}

void
XMLReader::read(const std::string& name, std::string& value)
{
  XMLDataNode* node = dynamic_cast<XMLDataNode*>(get_node(name));
  if (node)
    {
      value = node->data;
    }
}

/* EOF */
