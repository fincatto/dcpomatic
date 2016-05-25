/*
    Copyright (C) 2012-2016 Carl Hetherington <cth@carlh.net>

    This file is part of DCP-o-matic.

    DCP-o-matic is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    DCP-o-matic is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DCP-o-matic.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "gain_calculator_dialog.h"
#include "wx_util.h"
#include "lib/util.h"

GainCalculatorDialog::GainCalculatorDialog (wxWindow* parent)
	: TableDialog (parent, _("Gain Calculator"), 2, 1, true)
{
	add (_("I want to play this back at fader"), true);
	_wanted = add (new wxTextCtrl (this, wxID_ANY, wxT (""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator (wxFILTER_NUMERIC)));

	add (_("But I have to use fader"), true);
	_actual = add (new wxTextCtrl (this, wxID_ANY, wxT (""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator (wxFILTER_NUMERIC)));

	layout ();
}

float
GainCalculatorDialog::wanted_fader () const
{
	if (_wanted->GetValue().IsEmpty()) {
		return 0;
	}

	return relaxed_string_to_float (wx_to_std (_wanted->GetValue ()));
}

float
GainCalculatorDialog::actual_fader () const
{
	if (_actual->GetValue().IsEmpty()) {
		return 0;
	}

	return relaxed_string_to_float (wx_to_std (_actual->GetValue ()));
}
