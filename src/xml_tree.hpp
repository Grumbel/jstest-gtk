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

#ifndef HEADER_JSTREE_GTK_XML_TREE_HPP
#define HEADER_JSTREE_GTK_XML_TREE_HPP

#include <vector>
#include <iostream>

class XMLNode
{
public:
  XMLNode() {}
  virtual ~XMLNode() {}

  virtual void print(std::ostream& out) =0;
};

class XMLListNode : public XMLNode
{
public:
  std::string name;
  std::vector<XMLNode*> children;

  XMLListNode(const std::string& name_)
    : name(name_)
  {
  }

  ~XMLListNode()
  {
    for(std::vector<XMLNode*>::iterator i = children.begin(); i != children.end(); ++i)
      {
        delete *i;
      }
  }

  void print(std::ostream& out)
  {
    std::cout << "<" << name << ">" << std::endl;
    for(std::vector<XMLNode*>::iterator i = children.begin(); i != children.end(); ++i)
      {
        (*i)->print(out);
      }
    std::cout << "</" << name << ">" << std::endl;
  }

private:
  XMLListNode(const XMLListNode&);
  XMLListNode& operator=(const XMLListNode&);
};

class XMLDataNode : public XMLNode
{
public:
  std::string name;
  std::string data;

  XMLDataNode(const std::string& name_, const std::string& data_)
    : name(name_),
      data(data_)
  {}

  void print(std::ostream& out) {
    out << "<" << name << ">" << data << "</" << name << ">" << std::endl;
  }

private:
  XMLDataNode(const XMLDataNode&);
  XMLDataNode& operator=(const XMLDataNode&);
};

class XMLTree
{
private:
  // static C-like helper functions for expat
  static void start_element(void* userdata, const char* el, const char** attr);
  static void end_element(void* userdata, const char* el);
  static void character_data(void* userdata, const char* s, int len);

private:
  std::string filename;
  XML_Parser parser;

  XMLNode* root_node;
  std::vector<XMLListNode*> node_stack;

  XMLListNode* current_node;
  std::string node;
  std::string cdata;

public:
  XMLTree(const std::string& filename);
  ~XMLTree();

  XMLNode* get_root() const { return root_node; }
  void on_start_element(const char* el, const char** attr);
  void on_end_element(const char* el);
  void on_character_data(const char* s, int len);
  void raise_error(const std::string& str);

private:
  XMLTree(const XMLTree&);
  XMLTree& operator=(const XMLTree&);
};

#endif

/* EOF */
