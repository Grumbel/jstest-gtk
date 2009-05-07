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

#include "button_widget.hpp"

ButtonWidget::ButtonWidget(int width, int height)
{
  set_size_request(width, height);
}

bool
ButtonWidget::on_expose_event(GdkEventExpose* event)
{
  Gtk::DrawingArea::on_expose_event(event);
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
    {
      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

      int w  = get_allocation().get_width()  - 10;
      int h  = get_allocation().get_height() - 10;

      cr->set_source_rgb(0.0, 0.0, 0.0);
      cr->set_line_width(1.0);
      cr->translate(5, 5);
      cr->rectangle(0, 0, w, h);

      if (down)
        cr->fill_preserve();

      cr->stroke();
    }

  return true;
}

void
ButtonWidget::set_down(bool t)
{
  down = t;
  queue_draw();
}

/* EOF */
