# WebViewBase 组件 —— 供其他项目通过 include(webviewbase.pri) 引用

QT += core gui webenginewidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

win32: LIBS += -luser32

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/webpagebase.cpp \
    $$PWD/webviewbase.cpp

HEADERS += \
    $$PWD/webpagebase.h \
    $$PWD/webviewbase.h

FORMS += \
    $$PWD/webviewbase.ui
