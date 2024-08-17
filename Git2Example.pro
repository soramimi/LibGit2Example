DESTDIR = $$PWD/_bin
TEMPLATE = app
TARGET = Git2Example
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

win32:INCLUDEPATH += $$PWD/libgit2/include

win32:LIBS += -lCrypt32 -lAdvapi32 -lSecur32 -lDbghelp $$PWD/_bin/libgit2.lib
!win32:LIBS += -lgit2

SOURCES += \
	Git2.cpp \
	main.cpp

HEADERS += \
	Git2.h \
	Git2.h


HEADERS += \
	Git2.h


HEADERS += \
	Git2.h


HEADERS += \
	Git2.h


HEADERS += \
	Git2.h


HEADERS += \
	Git2.h


HEADERS += \
	Git2.h


HEADERS += \
	Git2.h
