##  -*- python -*-
##  Windstille - A Sci-Fi Action-Adventure Game
##  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##  
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##  
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.

env = Environment(CXXFLAGS=["-g", "-Wall", "-Werror"],
                  LIBS=["expat"])
env.ParseConfig('pkg-config --cflags --libs gtkmm-2.4 gtkglextmm-1.2 sigc++-2.0')
env.Program('jscfg-test', ['src/joystick_configuration.cpp'])
env.Program('xml-test', ['src/xml_tree.cpp'])
env.Program('jstest-gtk', [
    'src/axis_widget.cpp',
    'src/button_widget.cpp',
    'src/throttle_widget.cpp',
    'src/rudder_widget.cpp',
    'src/joystick_list_widget.cpp',
    'src/joystick_test_widget.cpp',
    'src/joystick_calibration_widget.cpp',
    'src/joystick_map_widget.cpp',
    'src/joystick.cpp',
    'src/calibrate_maximum_dialog.cpp',
    'src/remap_widget.cpp',
    'src/evdev_helper.cpp',
    'src/main.cpp'
    ])
    
# EOF #
