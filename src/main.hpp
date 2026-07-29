// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_MAIN_HPP
#define HEADER_JSTEST_GTK_MAIN_HPP

#include <gtkmm.h>

extern bool m_verbose;

class Main : public Gtk::Application
{
private:
  static Main* current_;
public:
  static Main* current() { return current_; }
  static Glib::RefPtr<Main> create();

private:
  std::string datadir;
  bool m_simple_ui;

public:
  Main();
  ~Main();

  int run(int argc, char** argv);

  std::string get_data_directory() const { return datadir; }
};

#endif

/* EOF */
