// SPDX-FileCopyrightText: 2008 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVDEV_HELPER_HPP
#define HEADER_EVDEV_HELPER_HPP

#include <string>

bool str2event(const std::string& name, int& type, int& code);
std::string btn2str(int i);
std::string abs2str(int i);
std::string rel2str(int i);

#endif

/* EOF */
