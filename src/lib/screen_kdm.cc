/*
    Copyright (C) 2013-2015 Carl Hetherington <cth@carlh.net>

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

#include "screen_kdm.h"
#include "cinema.h"
#include "screen.h"
#include "util.h"
#include <boost/foreach.hpp>

using std::string;
using std::list;
using boost::shared_ptr;

bool
operator== (ScreenKDM const & a, ScreenKDM const & b)
{
	return a.screen == b.screen && a.kdm == b.kdm;
}

string
ScreenKDM::filename (string film_name) const
{
	return tidy_for_filename (film_name) + "_" + tidy_for_filename (screen->cinema->name) + "_" + tidy_for_filename (screen->name) + ".kdm.xml";
}

void
ScreenKDM::write_files (string film_name, list<ScreenKDM> screen_kdms, boost::filesystem::path directory)
{
	/* Write KDMs to the specified directory */
	BOOST_FOREACH (ScreenKDM const & i, screen_kdms) {
		boost::filesystem::path out = directory / i.filename(film_name);
		i.kdm.as_xml (out);
	}
}
