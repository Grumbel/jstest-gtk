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

#ifndef HEADER_JSTREE_GTK_XML_TREE_HPP
#define HEADER_JSTREE_GTK_XML_TREE_HPP

#include <vector>
#include <memory>
#include <iostream>

#include <expat.h>

/*
 * The code below does not produce a full DOM, but a drastically
 * simplified one, namely nodes can either contain text or they can
 * contain child nodes, but not both. Nodes are also not allowed to
 * have attributes.
 */

class XMLNode
{
public:
  XMLNode() {}
  virtual ~XMLNode() {}

  virtual std::string get_name() const =0;

  virtual void print(std::ostream& out, int depth = 0) =0;
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

  std::string get_name() const { return name; }

  void print(std::ostream& out, int depth = 0)
  {
    std::cout << std::string(2*depth, ' ') << "<" << name << ">" << std::endl;
    for(std::vector<XMLNode*>::iterator i = children.begin(); i != children.end(); ++i)
    {
      (*i)->print(out, depth + 1);
    }
    std::cout << std::string(2*depth, ' ') << "</" << name << ">" << std::endl;
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

  std::string get_name() const { return name; }

  void print(std::ostream& out, int depth = 0)
  {
    out << std::string(2*depth, ' ') << "<" << name << ">" << data << "</" << name << ">" << std::endl;
  }

private:
  XMLDataNode(const XMLDataNode&);
  XMLDataNode& operator=(const XMLDataNode&);
};

class XMLParser
{
public:
  static std::auto_ptr<XMLNode> parse(const std::string& filename);

private:
  std::string filename;
  XML_Parser parser;

  std::auto_ptr<XMLNode> root_node;
  std::vector<XMLListNode*> node_stack;

  std::string node;
  std::string cdata;

  XMLParser(const std::string& filename);
  ~XMLParser();

  std::auto_ptr<XMLNode> get_root() { return root_node; }

  void on_start_element(const char* el, const char** attr);
  void on_end_element(const char* el);
  void on_character_data(const char* s, int len);
  void raise_error(const std::string& str);

private:
  static void start_element(void* userdata, const char* el, const char** attr);
  static void end_element(void* userdata, const char* el);
  static void character_data(void* userdata, const char* s, int len);

  XMLParser(const XMLParser&);
  XMLParser& operator=(const XMLParser&);
};

#endif

/* EOF */
