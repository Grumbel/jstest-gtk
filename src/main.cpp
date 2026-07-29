// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <cstring>
#include <cstdlib>

#include <gtkmm.h>

#include "joystick_config_files.hpp"
#include "joystick_test_widget.hpp"
#include "main.hpp"

Main* Main::current_ = 0;
bool m_verbose = false;

#ifndef JSTEST_GTK_DATADIR
#  define JSTEST_GTK_DATADIR "data/"
#endif

#ifndef JSTEST_GTK_VERSION
#  define JSTEST_GTK_VERSION "unknown"
#endif

Main::Main() :
  Gtk::Application("com.gmail.grumbel.jstest-gtk", Gio::APPLICATION_HANDLES_OPEN),
  datadir(JSTEST_GTK_DATADIR),
  m_simple_ui(false)
{
  current_ = this;
}

Main::~Main()
{
}

int
Main::run(int argc, char** argv)
{
  std::string device_file;

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp("--help", argv[i]) == 0 || strcmp("-h", argv[i]) == 0)
    {
      std::cout << "Usage: " << argv[0] << " [OPTIONS]... [DEVICE]\n"
                << "A graphical joystick tester.\n"
                << "\n"
                << "Options:\n"
                << "  -h, --help      Display this help and exit\n"
                << "  -v, --version   Display version information and exit\n"
                << "  --simple        Hide graphical representation of axis\n"
                << "  --verbose       Print useful extra information\n"
                << "  --datadir DIR   Load application data from DIR\n"
                << "\n"
                << "Report bugs to Ingo Ruhnke <grumbel@gmail.com>.\n";
      return 0;
    }
    else if (strcmp("--version", argv[i]) == 0 || strcmp("-v", argv[i]) == 0)
    {
      std::cout << "jstest-gtk " << JSTEST_GTK_VERSION << std::endl;
      return 0;
    }
    else if (strcmp("--simple", argv[i]) == 0)
    {
      m_simple_ui = true;
    }
    else if (strcmp("--verbose", argv[i]) == 0)
    {
      m_verbose = true;
    }
    else if (strcmp("--datadir", argv[i]) == 0)
    {
      i += 1;
      if (i >= argc)
      {
        std::cout << "Error: " << argv[0] << ": argument to --datadir is missing" << std::endl;
        return EXIT_FAILURE;
      }
      datadir = argv[i];
      if (!datadir.empty() && datadir.back() != '/')
        datadir += '/';
    }
    else if (argv[i][0] == '-')
    {
      std::cout << "Error: " << argv[0] << ": unrecognized option '" << argv[i] << "'" << std::endl;
      return EXIT_FAILURE;
    }
    else
    {
      if (!device_file.empty())
      {
        std::cout << "Error: " << argv[0] << ": multiple device files given, only one allowed: " << argv[i] << std::endl;
        return EXIT_FAILURE;
      }
      device_file = argv[i];
    }
  }

  joystick_configs = load_all_configs(datadir + "mappings");

  try
  {
    JoystickTestWidget window(m_simple_ui, device_file);
    window.show_all();
    return Gtk::Application::run(window);
  }
  catch (std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (Glib::Exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}

int main(int argc, char** argv)
{
  try
  {
    Glib::RefPtr<Main> app = Main::create();
    return app->run(argc, argv);
  }
  catch (std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}

Glib::RefPtr<Main> Main::create()
{
  return Glib::RefPtr<Main>(new Main);
}

/* EOF */
