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
#include <iostream>
#include <fstream>
#include <sstream>
#include <expat.h>

#include "xml_parser.hpp"

// static C functions that map back to the C++ member functions
void
XMLParser::start_element(void* userdata, const char* el, const char** attr)
{
  static_cast<XMLParser*>(userdata)->on_start_element(el, attr);
}

void
XMLParser::end_element(void* userdata, const char* el)
{
  static_cast<XMLParser*>(userdata)->on_end_element(el);
}

void
XMLParser::character_data(void* userdata, const char* s, int len)
{
  static_cast<XMLParser*>(userdata)->on_character_data(s, len);
}

std::auto_ptr<XMLNode>
XMLParser::parse(const std::string& filename)
{
  XMLParser tree(filename);
  return tree.get_root();
}

XMLParser::XMLParser(const std::string& filename_)
  : filename(filename_),
    parser(0)
{
  std::vector<char> data;
  { // Read the file into the vector data
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in)
    {
      throw std::runtime_error("couldn't open: " + filename);
    }
    else
    {
      in.seekg (0, std::ios::end);
      data.resize(in.tellg());
      in.seekg (0, std::ios::beg);
      in.read(&*data.begin(), data.size());
    }
  }

  parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, &start_element, &end_element);
  XML_SetCharacterDataHandler(parser, &character_data);

  if (XML_Parse(parser, &*data.begin(), data.size(), 0) == XML_STATUS_ERROR)
  {
    std::ostringstream out;
    out << filename << ":" << XML_GetCurrentLineNumber(parser)
        << ": parse error: " << XML_ErrorString(XML_GetErrorCode(parser));

    throw std::runtime_error(out.str());
  }
}

XMLParser::~XMLParser()
{
  XML_ParserFree(parser);
}

void
XMLParser::on_start_element(const char* name, const char** attr)
{
  if (*attr)
    raise_error("attribute not allowed");

  // spaces are ignored, everything else is an error
  for(int i = 0; i < (int)cdata.size(); ++i)
    if (!isspace(cdata[i]))
      raise_error("unexpected character data");
  cdata.clear();

  if (!node.empty())
  {
    XMLListNode* new_node = new XMLListNode(node);
    if (!root_node.get())
    {
      root_node.reset(new_node);
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
XMLParser::on_end_element(const char* el)
{
  if (!node.empty() && !cdata.empty())
  {
    node_stack.back()->children.push_back(new XMLDataNode(node, cdata));
    node.clear();
    cdata.clear();
  }
  else
  {
    // spaces are ignored, everything else is an error
    for(int i = 0; i < (int)cdata.size(); ++i)
      if (!isspace(cdata[i]))
        raise_error("unexpected character data");

    node_stack.pop_back();
    node.clear();
    cdata.clear();      
  }
}

void
XMLParser::on_character_data(const char* s, int len)
{
  cdata += std::string(s, len);
}

void
XMLParser::raise_error(const std::string& str)
{
  std::ostringstream out;
  out << filename << ":" << XML_GetCurrentLineNumber(parser) << ": " << str;
  throw std::runtime_error(out.str());
}

#ifdef __TEST__
int main(int argc, char** argv)
{
  try 
  {
    for(int i = 1; i < argc; ++i)
    {
      std::auto_ptr<XMLNode> root = XMLParser::parse(argv[i]);
      root->print(std::cout);
    }
  } 
  catch(std::exception& err) 
  {
    std::cout << "Error: " << err.what() << std::endl;
  }

  return 0;
}
#endif

/* EOF */
