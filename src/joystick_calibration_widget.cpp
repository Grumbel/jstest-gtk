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

#include <gtkmm/spinbutton.h>
#include <gtkmm/stock.h>

#include "joystick.hpp"
#include "joystick_calibration_widget.hpp"

JoystickCalibrationWidget::JoystickCalibrationWidget(Joystick& joystick)
  : joystick(joystick),
    label("The <i>center</i> values are the minimum and the maximum values of the deadzone. "
          "The <i>min</i> and <i>max</i> values refer to the outer values."),
    axis_frame("Axis"),
    refresh_button(Gtk::Stock::REFRESH),
    raw_button(Gtk::Stock::CLEAR),
    calibration_button(Gtk::Stock::PROPERTIES),
    apply_button(Gtk::Stock::APPLY)
{
  set_border_width(5);
  axis_frame.set_border_width(5);
  label.set_use_markup(true);
  label.set_line_wrap();
  pack_start(label, Gtk::PACK_SHRINK);

  for(int i = 0; i < joystick.get_axis_count(); ++i)
    {
      Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox());
      hbox->set_border_width(5);
 
      Gtk::SpinButton& center_min = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(new Gtk::Adjustment(0, -32768, 32767))));
      Gtk::SpinButton& center_max = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(new Gtk::Adjustment(0, -32768, 32767))));
      Gtk::SpinButton& outer_min  = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(new Gtk::Adjustment(0, -32768, 32767))));
      Gtk::SpinButton& outer_max  = *Gtk::manage(new Gtk::SpinButton(*Gtk::manage(new Gtk::Adjustment(0, -32768, 32767))));

      std::ostringstream str;
      str << i << ": ";
      hbox->add(*Gtk::manage(new Gtk::Label(str.str())));

      hbox->add(*Gtk::manage(new Gtk::Label(" C: ")));
      hbox->add(center_min);

      //hbox->add(*Gtk::manage(new Gtk::Label(" cmax: ")));
      hbox->add(center_max);

      hbox->add(*Gtk::manage(new Gtk::Label(" R: ")));
      hbox->add(outer_min);

      //hbox->add(*Gtk::manage(new Gtk::Label(" max: ")));
      hbox->add(outer_max);
      
      axis_vbox.add(*hbox);
    }

  buttonbox.add(refresh_button);
  buttonbox.add(raw_button);
  //buttonbox.add(calibration_button);
  buttonbox.add(apply_button);

  axis_frame.add(axis_vbox);

  pack_start(axis_frame, Gtk::PACK_EXPAND_WIDGET);
  pack_start(buttonbox, Gtk::PACK_SHRINK);

  raw_button.signal_clicked().connect(sigc::mem_fun(this, &JoystickCalibrationWidget::on_clear));
}

void
JoystickCalibrationWidget::on_clear()
{
  joystick.clear_calibration();  
}

void
JoystickCalibrationWidget::on_refresh()
{

}

void
JoystickCalibrationWidget::on_calibrate()
{
  
}

/* EOF */
