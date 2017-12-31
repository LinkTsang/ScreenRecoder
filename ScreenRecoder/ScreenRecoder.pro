#-------------------------------------------------
#
# Project created by QtCreator 2017-11-24T22:51:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScreenRecoder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

QT += winextras
LIBS += -lKernel32 -lUser32 -lGdi32

SOURCES += \
    main.cpp \
    screenrecorder.cpp \
    recorderwindow.cpp \
    helper.cpp \
    drawwidget.cpp \
    mylabel.cpp \
    editorwindow.cpp \
    windowmanager.cpp \
    outputwindow.cpp

HEADERS += \
    screenrecorder.h \
    recorderwindow.h \
    helper.h \
    drawwidget.h \
    mylabel.h \
    editorwindow.h \
    windowmanager.h \
    outputwindow.h

FORMS += \
    recorderwindow.ui \
    outputwindow.ui

RESOURCES += \
    res/res.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GifOutput/release/ -lGifOutput
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GifOutput/debug/ -lGifOutput
else:unix: LIBS += -L$$OUT_PWD/../GifOutput/ -lGifOutput

INCLUDEPATH += $$PWD/../GifOutput
DEPENDPATH += $$PWD/../GifOutput

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GifOutput/release/libGifOutput.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GifOutput/debug/libGifOutput.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GifOutput/release/GifOutput.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GifOutput/debug/GifOutput.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GifOutput/libGifOutput.a
