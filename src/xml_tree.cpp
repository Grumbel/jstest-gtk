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

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <expat.h>

#include "xml_tree.hpp"


XMLTree::XMLTree(const std::string& filename_)
  : filename(filename_),
    root_node(0)
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

  parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, &XMLTree::start_element, &XMLTree::end_element);
  XML_SetCharacterDataHandler(parser, &XMLTree::character_data);

  if (XML_Parse(parser, &*data.begin(), data.size(), 0) == XML_STATUS_ERROR)
    {
      std::ostringstream out;
      out << filename << ":" << XML_GetCurrentLineNumber(parser)
          << ": parse error: " << XML_ErrorString(XML_GetErrorCode(parser));

      XML_ParserFree(parser);
      throw std::runtime_error(out.str());
    }
    
  XML_ParserFree(parser);
}

XMLTree::~XMLTree()
{
  delete root_node;
}

void
XMLTree::on_start_element(const char* name, const char** attr)
{
  //std::cout << "START: " << name << std::endl;

  // space data is ignored, everything else is an error
  for(int i = 0; i < (int)cdata.size(); ++i)
    if (!isspace(cdata[i]))
      raise_error("unexpected character data");
  cdata.clear();

  if (!node.empty())
    {
      XMLListNode* new_node = new XMLListNode(node);
      if (!root_node)
        {
          root_node = new_node;
        }
      else
        {
          node_stack.back()->children.push_back(new_node);
        }
      node_stack.push_back(new_node);
    }

  node = name;
}

void
XMLTree::on_end_element(const char* el)
{
  //std::cout << "END: " << el << std::endl;

  if (!node.empty() && !cdata.empty())
    {
      node_stack.back()->children.push_back(new XMLDataNode(node, cdata));
      node.clear();
      cdata.clear();
    }
  else
    {
      for(int i = 0; i < (int)cdata.size(); ++i)
        if (!isspace(cdata[i]))
          raise_error("unexpected character data");

      node_stack.pop_back();
      node.clear();
      cdata.clear();      
    }
}

void
XMLTree::on_character_data(const char* s, int len)
{
  //std::cout << "CDATA: '";
  //std::cout.write(s, len);
  //std::cout << "'" << std::endl;

  cdata += std::string(s, len);
}

void
XMLTree::raise_error(const std::string& str)
{
  std::ostringstream out;
  out << filename << ":" << XML_GetCurrentLineNumber(parser) << ": " << str;
  throw std::runtime_error(out.str());
}

void
XMLTree::start_element(void* userdata, const char* el, const char** attr)
{
  static_cast<XMLTree*>(userdata)->on_start_element(el, attr);
}

void
XMLTree::end_element(void* userdata, const char* el)
{
  static_cast<XMLTree*>(userdata)->on_end_element(el);
}

void
XMLTree::character_data(void* userdata, const char* s, int len)
{
  static_cast<XMLTree*>(userdata)->on_character_data(s, len);
}

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
    {
      XMLTree tree(argv[i]);
      tree.get_root()->print(std::cout);
    }

  return 0;
}

/* EOF */
