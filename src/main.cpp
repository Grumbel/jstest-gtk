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

#include <iostream>
#include <gtkmm/main.h>
#include <string.h>
#include <stdlib.h>

#include "joystick_test_widget.hpp"
#include "joystick_list_widget.hpp"
#include "joystick_map_widget.hpp"
#include "joystick_calibration_widget.hpp"
#include "joystick.hpp"
#include "main.hpp"
#include "xml_writer.hpp"

Main* Main::current_ = 0;

Main::Main()
  : list_dialog(0)
{
  current_ = this;
}

Main::~Main()
{
}

void
Main::show_device_list_dialog()
{
  if (list_dialog)
    {
      list_dialog->show();
    }
  else
    {
      list_dialog = new JoystickListWidget();
      list_dialog->show_all();
    }
}

void
Main::show_device_property_dialog(const std::string& filename)
{
  Joystick* joystick = new Joystick(filename);
  JoystickTestWidget* dialog = new JoystickTestWidget(*joystick);
  dialog->signal_hide().connect(sigc::bind(sigc::mem_fun(this, &Main::on_dialog_hide), dialog));
  dialog->show_all();
  joysticks.push_back(joystick);
  dialogs.push_back(dialog);
}

void
Main::show_calibration_dialog(Joystick& joystick)
{
  JoystickCalibrationWidget* dialog = new JoystickCalibrationWidget(joystick);
  dialog->signal_hide().connect(sigc::bind(sigc::mem_fun(this, &Main::on_dialog_hide), dialog));
  dialog->show_all();
  dialogs.push_back(dialog);
}

void
Main::show_mapping_dialog(Joystick& joystick)
{
  JoystickMapWidget* dialog = new JoystickMapWidget(joystick);
  dialog->signal_hide().connect(sigc::bind(sigc::mem_fun(this, &Main::on_dialog_hide), dialog));
  dialog->show_all();
  dialogs.push_back(dialog);
}

void
Main::on_dialog_hide(Gtk::Dialog* dialog)
{
  dialogs.erase(std::remove(dialogs.begin(), dialogs.end(), dialog), dialogs.end());
  delete dialog;

  if (dialogs.empty())
    {
      Gtk::Main::quit();
    }
}

int
Main::main(int argc, char** argv)
{
  typedef std::vector<std::string> DeviceFiles;
  DeviceFiles device_files;
  std::string config_save_file;

  for(int i = 1; i < argc; ++i)
    {
      if (strcmp("--help", argv[i]) == 0 ||
          strcmp("-h", argv[i]) == 0)
        {
          std::cout << "Usage: " << argv[0] << " [OPTIONS]... [DEVICE]...\n"
                    << "A graphical joystick tester.\n"
                    << "\n"
                    << "Options:\n"
                    << "  -h, --help      Display this help and exit\n"
                    << "  -v, --version   Display version information and exit\n"
                    << "  -l, --load CFG  Load load configuration from file and apply them\n"
                    << "  -s, --save CFG  Save current device configuration to file CFG\n"
                    << "\n"
                    << "Report bugs to Ingo Ruhnke <grumbel@gmx.de>.\n";
          return 0;
        }
      else if (strcmp("--load", argv[i]) == 0 ||
               strcmp("-l", argv[i]) == 0)
        {
          ++i;
          if (i < argc)
            {
              std::cout << "Configuration file load is not yet implemented" << std::endl;
              return 0;
            }
          else
            {
              std::cout << "Error: " << argv[i-1] << " expected an argument" << std::endl;
              exit(EXIT_FAILURE);
            }         
        }
      else if (strcmp("--save", argv[i]) == 0 ||
               strcmp("-s", argv[i]) == 0)
        {
          ++i;
          if (i < argc)
            {
              config_save_file = argv[i];
            }
          else
            {
              std::cout << "Error: " << argv[i-1] << " expected an argument" << std::endl;
              exit(EXIT_FAILURE);
            }
        }
      else if (strcmp("--version", argv[i]) == 0 ||
               strcmp("-v", argv[i]) == 0)
        {
          std::cout << "jstest-gtk 0.1.0" << std::endl;
          return 0;
        }
      else if (argv[i][0] == '-')
        {
          std::cout << "Error: " << argv[0] << ": unrecognized option '" << argv[i] << "'" << std::endl;
          return EXIT_FAILURE;
        }
      else
        {
          device_files.push_back(argv[i]);
        }
    }

  if (!config_save_file.empty())
    {
      XMLWriter out(config_save_file);
      out.start_section("joysticks");
      for(DeviceFiles::iterator i = device_files.begin(); i != device_files.end(); ++i)
        {
          Joystick joystick(*i);
          joystick.write(out);
        }
      out.end_section("joysticks");
    }
  else
    {
      Glib::set_application_name("Joystick Test");
      Glib::set_prgname("jstest-gtk");
      std::cout << "'" << Glib::get_user_config_dir() + "/" + Glib::get_prgname() << "'" << std::endl;
      Gtk::Main kit(&argc, &argv);

      if (device_files.empty())
        {
          show_device_list_dialog();
        }
      else
        {
          for(DeviceFiles::iterator i = device_files.begin(); i != device_files.end(); ++i)
            {
              show_device_property_dialog(*i);
            }
        }
      Gtk::Main::run();
    }

  return 0;
}

int main(int argc, char** argv)
{
  try 
    {
      Main app;
      return app.main(argc, argv);
    } 
  catch(std::exception& err) 
    {
      std::cout << "Error: " << err.what() << std::endl;
      return EXIT_FAILURE;
    }
}

/* EOF */
