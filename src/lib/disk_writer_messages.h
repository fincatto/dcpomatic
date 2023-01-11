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


#include <boost/optional.hpp>
#include <string>


class Nanomsg;


/* We have the front-end application dcpomatic2_disk and the back-end
 * dcpomatic2_disk_writer.  The communication is line-based, separated
 * by \n.
 */

/* PING */

// Front-end sends:

#define DISK_WRITER_PING "P"

// Back-end responds

#define DISK_WRITER_PONG "O"

/* REQUEST TO WRITE DCP */

// Front-end sends:

#define DISK_WRITER_WRITE "W"
// DCP pathname
// Internal name of the drive to write to

// Back-end responds:

// everything is ok
#define DISK_WRITER_OK "D"

// there was an error
#define DISK_WRITER_ERROR "E"
// Error message
// Error number

// the drive is being formatted, 40% done
#define DISK_WRITER_FORMAT_PROGRESS "F"
// 0.4\n

// data is being copied, 30% done
#define DISK_WRITER_COPY_PROGRESS "C"
// 0.3\n

// data is being verified, 60% done
#define DISK_WRITER_VERIFY_PROGRESS "V"
// 0.6\n


/* REQUEST TO QUIT */

// Front-end sends:
#define DISK_WRITER_QUIT "Q"


/* REQUEST TO UNMOUNT A DRIVE */

// Front-end sends:
#define DISK_WRITER_UNMOUNT "U"
// XML representation of Drive object to unmount

// Back-end responds:
// DISK_WRITER_OK
// or
// DISK_WRITER_ERROR


class DiskWriterBackEndResponse
{
public:
	enum class Type {
		OK,
		ERROR,
		PONG,
		FORMAT_PROGRESS,
		COPY_PROGRESS,
		VERIFY_PROGRESS
	};

	static DiskWriterBackEndResponse ok() {
		return DiskWriterBackEndResponse(Type::OK);
	}

	static DiskWriterBackEndResponse error(std::string message, int number) {
		auto r = DiskWriterBackEndResponse(Type::ERROR);
		r._error_message = message;
		r._error_number = number;
		return r;
	}

	static DiskWriterBackEndResponse pong() {
		return DiskWriterBackEndResponse(Type::PONG);
	}

	static DiskWriterBackEndResponse format_progress(float p) {
		auto r = DiskWriterBackEndResponse(Type::FORMAT_PROGRESS);
		r._progress = p;
		return r;
	}

	static DiskWriterBackEndResponse copy_progress(float p) {
		auto r = DiskWriterBackEndResponse(Type::COPY_PROGRESS);
		r._progress = p;
		return r;
	}

	static DiskWriterBackEndResponse verify_progress(float p) {
		auto r = DiskWriterBackEndResponse(Type::VERIFY_PROGRESS);
		r._progress = p;
		return r;
	}

	static boost::optional<DiskWriterBackEndResponse> read_from_nanomsg(Nanomsg& nanomsg, int timeout);

	Type type() const {
		return _type;
	}

	std::string error_message() const {
		return _error_message;
	}

	int error_number() const {
		return _error_number;
	}

	float progress() const {
		return _progress;
	}

private:
	DiskWriterBackEndResponse(Type type)
		: _type(type)
	{}

	Type _type;
	std::string _error_message;
	int _error_number = 0;
	float _progress = 0;
};

