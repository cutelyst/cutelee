cmake_minimum_required(VERSION 3.5)

project(codegen_cutelee)

find_package(Cutelee REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

get_property(Cutelee_PLUGIN_DIR TARGET Cutelee::defaulttags PROPERTY LOCATION)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)
get_filename_component(Cutelee_PLUGIN_DIR ${Cutelee_PLUGIN_DIR} PATH)

configure_file(cutelee_paths.h.cmake ${PROJECT_BINARY_DIR}/cutelee_paths.h)

qt5_add_resources(
  codegen_example_RCS_SRCS
  custom_tags.qrc
  OPTIONS -root "/plugins/cutelee/${Cutelee_VERSION_MAJOR}.${Cutelee_VERSION_MINOR}/"
)

add_executable(codegen
  codegen.qrc
  comboboxdelegate.cpp
  main.cpp
  methodmodel.cpp
  codegentableview.cpp
  designwidget.cpp
  propertytablewidget.cpp
  mainwindow.cpp
  ${codegen_example_RCS_SRCS}
)
target_compile_definitions(codegen PRIVATE QT_DISABLE_DEPRECATED_BEFORE=0)

find_package(Qt5Widgets REQUIRED)

target_link_libraries(
  codegen
  Cutelee::Templates
  Qt5::Widgets
)
