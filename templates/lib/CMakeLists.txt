find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 COMPONENTS Core REQUIRED)

set(templates_target_name CuteleeQt${QT_VERSION_MAJOR}Templates)

configure_file(cutelee_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/cutelee_version.h)

set(Cutelee_PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR} )

configure_file(cutelee_config_p.h.cmake  ${CMAKE_CURRENT_BINARY_DIR}/cutelee_config_p.h)

set_source_files_properties(taglibraryinterface.h PROPERTIES
  SKIP_AUTOMOC TRUE
)

set(cutelee_templates_HEADERS
  cutelee_templates.h
  abstractlocalizer.h
  cachingloaderdecorator.h
  context.h
  engine.h
  exception.h
  filter.h
  filterexpression.h
  ${CMAKE_CURRENT_BINARY_DIR}/cutelee_templates_export.h
  ${CMAKE_CURRENT_BINARY_DIR}/cutelee_version.h
  metatype.h
  node.h
  outputstream.h
  parser.h
  qtlocalizer.h
  rendercontext.h
  safestring.h
  taglibraryinterface.h
  template.h
  templateloader.h
  typeaccessor.h
  token.h
  util.h
  variable.h
)

add_library(${templates_target_name} SHARED
  abstractlocalizer.cpp
  cachingloaderdecorator.cpp
  customtyperegistry.cpp
  context.cpp
  engine.cpp
  filter.cpp
  filterexpression.cpp
  lexer.cpp
  metatype.cpp
  node.cpp
  nodebuiltins.cpp
  nulllocalizer.cpp
  outputstream.cpp
  parser.cpp
  qtlocalizer.cpp
  rendercontext.cpp
  safestring.cpp
  template.cpp
  templateloader.cpp
  textprocessingmachine.cpp
  typeaccessors.cpp
  util.cpp
  variable.cpp

  nodebuiltins_p.h
  nulllocalizer_p.h
  pluginpointer_p.h
  statemachine_p.h
  template_p.h
  textprocessingmachine_p.h
  customtyperegistry_p.h
  engine_p.h
  cutelee_tags_p.h
  lexer_p.h
  metaenumvariable_p.h

  ${cutelee_templates_HEADERS}
)

add_library(Cutelee::Templates ALIAS ${templates_target_name})
generate_export_header(${templates_target_name} BASE_NAME cutelee_templates)
set_property(TARGET ${templates_target_name} PROPERTY
  EXPORT_NAME Templates
)
target_compile_features(${templates_target_name}
  PRIVATE
    cxx_auto_type
  PUBLIC
    cxx_override
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")

  set_property(TARGET ${templates_target_name} PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(${templates_target_name}
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()
target_compile_definitions(${templates_target_name}
  PRIVATE
    PLUGINS_PREFER_DEBUG_POSTFIX=$<CONFIG:Debug>
)

if (Qml_FOUND)
  set(scriptabletags_FILES
    scriptablecontext.cpp
    scriptablefilterexpression.cpp
    scriptablenode.cpp
    scriptableparser.cpp
    scriptablesafestring.cpp
    scriptabletags.cpp
    scriptabletemplate.cpp
    scriptablevariable.cpp
    scriptablefilter.cpp
    )

  foreach(file ${scriptabletags_FILES})
    list(APPEND scriptabletags_SRCS ${CMAKE_SOURCE_DIR}/templates/scriptabletags/${file})
  endforeach()

  target_sources(${templates_target_name} PRIVATE ${scriptabletags_SRCS})
  target_include_directories(${templates_target_name} PRIVATE ../scriptabletags)
  target_link_libraries(${templates_target_name}
    PRIVATE Qt${QT_VERSION_MAJOR}::Qml
  )
endif()

if (BUILD_TESTS)
  set(CUTELEE_TESTS_EXPORT "CUTELEE_TEMPLATES_EXPORT")
endif()

configure_file(cutelee_test_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/cutelee_test_export.h")

target_link_libraries(${templates_target_name}
  LINK_PUBLIC Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(${templates_target_name} PROPERTIES
  VERSION    ${Cutelee_VERSION}
  SOVERSION  ${Cutelee_VERSION_MAJOR}
)

target_include_directories(${templates_target_name} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}>
)

set_property(TARGET ${templates_target_name} PROPERTY PUBLIC_HEADER ${cutelee_templates_HEADERS})
install(TARGETS ${templates_target_name} EXPORT cutelee_targets
  RUNTIME DESTINATION bin COMPONENT Templates
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Templates
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Templates
  PUBLIC_HEADER DESTINATION include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}/cutelee
)
