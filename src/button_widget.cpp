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
  modify_bg(Gtk::STATE_NORMAL , Gdk::Color("white"));
  modify_fg(Gtk::STATE_NORMAL , Gdk::Color("black"));
}

bool
ButtonWidget::on_expose_event(GdkEventExpose* event)
{
  if (down)
    get_window()->draw_rectangle(get_style()->get_fg_gc(get_state()),
                                 true,
                                 0, 0,
                                 get_allocation().get_width(), get_allocation().get_height());
  else
    get_window()->draw_rectangle(get_style()->get_fg_gc(get_state()),
                                 false,
                                 0, 0,
                                 get_allocation().get_width()-1, get_allocation().get_height()-1);

  //get_window()->draw_glyphs(get_style()->get_fg_gc(get_state()), get_style()->get_font(), 0, 0, "1");
}

void
ButtonWidget::set_down(bool t)
{
  down = t;
  queue_draw();
}

/* EOF */
