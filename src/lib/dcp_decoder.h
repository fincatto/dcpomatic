/*
    Copyright (C) 2014-2016 Carl Hetherington <cth@carlh.net>

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

/** @file  src/dcp_decoder.h
 *  @brief A decoder of existing DCPs.
 */

#include "decoder.h"

namespace dcp {
	class Reel;
}

class DCPContent;
class Log;
struct dcp_subtitle_within_dcp_test;

class DCPDecoder : public Decoder
{
public:
	DCPDecoder (boost::shared_ptr<const DCPContent>, boost::shared_ptr<Log> log, bool fast);

	std::list<boost::shared_ptr<dcp::Reel> > reels () const {
		return _reels;
	}

private:
	friend struct dcp_subtitle_within_dcp_test;

	bool pass (PassReason, bool accurate);
	void seek (ContentTime t, bool accurate);

	std::list<ContentTimePeriod> image_subtitles_during (ContentTimePeriod, bool starting) const;
	std::list<ContentTimePeriod> text_subtitles_during (ContentTimePeriod, bool starting) const;

	/** Time of next thing to return from pass relative to the start of _reel */
	ContentTime _next;
	std::list<boost::shared_ptr<dcp::Reel> > _reels;
	std::list<boost::shared_ptr<dcp::Reel> >::iterator _reel;
	boost::shared_ptr<const DCPContent> _dcp_content;
};
