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

#include "axis_widget.hpp"

AxisWidget::AxisWidget(int width, int height)
  : x(0), y(0)
{
  set_size_request(width, height);
    //set colors
  modify_bg(Gtk::STATE_NORMAL , Gdk::Color("white"));
  modify_fg(Gtk::STATE_NORMAL , Gdk::Color("black"));
}

bool
AxisWidget::on_expose_event(GdkEventExpose* event)
{
  get_window()->draw_rectangle(get_style()->get_fg_gc(get_state()),
                               false,
                               0, 0,
                               get_allocation().get_width()-1, get_allocation().get_height()-1);

  int cx = get_allocation().get_width()/2;
  int cy = get_allocation().get_height()/2;

  get_window()->draw_arc(get_style()->get_fg_gc(get_state()),
                         false,
                         0, 0,
                         get_allocation().get_width()-1, get_allocation().get_height()-1,
                         0, 64 * 360);

  get_window()->draw_arc(get_style()->get_fg_gc(get_state()),
                         false,
                         cx-4, cy-4,
                         9, 9,
                         0, 64 * 360);

  int px = get_allocation().get_width()/2  + (get_allocation().get_width()/2  * x);
  int py = get_allocation().get_height()/2 + (get_allocation().get_height()/2 * y);

  get_window()->draw_arc(get_style()->get_fg_gc(get_state()),
                         true,
                         px-5, py-5,
                         11, 11,
                         0, 64 * 360);
  
  return true;
}

void
AxisWidget::set_x_axis(double x_)
{
  x = x_;
  queue_draw();
}

void
AxisWidget::set_y_axis(double y_)
{
  y = y_;
  queue_draw();
}

/* EOF */

