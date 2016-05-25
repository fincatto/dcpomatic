/*
    Copyright (C) 2014 Carl Hetherington <cth@carlh.net>

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

/** @file  test/player_test.cc
 *  @brief Various tests of Player.
 */

#include <iostream>
#include <boost/test/unit_test.hpp>
#include "lib/film.h"
#include "lib/ffmpeg_content.h"
#include "lib/dcp_content_type.h"
#include "lib/ratio.h"
#include "lib/audio_buffers.h"
#include "lib/player.h"
#include "test.h"

using std::cout;
using std::list;
using boost::shared_ptr;

static bool
valid (Content const *)
{
	return true;
}

/** Player::overlaps */
BOOST_AUTO_TEST_CASE (player_overlaps_test)
{
	shared_ptr<Film> film = new_test_film ("player_overlaps_test");
	film->set_container (Ratio::from_id ("185"));

	/* This content is 3s long */
	shared_ptr<FFmpegContent> A (new FFmpegContent (film, "test/data/test.mp4"));
	shared_ptr<FFmpegContent> B (new FFmpegContent (film, "test/data/test.mp4"));
	shared_ptr<FFmpegContent> C (new FFmpegContent (film, "test/data/test.mp4"));

	film->examine_and_add_content (A);
	film->examine_and_add_content (B);
	film->examine_and_add_content (C);
	wait_for_jobs ();

	BOOST_CHECK_EQUAL (A->full_length(), DCPTime (288000));

	A->set_position (DCPTime::from_seconds (0));
	B->set_position (DCPTime::from_seconds (10));
	C->set_position (DCPTime::from_seconds (20));

	shared_ptr<Player> player (new Player (film, film->playlist ()));

	list<shared_ptr<Piece> > o = player->overlaps (DCPTime::from_seconds (0), DCPTime::from_seconds (5), &valid);
	BOOST_CHECK_EQUAL (o.size(), 1U);
	BOOST_CHECK_EQUAL (o.front()->content, A);

	o = player->overlaps (DCPTime::from_seconds (5), DCPTime::from_seconds (8), &valid);
	BOOST_CHECK_EQUAL (o.size(), 0U);

	o = player->overlaps (DCPTime::from_seconds (8), DCPTime::from_seconds (12), &valid);
	BOOST_CHECK_EQUAL (o.size(), 1U);
	BOOST_CHECK_EQUAL (o.front()->content, B);

	o = player->overlaps (DCPTime::from_seconds (2), DCPTime::from_seconds (12), &valid);
	BOOST_CHECK_EQUAL (o.size(), 2U);
	BOOST_CHECK_EQUAL (o.front()->content, A);
	BOOST_CHECK_EQUAL (o.back()->content, B);

	o = player->overlaps (DCPTime::from_seconds (8), DCPTime::from_seconds (11), &valid);
	BOOST_CHECK_EQUAL (o.size(), 1U);
	BOOST_CHECK_EQUAL (o.front()->content, B);
}

/** Check that the Player correctly generates silence when used with a silent FFmpegContent */
BOOST_AUTO_TEST_CASE (player_silence_padding_test)
{
	shared_ptr<Film> film = new_test_film ("player_silence_padding_test");
	film->set_name ("player_silence_padding_test");
	shared_ptr<FFmpegContent> c (new FFmpegContent (film, "test/data/test.mp4"));
	film->set_container (Ratio::from_id ("185"));
	film->set_audio_channels (6);

	film->examine_and_add_content (c);
	wait_for_jobs ();

	shared_ptr<Player> player (new Player (film, film->playlist ()));
	shared_ptr<AudioBuffers> test = player->get_audio (DCPTime (0), DCPTime::from_seconds (1), true);
	BOOST_CHECK_EQUAL (test->frames(), 48000);
	BOOST_CHECK_EQUAL (test->channels(), film->audio_channels ());

	for (int i = 0; i < test->frames(); ++i) {
		for (int c = 0; c < test->channels(); ++c) {
			BOOST_CHECK_EQUAL (test->data()[c][i], 0);
		}
	}
}
