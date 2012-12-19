/*
    Copyright (C) 2012 Carl Hetherington <cth@carlh.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/** @file  src/j2k_still_encoder.h
 *  @brief An encoder which writes JPEG2000 files for a single still source image.
 */

#include <list>
#include <vector>
#include "encoder.h"

class Image;
class Log;
class EncodeOptions;

/** @class J2KStillEncoder
 *  @brief An encoder which writes repeated JPEG2000 files from a single decoded input.
 */
class J2KStillEncoder : public Encoder
{
public:
	J2KStillEncoder (boost::shared_ptr<const Film>, boost::shared_ptr<const EncodeOptions>);

private:
	void do_process_video (boost::shared_ptr<Image>, boost::shared_ptr<Subtitle>);

	void link (std::string, std::string) const;
};
