#-------------------------------------------------
#
# Project created by QtCreator 2012-03-31T14:01:27
#
#-------------------------------------------------

QT       += core gui network

TARGET   = IntelliHouse
TEMPLATE = app


SOURCES += \
        main.cpp \
        container.cpp \
        panel.cpp \
        qextserialport.cpp \
        qextserialbase.cpp \
        serialservice.cpp \
        teleservice.cpp \
        WidgetKeyboard.cpp \
        picturebutton.cpp \
        soundservice.cpp \
        moduleregistry.cpp \
        settings.cpp \
        coorimpl.cpp \
        netservice.cpp

HEADERS  += \
        container.hpp \
        qextserialport.h \
        qextserialbase.h \
        serialservice.hpp \
        teleservice.hpp \
        WidgetKeyboard.hpp \
        picturebutton.hpp \
        panel.hpp \
        soundservice.hpp \
        moduleregistry.hpp \
        settings.hpp \
        abstractmodule.hpp \
        coorimpl.hpp \
        abstractteleservice.hpp \
        abstractsoundservice.hpp \
        abstractserialservice.hpp \
        abstractnetservice.hpp \
        netservice.hpp \
        config_etc_cs.h

FORMS    += container.ui \
        panel.ui \
        WidgetKeyboard.ui

RESOURCES += \
        image.qrc
	
win32-g++ {
SOURCES += \
        win_qextserialport.cpp
HEADERS += \
	win_qextserialport.h
}

else:win32-msvc* {
SOURCES += \
        win_qextserialport.cpp
HEADERS += \
	win_qextserialport.h
}

else:linux-g++ {
SOURCES += \
        posix_qextserialport.cpp
HEADERS += \
	posix_qextserialport.h
}

else: {
SOURCES += \
        posix_qextserialport.cpp
HEADERS += \
	posix_qextserialport.h
}
