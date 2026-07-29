# SPDX-FileCopyrightText: 2015 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.5)

# Source of truth: top-level VERSION file (e.g. "0.2.0-dev"), or
# -DPROJECT_VERSION_FULL=... from the packaging (Nix flake appends +g<rev>).
if(NOT DEFINED PROJECT_VERSION_FULL)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL LIMIT_COUNT 1)
endif()

# CMake's project(VERSION) needs a numeric major.minor.patch
string(REGEX REPLACE "^([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" PROJECT_VERSION_CMAKE "${PROJECT_VERSION_FULL}")

project(jstest-gtk VERSION ${PROJECT_VERSION_CMAKE})

include(GNUInstallDirs)

option(WARNINGS "Switch on extra warnings" OFF)
option(WERROR "Turn warnings into errors" OFF)
option(BUILD_TESTS "Build test cases" OFF)

if (BUILD_TESTS)
  enable_testing()
endif(BUILD_TESTS)

find_package(Git)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always HEAD
    OUTPUT_VARIABLE GIT_REPO_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=%cd --date=format:%Y-%m-%d ${GIT_REPO_VERSION}
    OUTPUT_VARIABLE GIT_REPO_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
  set(GIT_REPO_VERSION "${PROJECT_VERSION_FULL}")
endif()

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

# Mirror data/ into the build tree (icons/, mappings/, …)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
  PATTERN "*~" EXCLUDE)

file(GLOB JSTEST_GTK_SOURCES src/*.cpp)

add_executable(jstest-gtk ${JSTEST_GTK_SOURCES})
include_directories(SYSTEM
  ${GTKMM_INCLUDE_DIRS}
  ${SIGCPP_INCLUDE_DIRS}
  ${X11_INCLUDE_DIRS})
set_property(TARGET jstest-gtk PROPERTY COMPILE_OPTIONS
  ${GTKMM_CFLAGS_OTHER}
  ${SIGCPP_CFLAGS_OTHER}
  ${X11_CFLAGS_OTHER}
  ${WARNINGS_CXX_FLAGS})
target_compile_definitions(jstest-gtk PRIVATE
  JSTEST_GTK_DATADIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/"
  JSTEST_GTK_VERSION="${PROJECT_VERSION_FULL}")
target_link_libraries(jstest-gtk
  udev
  ${CMAKE_THREAD_LIBS_INIT}
  ${GTKMM_LIBRARIES}
  ${SIGCPP_LIBRARIES}
  ${X11_LIBRARIES})

install(TARGETS jstest-gtk
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Runtime assets: icons/, mappings/ (and applications/ is not required at runtime)
install(DIRECTORY data/icons data/mappings
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  PATTERN "*~" EXCLUDE)

install(FILES
  data/applications/jstest-gtk.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

install(FILES
  data/icons/jstest-gtk.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

install(FILES
  doc/jstest-gtk.1
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

# EOF #
