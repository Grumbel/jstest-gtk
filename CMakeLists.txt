# jstest-gtk - A graphical joystick tester
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)
project(jstest-gtk)

option(WARNINGS "Switch on extra warnings" OFF)
option(WERROR "Turn warnings into errors" OFF)

if(WARNINGS)
  set(WARNINGS_CXX_FLAGS ${WARNINGS_CXX_FLAGS}
    -Wall -Wextra
    -pedantic -Wno-c++0x-compat -Wnon-virtual-dtor
    -Wcast-qual -Winit-self -Wno-unused-parameter
    # -Wconversion -Wshadow -Weffc++
    )
endif()

if(WERROR)
  set(WARNINGS_CXX_FLAGS ${WARNINGS_CXX_FLAGS}
    -Werror)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

find_package(PkgConfig REQUIRED)
find_package(Threads)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0)
pkg_search_module(SIGCPP REQUIRED sigc++-2.0)
pkg_search_module(X11 REQUIRED x11)

# Copy data files over to build/
file(GLOB JSTEST_GTK_DATA data/*.png)
file(
  COPY ${JSTEST_GTK_DATA}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data/)

add_library(binreloc STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/binreloc-2.0/binreloc.c)
set_property(TARGET binreloc PROPERTY COMPILE_DEFINITIONS ENABLE_BINRELOC)

file(GLOB JSTEST_GTK_SOURCES src/*.cpp)

add_executable(jstest-gtk ${JSTEST_GTK_SOURCES})
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/external/binreloc-2.0/
  ${GTKMM_INCLUDE_DIRS}
  ${SIGCPP_INCLUDE_DIRS}
  ${X11_INCLUDE_DIRS})
set_property(TARGET jstest-gtk PROPERTY COMPILE_OPTIONS
  ${GTKMM_CFLAGS_OTHER}
  ${SIGCPP_CFLAGS_OTHER}
  ${X11_CFLAGS_OTHER}
  ${WARNINGS_CXX_FLAGS})
target_link_libraries(jstest-gtk
  binreloc
  ${CMAKE_THREAD_LIBS_INIT}
  ${GTKMM_LIBRARIES}
  ${SIGCPP_LIBRARIES}
  ${X11_LIBRARIES})

# EOF #
