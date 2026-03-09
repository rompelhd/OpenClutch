cmake_minimum_required(VERSION 3.16)
project(OpenACLauncher)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(OpenACLauncher
    src/main.cpp
    src/MainWindow.cpp
    src/ACManager.cpp
    src/SettingsDialog.cpp

    src/SinglePlayerScreen.cpp
    src/CarSelectorDialog.cpp
    src/SkinSelectorDialog.cpp
    src/CircuitSelectorDialog.cpp
)

find_package(nlohmann_json REQUIRED)
target_link_libraries(OpenACLauncher Qt6::Widgets nlohmann_json::nlohmann_json)

target_link_libraries(OpenACLauncher Qt6::Widgets)
