/*
    Copyright (C) 2020-2021 Carl Hetherington <cth@carlh.net>

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


#include <dcp/language_tag.h>
#include <wx/wx.h>
#include <boost/optional.hpp>


class LanguageSubtagPanel;


class RegionSubtagDialog : public wxDialog
{
public:
	RegionSubtagDialog (wxWindow* parent, dcp::LanguageTag::RegionSubtag region);

	boost::optional<dcp::LanguageTag::RegionSubtag> get () const;

private:
	LanguageSubtagPanel* _panel;
};


