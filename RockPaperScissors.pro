QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    download_manager.cpp \
    main.cpp \
    mainwindow.cpp \
    version_manager.cpp

HEADERS += \
    download_manager.h \
    mainwindow.h \
    version_manager.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    RockPaperScissors_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QT += network widgets
CONFIG += console

DISTFILES += \
    C:/Users/rgb/Desktop/token.txt.txt
