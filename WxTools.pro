QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    main\
    global\
    utils\
    service\
    link

SOURCES += \
    global/appruntime.cpp \
    global/usettings.cpp \
    link/linkwidget.cpp \
    main/main.cpp \
    main/mainwindow.cpp \
    utils/fileutil.cpp \
    utils/stringutil.cpp \
    service/deanservice.cpp

HEADERS += \
    global/appruntime.h \
    global/usettings.h \
    link/linkwidget.h \
    main/mainwindow.h \
    utils/dlog.h \
    utils/fileutil.h \
    utils/myjson.h \
    utils/stringutil.h \
    service/deanservice.h

FORMS += \
    link/linkwidget.ui \
    main/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
