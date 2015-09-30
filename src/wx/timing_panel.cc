/*
    Copyright (C) 2012-2015 Carl Hetherington <cth@carlh.net>

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

#include "timing_panel.h"
#include "wx_util.h"
#include "film_viewer.h"
#include "timecode.h"
#include "content_panel.h"
#include "lib/content.h"
#include "lib/image_content.h"
#include "lib/raw_convert.h"
#include "lib/subtitle_content.h"
#include "lib/dcp_subtitle_content.h"
#include "lib/subrip_content.h"
#include <boost/foreach.hpp>
#include <set>
#include <iostream>

using std::cout;
using std::string;
using std::set;
using boost::shared_ptr;
using boost::dynamic_pointer_cast;

TimingPanel::TimingPanel (ContentPanel* p, FilmViewer* viewer)
	/* horrid hack for apparent lack of context support with wxWidgets i18n code */
	: ContentSubPanel (p, S_("Timing|Timing"))
	, _viewer (viewer)
{
	wxFlexGridSizer* grid = new wxFlexGridSizer (2, 4, 4);
	_sizer->Add (grid, 0, wxALL, 8);

	wxSize size = TimecodeBase::size (this);

	wxSizer* labels = new wxBoxSizer (wxHORIZONTAL);
	//// TRANSLATORS: this is an abbreviation for "hours"
	wxStaticText* t = new wxStaticText (this, wxID_ANY, _("h"), wxDefaultPosition, size, wxALIGN_CENTRE_HORIZONTAL);
#ifdef DCPOMATIC_LINUX
	/* Hack to work around failure to centre text on GTK */
	gtk_label_set_line_wrap (GTK_LABEL (t->GetHandle()), FALSE);
#endif
	labels->Add (t, 1, wxEXPAND);
	add_label_to_sizer (labels, this, wxT (":"), false);
	//// TRANSLATORS: this is an abbreviation for "minutes"
	t = new wxStaticText (this, wxID_ANY, _("m"), wxDefaultPosition, size, wxALIGN_CENTRE_HORIZONTAL);
#ifdef DCPOMATIC_LINUX
	gtk_label_set_line_wrap (GTK_LABEL (t->GetHandle()), FALSE);
#endif
	labels->Add (t, 1, wxEXPAND);
	add_label_to_sizer (labels, this, wxT (":"), false);
	//// TRANSLATORS: this is an abbreviation for "seconds"
	t = new wxStaticText (this, wxID_ANY, _("s"), wxDefaultPosition, size, wxALIGN_CENTRE_HORIZONTAL);
#ifdef DCPOMATIC_LINUX
	gtk_label_set_line_wrap (GTK_LABEL (t->GetHandle()), FALSE);
#endif
	labels->Add (t, 1, wxEXPAND);
	add_label_to_sizer (labels, this, wxT (":"), false);
	//// TRANSLATORS: this is an abbreviation for "frames"
	t = new wxStaticText (this, wxID_ANY, _("f"), wxDefaultPosition, size, wxALIGN_CENTRE_HORIZONTAL);
#ifdef DCPOMATIC_LINUX
	gtk_label_set_line_wrap (GTK_LABEL (t->GetHandle()), FALSE);
#endif
	labels->Add (t, 1, wxEXPAND);
	grid->Add (new wxStaticText (this, wxID_ANY, wxT ("")));
	grid->Add (labels);

	add_label_to_sizer (grid, this, _("Position"), true);
	_position = new Timecode<DCPTime> (this);
	grid->Add (_position);
	add_label_to_sizer (grid, this, _("Full length"), true);
	_full_length = new Timecode<DCPTime> (this);
	grid->Add (_full_length);
	add_label_to_sizer (grid, this, _("Trim from start"), true);
	_trim_start = new Timecode<ContentTime> (this);
	grid->Add (_trim_start);
	_trim_start_to_playhead = new wxButton (this, wxID_ANY, _("Trim up to current position"));
	grid->AddSpacer (0);
	grid->Add (_trim_start_to_playhead);
	add_label_to_sizer (grid, this, _("Trim from end"), true);
	_trim_end = new Timecode<ContentTime> (this);
	grid->Add (_trim_end);
	_trim_end_to_playhead = new wxButton (this, wxID_ANY, _("Trim after current position"));
	grid->AddSpacer (0);
	grid->Add (_trim_end_to_playhead);
	add_label_to_sizer (grid, this, _("Play length"), true);
	_play_length = new Timecode<DCPTime> (this);
	grid->Add (_play_length);

	{
		add_label_to_sizer (grid, this, _("Video frame rate"), true);
		wxBoxSizer* s = new wxBoxSizer (wxHORIZONTAL);
		_video_frame_rate = new wxTextCtrl (this, wxID_ANY);
		s->Add (_video_frame_rate, 1, wxEXPAND);
		_set_video_frame_rate = new wxButton (this, wxID_ANY, _("Set"));
		_set_video_frame_rate->Enable (false);
		s->Add (_set_video_frame_rate, 0, wxLEFT | wxRIGHT, 8);
		grid->Add (s, 1, wxEXPAND);
	}

	grid->AddSpacer (0);

	/* We can't use Wrap() here as it doesn't work with markup:
	 * http://trac.wxwidgets.org/ticket/13389
	 */

	wxString in = _("<i>Only change this if it the content's frame rate has been read incorrectly.</i>");
	wxString out;
	int const width = 20;
	int current = 0;
	for (size_t i = 0; i < in.Length(); ++i) {
		if (in[i] == ' ' && current >= width) {
			out += '\n';
			current = 0;
		} else {
			out += in[i];
			++current;
		}
	}

	t = new wxStaticText (this, wxID_ANY, wxT (""));
	t->SetLabelMarkup (out);
	grid->Add (t, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 6);

	_position->Changed.connect    (boost::bind (&TimingPanel::position_changed, this));
	_full_length->Changed.connect (boost::bind (&TimingPanel::full_length_changed, this));
	_trim_start->Changed.connect  (boost::bind (&TimingPanel::trim_start_changed, this));
	_trim_start_to_playhead->Bind (wxEVT_COMMAND_BUTTON_CLICKED, boost::bind (&TimingPanel::trim_start_to_playhead_clicked, this));
	_trim_end->Changed.connect    (boost::bind (&TimingPanel::trim_end_changed, this));
	_trim_end_to_playhead->Bind   (wxEVT_COMMAND_BUTTON_CLICKED, boost::bind (&TimingPanel::trim_end_to_playhead_clicked, this));
	_play_length->Changed.connect (boost::bind (&TimingPanel::play_length_changed, this));
	_video_frame_rate->Bind       (wxEVT_COMMAND_TEXT_UPDATED, boost::bind (&TimingPanel::video_frame_rate_changed, this));
	_set_video_frame_rate->Bind   (wxEVT_COMMAND_BUTTON_CLICKED, boost::bind (&TimingPanel::set_video_frame_rate, this));
}

void
TimingPanel::update_full_length ()
{
	set<DCPTime> check;
	BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
		check.insert (i->full_length ());
	}

	if (check.size() == 1) {
		_full_length->set (_parent->selected().front()->full_length (), _parent->film()->video_frame_rate ());
	} else {
		_full_length->clear ();
	}
}

void
TimingPanel::update_play_length ()
{
	set<DCPTime> check;
	BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
		check.insert (i->length_after_trim ());
	}

	if (check.size() == 1) {
		_play_length->set (_parent->selected().front()->length_after_trim (), _parent->film()->video_frame_rate ());
	} else {
		_play_length->clear ();
	}
}

void
TimingPanel::film_content_changed (int property)
{
	int const film_video_frame_rate = _parent->film()->video_frame_rate ();

	/* Here we check to see if we have exactly one different value of various
	   properties, and fill the controls with that value if so.
	*/

	if (property == ContentProperty::POSITION) {

		set<DCPTime> check;
		BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
			check.insert (i->position ());
		}

		if (check.size() == 1) {
			_position->set (_parent->selected().front()->position(), film_video_frame_rate);
		} else {
			_position->clear ();
		}

	} else if (
		property == ContentProperty::LENGTH ||
		property == VideoContentProperty::VIDEO_FRAME_RATE ||
		property == VideoContentProperty::VIDEO_FRAME_TYPE ||
		property == SubtitleContentProperty::SUBTITLE_VIDEO_FRAME_RATE
		) {

		update_full_length ();

	} else if (property == ContentProperty::TRIM_START) {

		set<ContentTime> check;
		BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
			check.insert (i->trim_start ());
		}

		if (check.size() == 1) {
			_trim_start->set (_parent->selected().front()->trim_start (), film_video_frame_rate);
		} else {
			_trim_start->clear ();
		}

	} else if (property == ContentProperty::TRIM_END) {

		set<ContentTime> check;
		BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
			check.insert (i->trim_end ());
		}

		if (check.size() == 1) {
			_trim_end->set (_parent->selected().front()->trim_end (), film_video_frame_rate);
		} else {
			_trim_end->clear ();
		}
	}

	if (
		property == ContentProperty::LENGTH ||
		property == ContentProperty::TRIM_START ||
		property == ContentProperty::TRIM_END ||
		property == VideoContentProperty::VIDEO_FRAME_RATE ||
		property == VideoContentProperty::VIDEO_FRAME_TYPE ||
		property == SubtitleContentProperty::SUBTITLE_VIDEO_FRAME_RATE
		) {

		update_play_length ();
	}

	if (property == VideoContentProperty::VIDEO_FRAME_RATE) {
		set<double> check;
		shared_ptr<const VideoContent> vc;
		BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
			shared_ptr<const VideoContent> t = dynamic_pointer_cast<const VideoContent> (i);
			if (t) {
				check.insert (t->video_frame_rate ());
				vc = t;
			}
		}
		if (check.size() == 1) {
			checked_set (_video_frame_rate, raw_convert<string> (vc->video_frame_rate (), 5));
			_video_frame_rate->Enable (true);
		} else {
			checked_set (_video_frame_rate, wxT (""));
			_video_frame_rate->Enable (false);
		}
	}

	if (property == SubtitleContentProperty::SUBTITLE_VIDEO_FRAME_RATE) {
		shared_ptr<const SubtitleContent> check;
		int count = 0;
		BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
			shared_ptr<const SubtitleContent> t = dynamic_pointer_cast<const SubtitleContent> (i);
			if (t) {
				check = t;
				++count;
			}
		}
		if (count == 1) {
			checked_set (_video_frame_rate, raw_convert<string> (check->subtitle_video_frame_rate (), 5));
			_video_frame_rate->Enable (true);
		} else {
			checked_set (_video_frame_rate, wxT (""));
			_video_frame_rate->Enable (false);
		}
	}

	bool have_still = false;
	BOOST_FOREACH (shared_ptr<const Content> i, _parent->selected ()) {
		shared_ptr<const ImageContent> ic = dynamic_pointer_cast<const ImageContent> (i);
		if (ic && ic->still ()) {
			have_still = true;
		}
	}

	_full_length->set_editable (have_still);
	_play_length->set_editable (!have_still);
	_set_video_frame_rate->Enable (false);
}

void
TimingPanel::position_changed ()
{
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		i->set_position (_position->get (_parent->film()->video_frame_rate ()));
	}
}

void
TimingPanel::full_length_changed ()
{
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		shared_ptr<ImageContent> ic = dynamic_pointer_cast<ImageContent> (i);
		if (ic && ic->still ()) {
			int const vfr = _parent->film()->video_frame_rate ();
			ic->set_video_length (_full_length->get (vfr).frames_round (vfr));
		}
	}
}

void
TimingPanel::trim_start_changed ()
{
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		i->set_trim_start (_trim_start->get (_parent->film()->video_frame_rate ()));
	}
}


void
TimingPanel::trim_end_changed ()
{
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		i->set_trim_end (_trim_end->get (_parent->film()->video_frame_rate ()));
	}
}

void
TimingPanel::play_length_changed ()
{
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		FrameRateChange const frc = _parent->film()->active_frame_rate_change (i->position ());
		i->set_trim_end (
			ContentTime (i->full_length() - _play_length->get (_parent->film()->video_frame_rate()), frc)
			- i->trim_start ()
			);
	}
}

void
TimingPanel::video_frame_rate_changed ()
{
	_set_video_frame_rate->Enable (true);
}

void
TimingPanel::set_video_frame_rate ()
{
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		shared_ptr<VideoContent> vc = dynamic_pointer_cast<VideoContent> (i);
		shared_ptr<DCPSubtitleContent> dsc = dynamic_pointer_cast<DCPSubtitleContent> (i);
		shared_ptr<SubRipContent> ssc = dynamic_pointer_cast<SubRipContent> (i);
		if (vc) {
			vc->set_video_frame_rate (raw_convert<double> (wx_to_std (_video_frame_rate->GetValue ())));
		} else if (dsc) {
			dsc->set_subtitle_video_frame_rate (raw_convert<double> (wx_to_std (_video_frame_rate->GetValue ())));
		} else if (ssc) {
			ssc->set_subtitle_video_frame_rate (raw_convert<double> (wx_to_std (_video_frame_rate->GetValue ())));
		}
		_set_video_frame_rate->Enable (false);
	}
}

void
TimingPanel::content_selection_changed ()
{
	bool const e = !_parent->selected().empty ();

	_position->Enable (e);
	_full_length->Enable (e);
	_trim_start->Enable (e);
	_trim_end->Enable (e);
	_play_length->Enable (e);
	_video_frame_rate->Enable (e);

	film_content_changed (ContentProperty::POSITION);
	film_content_changed (ContentProperty::LENGTH);
	film_content_changed (ContentProperty::TRIM_START);
	film_content_changed (ContentProperty::TRIM_END);
	film_content_changed (VideoContentProperty::VIDEO_FRAME_RATE);
	film_content_changed (SubtitleContentProperty::SUBTITLE_VIDEO_FRAME_RATE);
}

void
TimingPanel::film_changed (Film::Property p)
{
	if (p == Film::VIDEO_FRAME_RATE) {
		update_full_length ();
		update_play_length ();
	}
}

void
TimingPanel::trim_start_to_playhead_clicked ()
{
	DCPTime const ph = _viewer->position ();
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		if (i->position() < ph && ph < i->end ()) {
			FrameRateChange const frc = _parent->film()->active_frame_rate_change (i->position ());
			i->set_trim_start (i->trim_start() + ContentTime (ph - i->position (), frc));
		}
	}
}

void
TimingPanel::trim_end_to_playhead_clicked ()
{
	DCPTime const ph = _viewer->position ();
	BOOST_FOREACH (shared_ptr<Content> i, _parent->selected ()) {
		if (i->position() < ph && ph < i->end ()) {
			FrameRateChange const frc = _parent->film()->active_frame_rate_change (i->position ());
			i->set_trim_end (ContentTime (i->position() + i->full_length() - ph, frc) - i->trim_start());
		}

	}
}
