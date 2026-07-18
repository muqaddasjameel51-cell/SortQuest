cmake_minimum_required(VERSION 3.16)
project(Sorting VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_add_executable(untitled
    main.cpp
    mainwindow.h
    mainwindow.cpp
    stagewidget.h
    stagewidget.cpp
    sortwork.h
    sortwork.cpp
    welcomewindow.h welcomewindow.cpp
    gamewindow.h gamewindow.cpp
    gameplaywindow.h gameplaywindow.cpp
    gamestagewidget.h gamestagewidget.cpp
    bubblegameengine.h bubblegameengine.cpp
)

target_link_libraries(untitled PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

set_target_properties(untitled PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)