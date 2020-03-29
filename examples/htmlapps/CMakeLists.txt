cmake_minimum_required(VERSION 3.5)

project(htmlapps)

find_package(Cutelee5 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

get_property(Cutelee_PLUGIN_DIR TARGET Cutelee5::defaulttags PROPERTY LOCATION)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)

configure_file(cutelee_paths.h.cmake ${PROJECT_BINARY_DIR}/cutelee_paths.h)

add_library(customplugin MODULE
  customplugin.cpp
  rssfeed.cpp
)
cutelee_adjust_plugin_name(customplugin)

find_package(Qt5 REQUIRED XmlPatterns Network)

target_link_libraries(customplugin
  Cutelee5::Templates
  Qt5::XmlPatterns
  Qt5::Network
)

add_executable(htmlapps
  main.cpp
  mainwindow.cpp
  localrequestmanager.cpp
  templatereply.cpp
)

find_package(Qt5WebKitWidgets REQUIRED)

target_link_libraries(htmlapps
  Cutelee5::Templates
  Qt5::WebKitWidgets
)
