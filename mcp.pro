DESTDIR = $$PWD
TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

HEADERS += \
	jstream.h \
	strformat.h
