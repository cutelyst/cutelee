cmake_minimum_required(VERSION 3.5)

project(books_cutelee)

find_package(Cutelee5 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

get_property(Cutelee_PLUGIN_DIR TARGET Cutelee5::defaulttags PROPERTY LOCATION)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)

configure_file(cutelee_paths.h.cmake ${PROJECT_BINARY_DIR}/cutelee_paths.h)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)

add_executable(books
  main.cpp
  bookwindow.cpp
  bookwrapper.cpp
  bookdelegate.cpp
  books.qrc
)

target_link_libraries(
  books
  Cutelee5::Templates
  Qt5::Widgets
  Qt5::Sql

)
