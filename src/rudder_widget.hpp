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

#ifndef HEADER_JSTEST_GTK_RUDDER_WIDGET_HPP
#define HEADER_JSTEST_GTK_RUDDER_WIDGET_HPP

#include <gtkmm/drawingarea.h>

class RudderWidget : public Gtk::DrawingArea
{
private:
public:
  RudderWidget(int width, int height);

  bool on_expose_event(GdkEventExpose* event);

private:
  RudderWidget(const RudderWidget&);
  RudderWidget& operator=(const RudderWidget&);
};

#endif

/* EOF */
