// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_RUDDER_WIDGET_HPP
#define HEADER_JSTEST_GTK_RUDDER_WIDGET_HPP

#include <gtkmm/drawingarea.h>

class RudderWidget : public Gtk::DrawingArea
{
private:
  double pos;

public:
  RudderWidget(int width, int height);

  bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
  void set_pos(double p);

private:
  RudderWidget(const RudderWidget&);
  RudderWidget& operator=(const RudderWidget&);
};

#endif

/* EOF */
