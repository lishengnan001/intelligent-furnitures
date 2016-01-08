#-------------------------------------------------
#
# Project created by QtCreator 2012-02-22T15:18:52
#
#-------------------------------------------------

#QT       += core gui

#TARGET = IntelliHouse
#TEMPLATE = app

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(IntelliHouse)

SOURCES +=\
    formlinkage.cpp \
    imagewidget.cpp \
    module.cpp \
    teleservice.cpp \
    linkage.cpp \
    button.cpp \
    camerainit.cpp \
    gprsthread.cpp

HEADERS  += \
    formlinkage.h \
    imagewidget.h \
    module.hpp \
    abstractmodule.hpp \
    teleservice.h \
    linkage.h \
    abstractteleservice.hpp \
    abstractsoundservice.hpp \
    abstractserialservice.hpp \
    button.h \
    camerainit.h \
    gprsthread.h

FORMS    += \
    formlinkage.ui

RESOURCES += \
    image.qrc

OTHER_FILES += \
    image/icon/video.png
