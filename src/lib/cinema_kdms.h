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

class Cinema;
class Job;
class Log;

class CinemaKDMs
{
public:
	void make_zip_file (std::string film_name, boost::filesystem::path zip_file) const;

	static std::list<CinemaKDMs> collect (std::list<ScreenKDM> kdms);
	static void write_zip_files (std::string film_name, std::list<CinemaKDMs> cinema_kdms, boost::filesystem::path directory);
	static void email (
		std::string film_name,
		std::string cpl_name,
		std::list<CinemaKDMs> cinema_kdms,
		dcp::LocalTime from,
		dcp::LocalTime to,
		boost::shared_ptr<Log> log
		);

	boost::shared_ptr<Cinema> cinema;
	std::list<ScreenKDM> screen_kdms;
};
