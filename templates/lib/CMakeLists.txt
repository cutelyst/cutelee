find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 COMPONENTS Core REQUIRED)

configure_file(cutelee_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/cutelee_version.h)

set(Cutelee_PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR} )

configure_file(cutelee_config_p.h.cmake  ${CMAKE_CURRENT_BINARY_DIR}/cutelee_config_p.h)

set_source_files_properties(taglibraryinterface.h PROPERTIES
  SKIP_AUTOMOC TRUE
)

add_library(Cutelee_Templates SHARED
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

  # Help IDEs find some non-compiled files.
  customtyperegistry_p.h
  engine_p.h
  exception.h
  cutelee_tags_p.h
  cutelee_templates.h
  lexer_p.h
  metaenumvariable_p.h
  nodebuiltins_p.h
  nulllocalizer_p.h
  pluginpointer_p.h
  statemachine_p.h
  taglibraryinterface.h
  template_p.h
  textprocessingmachine_p.h
  token.h
  typeaccessor.h
)
add_library(Cutelee::Templates ALIAS Cutelee_Templates)
generate_export_header(Cutelee_Templates)
set_property(TARGET Cutelee_Templates PROPERTY
  EXPORT_NAME Templates
)
target_compile_features(Cutelee_Templates
  PRIVATE
    cxx_auto_type
  PUBLIC
    cxx_override
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")

  set_property(TARGET Cutelee_Templates PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(Cutelee_Templates
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()
target_compile_definitions(Cutelee_Templates
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

  target_sources(Cutelee_Templates PRIVATE ${scriptabletags_SRCS})
  target_include_directories(Cutelee_Templates PRIVATE ../scriptabletags)
  target_link_libraries(Cutelee_Templates
    PRIVATE Qt${QT_VERSION_MAJOR}::Qml
  )
endif()

if (BUILD_TESTS)
  set(CUTELEE_TESTS_EXPORT "CUTELEE_TEMPLATES_EXPORT")
endif()

configure_file(cutelee_test_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/cutelee_test_export.h")

target_link_libraries(Cutelee_Templates
  LINK_PUBLIC Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(Cutelee_Templates PROPERTIES
  VERSION    ${Cutelee_VERSION}
  SOVERSION  ${Cutelee_VERSION_MAJOR}
)

install(TARGETS Cutelee_Templates EXPORT cutelee_targets
  RUNTIME DESTINATION bin COMPONENT Templates
  LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Templates
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Templates
  INCLUDES DESTINATION include
)

install(FILES
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
  DESTINATION include/cutelee COMPONENT Templates
)

install(FILES
  cutelee_templates.h
  DESTINATION include COMPONENT Templates
)
