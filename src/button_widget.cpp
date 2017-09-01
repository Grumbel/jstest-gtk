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

#include "button_widget.hpp"

ButtonWidget::ButtonWidget(int width, int height, const std::string& name_)
  : name(name_)
{
  set_size_request(width, height);
}

bool
ButtonWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Gtk::DrawingArea::on_draw(cr);

    int w  = get_allocation().get_width()  - 10;
    int h  = get_allocation().get_height() - 10;

    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->set_line_width(1.0);
    cr->translate(5, 5);
    cr->rectangle(0, 0, w, h);

    if (down)
      cr->fill_preserve();

    cr->stroke();

    if (down)
      cr->set_source_rgb(1.0, 1.0, 1.0);

    // FIXME: There are better ways to center text
    if (name.size() == 2)
      cr->move_to(w/2-6, h/2+3);
    else
      cr->move_to(w/2-4, h/2+3);
    cr->show_text(name);

  return true;
}

void
ButtonWidget::set_down(bool t)
{
  down = t;
  queue_draw();
}

/* EOF */
