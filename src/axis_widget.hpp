// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_AXIS_WIDGET_HPP
#define HEADER_JSTEST_GTK_AXIS_WIDGET_HPP

#include <gtkmm/drawingarea.h>
#include <gtkmm/alignment.h>

class AxisWidget : public Gtk::Alignment
{
private:
  Gtk::DrawingArea drawingarea;
  double x;
  double y;

public:
  AxisWidget(int width, int height);

  bool on_draw(const Cairo::RefPtr<Cairo::Context>& context) override;

  void set_x_axis(double x);
  void set_y_axis(double x);

private:
  AxisWidget(const AxisWidget&);
  AxisWidget& operator=(const AxisWidget&);
};

#endif

/* EOF */
