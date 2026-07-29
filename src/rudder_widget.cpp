// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "rudder_widget.hpp"

RudderWidget::RudderWidget(int width, int height)
  : pos(0.0)
{
  set_size_request(width, height);
}

bool
RudderWidget::on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr)
{
  double p = (pos + 1.0)/2.0;

    int w  = get_allocation().get_width()-10;
    int h  = get_allocation().get_height()-10;

    cr->translate(5, 5);

    // Outer Rectangle
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->set_line_width(1.0);
    cr->rectangle(0, 0, w, h);
    cr->stroke();

    cr->set_source_rgba(0.0, 0.0, 0.0, 0.5);
    cr->move_to(w/2, 0);
    cr->line_to(w/2, h);
    cr->stroke();

    cr->set_line_width(2.0);
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->move_to(w * p, 0);
    cr->line_to(w * p, h);
    cr->stroke();

  return true;
}

void
RudderWidget::set_pos(double p)
{
  pos = p;
  queue_draw();
}

/* EOF */
