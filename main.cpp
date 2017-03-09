
// Copyright (c) 2015-2017 niXman (i dot nixman dog gmail dot com). All
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

#include <fqueue/fqueue.hpp>

#include <iostream>
#include <utility>
#include <random>
#include <queue>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>

/**************************************************************************/

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#define MY_ASSERT(...) \
    if ( !(__VA_ARGS__) ) { \
        std::cerr << "expression error(" __FILE__ ":" STRINGIZE(__LINE__) "): \"" #__VA_ARGS__ "\"" << std::endl; \
        std::abort(); \
    }

#define RUN_TEST(testname) \
    std::cout << "exec test \"" << #testname << "\"..." << std::flush; \
    testname; \
    std::cout << "done" << std::endl << std::flush

/**************************************************************************/

std::size_t file_size(const char *fname) {
    struct stat st;
    MY_ASSERT(0 == ::stat(fname, &st));

    return st.st_size;
}

/**************************************************************************/

std::pair<std::unique_ptr<char[]>, std::size_t>
make_buf(std::size_t minsize, std::size_t maxsize) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minsize, maxsize);
    std::size_t size = dis(gen);

    std::unique_ptr<char[]> ptr(new char[size]);
    return std::make_pair(std::move(ptr), size);
}

/**************************************************************************/

void test00(std::size_t iterations, std::size_t minsize, std::size_t maxsize) {
    static const char *fn = "fq0.dat";
    ::remove(fn);

    std::uint64_t idxw = UINT64_MAX, idxr = UINT64_MAX;
    std::queue<std::size_t> sizes;

    fqueue::fqueue fq(fn);
    for ( std::size_t i = 0; i < iterations; ++i ) {
        const auto buf = make_buf(minsize, maxsize);
        idxw = fq.push(buf.first.get(), buf.second);
        sizes.push(buf.second);
    }

    MY_ASSERT(fq.records() == iterations);

    for ( std::size_t i = 0; i < iterations; ++i ) {
        fqueue::fqueue::record rec = fq.pop();
        idxr = rec.idx;
        MY_ASSERT(sizes.front() == rec.size)
        sizes.pop();
    }

    MY_ASSERT(idxw == idxr);
    MY_ASSERT(fq.records() == 0);
}

/**************************************************************************/

void test01(std::size_t iterations, std::size_t minsize, std::size_t maxsize) {
    static const char *fn = "fq1.dat";
    ::remove(fn);

    fqueue::fqueue fq(fn, 0);
    MY_ASSERT(file_size(fn) == sizeof(std::uint64_t)*4);
    for ( std::size_t i = 0; i < iterations; ++i ) {
        const auto buf = make_buf(minsize, maxsize);
        std::uint64_t ii = fq.push(buf.first.get(), buf.second);
        MY_ASSERT(ii == i+1);
        MY_ASSERT(fq.records() == 1);

        fqueue::fqueue::record rec = fq.pop();
        MY_ASSERT(fq.records() == 0);
        MY_ASSERT(rec.size == buf.second);
        MY_ASSERT(0 == std::memcmp(rec.ptr.get(), buf.first.get(), buf.second));
    }

    MY_ASSERT(file_size(fn) != sizeof(std::uint64_t)*4);
    fq.truncate();
    MY_ASSERT(file_size(fn) == sizeof(std::uint64_t)*4);
}

/**************************************************************************/

void test02(std::size_t iterations, std::size_t minsize, std::size_t maxsize) {
    (void)iterations;

    static const char *fn = "fq2.dat";
    ::remove(fn);

    fqueue::fqueue fq(fn);
    MY_ASSERT(fq.records() == 0);
    MY_ASSERT(fq.empty() == true);
    MY_ASSERT(fq.index() == 0);

    const auto buf = make_buf(minsize, maxsize);

    fq.push(buf.first.get(), buf.second);
    MY_ASSERT(fq.records() == 1);
    MY_ASSERT(fq.index() == 1);

    fq.push(buf.first.get(), buf.second);
    MY_ASSERT(fq.records() == 2);
    MY_ASSERT(fq.index() == 2);

    fq.pop();
    MY_ASSERT(fq.records() == 1);
    MY_ASSERT(fq.index() == 2);

    fq.pop();
    MY_ASSERT(fq.records() == 0);
    MY_ASSERT(fq.empty() == true);
    MY_ASSERT(fq.index() == 2);

    bool thrown = false;
    try {
        fq.pop();
    } catch (const std::exception &) {
        thrown = true;
    }
    MY_ASSERT(true == thrown);
    MY_ASSERT(fq.index() == 2);
}

/**************************************************************************/

void test03(std::size_t iterations, std::size_t minsize, std::size_t maxsize) {
    static const char *fn = "fq3.dat";
    ::remove(fn);

    fqueue::fqueue fq(fn, 0);
    MY_ASSERT(fq.records() == 0);
    MY_ASSERT(fq.empty() == true);
    MY_ASSERT(file_size(fn) == sizeof(std::uint64_t)*4);

    for ( std::size_t i = 0; i < iterations; ++i ) {
        const auto buf = make_buf(minsize, maxsize);
        fq.push(buf.first.get(), buf.second);
    }
    MY_ASSERT(fq.records() == iterations);
    MY_ASSERT(fq.empty() == false);

    std::size_t fsize = file_size(fn);

    fq.reset();
    MY_ASSERT(fq.records() == 0);
    MY_ASSERT(fq.empty() == true);
    MY_ASSERT(fsize == file_size(fn));
}

/**************************************************************************/

int main() {
    static const std::size_t N = 1024;
    static const std::size_t minsize = 47;
    static const std::size_t maxsize = 1024*33;

    RUN_TEST(test00(N, minsize, maxsize));
    RUN_TEST(test01(N, minsize, maxsize));
    RUN_TEST(test02(N, minsize, maxsize));
    RUN_TEST(test03(N, minsize, maxsize));
}

/**************************************************************************/
