
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += \
	-std=c++11

QMAKE_LFLAGS += \
	-pthread

INCLUDEPATH += \
	./include

SOURCES += \
	main.cpp \
	src/fqueue.cpp

HEADERS += \
	./include/fqueue/fqueue.hpp
