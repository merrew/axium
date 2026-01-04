include(JKQtPlotter-master/JKQtPlotter-master/lib/jkqtmathtext.pri)
include(JKQtPlotter-master/JKQtPlotter-master/lib/jkqtcommon.pri)
TEMPLATE = app
CONFIG += c++17

QT += widgets openglwidgets

TARGET = widget

SOURCES += \
    $$files(parser/*.cpp) \
    Axes.cpp \
    Colors.cpp \
    Fonts.cpp \
    FuncGL.cpp \
    Graph_function.cpp \
    Grid.cpp \
    OpenGLContainer.cpp \
    functions.cpp \
    main.cpp \
    mathoperators.cpp \
    panel.cpp \
    toolspanel.cpp

HEADERS += \
    $$files(parser/*.h) \
    Axes.h \
    Colors.h \
    Fonts.h \
    FuncGL.h \
    Graph_function.h \
    Grid.h \
    OpenGLContainer.h \
    Widget.h \
    functions.h \
    mathoperators.h \
    panel.h \
    toolspanel.h

RESOURCES += \
    resources.qrc
