// SPDX-FileCopyrightText: 2009 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2025 Raphael Rosch <jstest-bugs@insaner.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_JSTEST_GTK_BUTTON_WIDGET_HPP
#define HEADER_JSTEST_GTK_BUTTON_WIDGET_HPP



class ButtonWidget : public Gtk::ToggleButton {
public:
  ButtonWidget()
  {
    add_events(Gdk::BUTTON_PRESS_MASK);
  }

protected:
  bool on_button_press_event(GdkEventButton* event) override
  {
    return true; // Prevent toggle
  }
};

#endif

/* EOF */
