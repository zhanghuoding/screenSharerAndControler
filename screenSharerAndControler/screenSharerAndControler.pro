#-------------------------------------------------
#
# Project created by QtCreator 2018-10-05T15:18:31
#
#-------------------------------------------------

QT       += core gui \
            network \
            multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#This APP's full name is screenSharerAndControler.
#And it's brief name is SSAC.

TARGET = SSAC
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += ANDROID
#DEFINES += DEBUG
#DEFINES += DESKTOP

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    rootwidget.cpp \
    tool.cpp \
    imageButton.cpp \
    textButton.cpp \
    pQWidget.cpp \
    newLineEdit.cpp \
    getLocalHostInfo.cpp \
    localHostInfo.cpp \
    connectPartnerHostAndGetScreen.cpp \
    controlRemoteDevice.cpp \
    connectByPartnerHostAndShareScreen.cpp \
    tcpConnecterWithServer.cpp \
    QScreenRecorder/qScreenRecorder.cpp \
    QScreenRecorder/qScreenRecorderResultReceiver.cpp \
    QScreenRecorder/myMedia.cpp

HEADERS += \
        mainwindow.h \
    variable.h \
    Singleton/call_once.h \
    Singleton/singleton.h \
    rootwidget.h \
    tool.h \
    imageButton.h \
    type.h \
    textButton.h \
    pQWidget.h \
    newLineEdit.h \
    getLocalHostInfo.h \
    localHostInfo.h \
    connectPartnerHostAndGetScreen.h \
    controlRemoteDevice.h \
    connectByPartnerHostAndShareScreen.h \
    tcpConnecterWithServer.h \
    QScreenRecorder/qScreenRecorder.h \
    QScreenRecorder/qScreenRecorderResultReceiver.h \
    QScreenRecorder/myMedia.h

CONFIG += mobility
MOBILITY = 



if(contains(DEFINES,ANDROID)){

android{
QT += androidextras
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/assets/resource/image/icon.png \
    #android/src/com/screenRecorder/MainActivity.java \
    #android/src/com/screenRecorder/ScreenRecorder.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

}
