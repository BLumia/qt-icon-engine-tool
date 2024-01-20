cmake_minimum_required (VERSION 3.10)

project(iconenginelist)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

option(PREFER_QT_5 "Prefer to use Qt 5 even if we have Qt 6" OFF)

if(PREFER_QT_5)
    set(QT_NS Qt5)
else()
    set(QT_NS Qt6)
endif()

find_package(QT NAMES ${QT_NS} REQUIRED COMPONENTS Core)

# for QFactoryLoader to list all iconengines
find_package(${QT_NS} CONFIG REQUIRED COMPONENTS Core)
find_package(${QT_NS}Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
if(${QT_VERSION_MAJOR} EQUAL "5")
    find_package(Qt5ThemeSupport REQUIRED)
endif()

# other stuff
find_package(${QT_NS} CONFIG REQUIRED COMPONENTS Widgets Quick Qml)

add_executable(iconenginelist main.cpp)
target_link_libraries(iconenginelist
    PRIVATE
        Qt::GuiPrivate

        Qt::Widgets
        Qt::Quick
        Qt::Qml
)
