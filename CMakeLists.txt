cmake_minimum_required (VERSION 3.10)

project(iconenginelist)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# for QFactoryLoader to list all iconengines
find_package(Qt5 CONFIG REQUIRED COMPONENTS Core)
find_package(Qt5Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
find_package(Qt5ThemeSupport REQUIRED)

# other stuff
find_package(Qt5 CONFIG REQUIRED COMPONENTS Widgets Quick Qml)

add_executable(iconenginelist main.cpp)
target_link_libraries(iconenginelist
    PRIVATE
        Qt::GuiPrivate

        Qt::Widgets
        Qt::Quick
        Qt::Qml
)
