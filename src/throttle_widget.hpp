// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_THROTTLE_WIDGET_HPP
#define HEADER_JSTEST_GTK_THROTTLE_WIDGET_HPP

#include <gtkmm/drawingarea.h>

class ThrottleWidget : public Gtk::DrawingArea
{
private:
  bool invert;
  double pos;

public:
  ThrottleWidget(int width, int height, bool invert = false);

  bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
  void set_pos(double p);

private:
  ThrottleWidget(const ThrottleWidget&);
  ThrottleWidget& operator=(const ThrottleWidget&);
};

#endif

/* EOF */
