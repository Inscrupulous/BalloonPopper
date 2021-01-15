#-------------------------------------------------
#
# Project created by QtCreator 2020-11-04T02:32:24
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += core #for resource?
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BalloonPopperGame
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        balloon.cpp \
        balloonserver.cpp \
        client.cpp \
        customizedialog.cpp \
        gameenddialog.cpp \
        gameplaydialog.cpp \
        hostnamedialog.cpp \
        hscoresdialog.cpp \
        lobby.cpp \
        main.cpp \
        mainwindow.cpp \
        pellet.cpp \
        playernamedialog.cpp \
        tank.cpp \
        timer.cpp

HEADERS += \
        balloon.h \
        balloonserver.h \
        client.h \
        customizedialog.h \
        defs.h \
        gameenddialog.h \
        gameplaydialog.h \
        hostnamedialog.h \
        hscoresdialog.h \
        lobby.h \
        mainwindow.h \
        messagetype.h \
        pellet.h \
        playerdata.h \
        playernamedialog.h \
        tank.h \
        timer.h

FORMS += \
        customizedialog.ui \
        gameenddialog.ui \
        gameplaydialog.ui \
        hostnamedialog.ui \
        hscoresdialog.ui \
        mainwindow.ui \
        playernamedialog.ui \
        server.ui \
        timer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
