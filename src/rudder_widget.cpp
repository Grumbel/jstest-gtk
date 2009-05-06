/*
**  jstest-gtk - A simple joystick test and calibration utility
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

#include "rudder_widget.hpp"

RudderWidget::RudderWidget(int width, int height)
  : pos(0.0)
{
  set_size_request(width, height);
  modify_bg(Gtk::STATE_NORMAL , Gdk::Color("white"));
  modify_fg(Gtk::STATE_NORMAL , Gdk::Color("black"));
}

bool
RudderWidget::on_expose_event(GdkEventExpose* event)
{
  double p = (pos + 1.0)/2.0;
  get_window()->draw_rectangle(get_style()->get_fg_gc(get_state()),
                               true,
                               (get_allocation().get_width() * p) - 4, 0,
                               9, get_allocation().get_height()-1);
}

void
RudderWidget::set_pos(double p)
{
  pos = p;
  queue_draw();
}

/* EOF */
