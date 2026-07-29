// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "throttle_widget.hpp"

ThrottleWidget::ThrottleWidget(int width, int height, bool invert_)
  : invert(invert_),
    pos(0.0)
{
  set_size_request(width, height);
  //modify_bg(Gtk::STATE_NORMAL , Gdk::Color("white"));
  //modify_fg(Gtk::STATE_NORMAL , Gdk::Color("black"));
}

bool
ThrottleWidget::on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr)
{
  double p = 1.0 - (pos + 1.0) / 2.0;

    int w  = get_allocation().get_width()-10;
    int h  = get_allocation().get_height()-10;

    cr->translate(5, 5);

    // Outer Rectangle
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->set_line_width(1.0);
    cr->rectangle(0, 0, w, h);
    cr->stroke();

    int dh = h*p;
    cr->rectangle(0, h - dh, w, dh);
    cr->fill();

  return true;
}

void
ThrottleWidget::set_pos(double p)
{
  if (invert)
    pos = -p;
  else
    pos = p;
  queue_draw();
}

/* EOF */
