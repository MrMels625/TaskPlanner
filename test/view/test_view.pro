QT += core gui widgets testlib
CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle
CONFIG += autouic

TEMPLATE = app
TARGET = test_view

# Макрос TEST_BUILD может использоваться в коде для отключения 
# специфичной для продакшена логики (например, в controller.cpp)
DEFINES += TEST_BUILD

INCLUDEPATH += \
    $$PWD/../../src \
    $$PWD/../../src/view \
    $$PWD/../../src/storage \
    $$PWD/../../src/controller

SOURCES += \
    test_view.cpp \
    $$PWD/../../src/view/taskplannerview.cpp \
    $$PWD/../../src/view/gamification_view.cpp \
    $$PWD/../../src/storage/task.cpp \
    $$PWD/../../src/storage/memorystorage.cpp \
    $$PWD/../../src/storage/serial-utils.cpp

HEADERS += \
    ../../src/view/gamification_view.hpp \
    mockstorage.hpp \
    $$PWD/../../src/view/taskplannerview.hpp \
    $$PWD/../../src/view/iview.hpp \
    $$PWD/../../src/view/gamification_view.hpp \
    $$PWD/../../src/view/igamification_view.hpp \
    $$PWD/../../src/storage/istorage.hpp \
    $$PWD/../../src/storage/task.hpp \
    $$PWD/../../src/storage/igamification_storage.hpp \
    $$PWD/../../src/storage/serial-utils.hpp

# Обязательно указываем UI-файл, чтобы сработал autouic и сгенерировал ui_taskplanner.h
FORMS += \
    $$PWD/../../src/view/taskplanner.ui
