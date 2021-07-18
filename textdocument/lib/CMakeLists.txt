set(cutelee_textdocument_HEADERS
  cutelee_textdocument.h
  abstractmarkupbuilder.h
  bbcodebuilder.h
  markupdirector.h
  plaintextmarkupbuilder.h
  texthtmlbuilder.h
  mediawikimarkupbuilder.h
  ${CMAKE_CURRENT_BINARY_DIR}/cutelee_textdocument_export.h
)

add_library(Cutelee_TextDocument SHARED
  bbcodebuilder.cpp
  markupdirector.cpp
  plaintextmarkupbuilder.cpp
  texthtmlbuilder.cpp
  mediawikimarkupbuilder.cpp

  markupdirector_p.h
  ${cutelee_textdocument_HEADERS}
)

generate_export_header(Cutelee_TextDocument)
add_library(Cutelee::TextDocument ALIAS Cutelee_TextDocument)
set_property(TARGET Cutelee_TextDocument PROPERTY
  EXPORT_NAME TextDocument
)
target_compile_features(Cutelee_TextDocument
  PRIVATE
    cxx_auto_type
  PUBLIC
    cxx_override
)
if (CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(TARGET Cutelee_TextDocument PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(Cutelee_TextDocument
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()

target_link_libraries(Cutelee_TextDocument
  PUBLIC Qt${QT_VERSION_MAJOR}::Gui
)

set_target_properties(Cutelee_TextDocument PROPERTIES
  VERSION    ${Cutelee_VERSION}
  SOVERSION  ${Cutelee_VERSION_MAJOR}
)

target_include_directories(Cutelee_TextDocument PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}>
)

set_property(TARGET Cutelee_TextDocument PROPERTY PUBLIC_HEADER ${cutelee_textdocument_HEADERS})
install(TARGETS Cutelee_TextDocument EXPORT cutelee_targets
  RUNTIME DESTINATION bin COMPONENT TextDocument
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT TextDocument
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT TextDocument
  PUBLIC_HEADER DESTINATION include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}/cutelee
)
