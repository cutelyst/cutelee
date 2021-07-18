cmake_minimum_required(VERSION 3.5)

project(textedit_cutelee)

find_package(Cutelee REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

## Cutelee plugin

add_library(customtags MODULE
  customtagslibrary.cpp
  audiotextdocumentoutputter.cpp
  filters.cpp
)
cutelee_adjust_plugin_name(customtags)

target_link_libraries(customtags
  Cutelee::Templates
  Cutelee::TextDocument
)


## Application

get_property(Cutelee_PLUGIN_DIR TARGET Cutelee::defaulttags PROPERTY LOCATION)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)

configure_file(cutelee_paths.h.cmake ${PROJECT_BINARY_DIR}/cutelee_paths.h)


add_executable(textedit
  main.cpp
  audioobject.cpp
  textedit.cpp
  textedit.qrc
)

target_link_libraries(
  textedit
  Cutelee::Templates
  Qt5::Widgets
)
