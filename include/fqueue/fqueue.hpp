
// Copyright (c) 2015-2016 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of FQUEUE(https://github.com/niXman/fqueue) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <cstdint>
#include <memory>

namespace fqueue {

/**************************************************************************/

struct fqueue {
	fqueue() = delete;
	fqueue(const fqueue &) = delete;
	fqueue& operator=(const fqueue &) = delete;

	fqueue(const char *fname, std::size_t fsize = 1024*1024*100);
	fqueue(fqueue &&r);
	~fqueue();
	fqueue& operator= (fqueue &&r);

	std::size_t records() const;
	bool empty() const;

	void reset();

	std::uint64_t push(const void *ptr, std::size_t size);

	struct record {
		std::uint64_t idx;
		std::unique_ptr<char[]> ptr;
		std::size_t size;
	};
	record front();
	record pop();

private:
	struct impl;
	impl *pimpl;
};

/**************************************************************************/

} // ns fqueue