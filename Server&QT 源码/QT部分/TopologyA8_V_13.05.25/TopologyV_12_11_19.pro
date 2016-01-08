#-------------------------------------------------
#
# Project created by QtCreator 2012-11-19T01:35:42
#
#-------------------------------------------------

#QT       += core gui

#TARGET = TopologyV_12_11_19
#TEMPLATE = app
TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(TopologyV1.0)

SOURCES +=\
        topologymodule.cpp \
    node.cpp \
    module.cpp \
    graphwidget.cpp \
    edge.cpp

HEADERS  += \
    edge.h \
    topologymodule.hpp \
    node.h \
    graphwidget.h \
    abstractteleservice.hpp \
    abstractsoundservice.hpp \
    abstractserialservice.hpp \
    abstractmodule.hpp \
    module.hpp

RESOURCES += \
    image.qrc
