/*
    Copyright (C) 2020 Carl Hetherington <cth@carlh.net>

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

#include "copy_dcp_details_to_film.h"
#include "dcp_content.h"
#include "film.h"
#include "types.h"
#include "video_content.h"
#include "audio_content.h"
#include "ratio.h"
#include "dcp_content_type.h"
#include <boost/shared_ptr.hpp>
#include <map>

using std::map;
using std::string;
using boost::shared_ptr;

void
copy_dcp_details_to_film (shared_ptr<const DCPContent> dcp, shared_ptr<Film> film)
{
	string name = dcp->name ();
	name = name.substr (0, name.find("_"));
	film->set_name (name);
	film->set_use_isdcf_name (true);
	if (dcp->content_kind()) {
		film->set_dcp_content_type (DCPContentType::from_libdcp_kind(dcp->content_kind().get()));
	}
	film->set_encrypted (dcp->encrypted());
	film->set_reel_type (REELTYPE_BY_VIDEO_CONTENT);
	film->set_interop (dcp->standard() == dcp::INTEROP);
	film->set_three_d (dcp->three_d());

	if (dcp->video) {
		film->set_container (Ratio::nearest_from_ratio(dcp->video->size().ratio()));
		film->set_resolution (dcp->resolution());
		DCPOMATIC_ASSERT (dcp->video_frame_rate());
		film->set_video_frame_rate (*dcp->video_frame_rate());
	}

	if (dcp->audio) {
		film->set_audio_channels (dcp->audio->stream()->channels());
	}

	map<dcp::Marker, dcpomatic::ContentTime> dcp_markers;
	map<dcp::Marker, dcpomatic::DCPTime> film_markers;
	film->clear_markers ();
	for (map<dcp::Marker, dcpomatic::ContentTime>::const_iterator i = dcp_markers.begin(); i != dcp_markers.end(); ++i) {
		film->set_marker (i->first, dcpomatic::DCPTime(i->second.get()));
	}

	film->set_ratings (dcp->ratings());
	film->set_content_version (dcp->content_version());
}

