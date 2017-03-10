#!/usr/bin/env bash

readonly CPPFLAGS="-I../include"
readonly CXXFLAGS="-std=c++11 -Wall -Wextra"
readonly LDFLAGS="-lrt"

function build {
    # $1 - cpp files
    # $2 - output file
    
    echo -n "building $1 ... "
    g++ ${CPPFLAGS} ${CXXFLAGS} ${1} ${LDFLAGS} -o$2
    local res=$?
    [[ $res == 0 ]] && {
        echo "done"
    } || {
        echo "error!"
        return $res
    }
    
    return 0
}

build "info.cpp ../src/fqueue.cpp" info || exit
build "unpack.cpp ../src/fqueue.cpp" unpack || exit
build "pack.cpp ../src/fqueue.cpp" pack || exit
