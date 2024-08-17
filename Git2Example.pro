DESTDIR = $$PWD/_bin
TEMPLATE = app
TARGET = Git2Example
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lgit2

SOURCES += \
        Git2.cpp \
        main.cpp

HEADERS += \
    Git2.h
