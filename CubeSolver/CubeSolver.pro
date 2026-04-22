QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
msvc{
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    buildcube.cpp \
    camerathread.cpp \
    cubedetect.cpp \
    cubiecube.cpp \
    main.cpp \
    mainwindow.cpp \
    solver.cpp \
    welcome.cpp

HEADERS += \
    buildcube.h \
    camerathread.h \
    cubedetect.h \
    cubiecube.h \
    mainwindow.h \
    solver.h \
    welcome.h

FORMS += \
    buildcube.ui \
    cubedetect.ui \
    mainwindow.ui \
    welcome.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 本地opencv目录，如果是MSVC编译请添加lib库
INCLUDEPATH += E:\openCV\opencv3410\opencv\opencv-build\install\include
LIBS += E:\openCV\opencv3410\opencv\opencv-build\lib\libopencv_*.a \
        E:\openCV\opencv3410\opencv\build\x64\vc15\lib\*.lib

RESOURCES += \
    resource.qrc
