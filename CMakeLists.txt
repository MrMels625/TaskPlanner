cmake_minimum_required(VERSION 3.16)

project(TaskPlanner VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)

set(PROJECT_SOURCES
    src/main/main.cpp
    src/main/taskplanner.cpp
    src/main/taskplanner.hpp
    src/main/taskplanner.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TaskPlanner
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/storage/task.hpp
        src/storage/task.cpp
        src/storage/istorage.hpp
        src/storage/memorystorage.hpp
        src/storage/memorystorage.cpp
        src/view/iview.hpp
        src/controller/icontroller.hpp
        src/storage/serial-utils.hpp
        src/storage/serial-utils.cpp
    )
else()
    if(ANDROID)
        add_library(TaskPlanner SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(TaskPlanner
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TaskPlanner PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TaskPlanner)
endif()
set_target_properties(TaskPlanner PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TaskPlanner
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TaskPlanner)
endif()