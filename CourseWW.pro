TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ast.cpp \
        lexer.cpp \
        main.cpp \
        parser.cpp

DISTFILES += \ \
    program.txt

HEADERS += \
    ast.h \
    error.h \
    lexer.h \
    parser.h
