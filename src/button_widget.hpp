/*
**  jstest-gtk - A graphical joystick tester
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_JSTEST_GTK_BUTTON_WIDGET_HPP
#define HEADER_JSTEST_GTK_BUTTON_WIDGET_HPP

#include <gtkmm/drawingarea.h>

class ButtonWidget : public Gtk::DrawingArea
{
private:
  std::string name;
  bool down;

public:
  ButtonWidget(int width, int height, const std::string& name);

  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  void set_down(bool t);

private:
  ButtonWidget(const ButtonWidget&);
  ButtonWidget& operator=(const ButtonWidget&);
};

#endif

/* EOF */
