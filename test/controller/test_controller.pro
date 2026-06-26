QT += core testlib
CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = test_controller

DEFINES += TEST_BUILD

INCLUDEPATH += \
    $$PWD/../../src \
    $$PWD/../../src/controller \
    $$PWD/../../src/storage \
    $$PWD/../../src/view

SOURCES += \
    test_controller.cpp \
    $$PWD/../../src/controller/controller.cpp \
    $$PWD/../../src/storage/task.cpp

HEADERS += \
    mockstorage.hpp \
    mockview.hpp \
    $$PWD/../../src/controller/controller.hpp \
    $$PWD/../../src/controller/icontroller.hpp \
    $$PWD/../../src/storage/istorage.hpp \
    $$PWD/../../src/storage/task.hpp \
    $$PWD/../../src/storage/achievement.hpp \
    $$PWD/../../src/view/iview.hpp
