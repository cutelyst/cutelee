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

add_library(CuteleeQt${QT_VERSION_MAJOR}TextDocument SHARED
  bbcodebuilder.cpp
  markupdirector.cpp
  plaintextmarkupbuilder.cpp
  texthtmlbuilder.cpp
  mediawikimarkupbuilder.cpp

  markupdirector_p.h
  ${cutelee_textdocument_HEADERS}
)

generate_export_header(CuteleeQt${QT_VERSION_MAJOR}TextDocument)
add_library(Cutelee::TextDocument ALIAS CuteleeQt${QT_VERSION_MAJOR}TextDocument)
set_property(TARGET CuteleeQt${QT_VERSION_MAJOR}TextDocument PROPERTY
  EXPORT_NAME TextDocument
)
target_compile_features(CuteleeQt${QT_VERSION_MAJOR}TextDocument
  PRIVATE
    cxx_auto_type
  PUBLIC
    cxx_override
)
if (CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(TARGET CuteleeQt${QT_VERSION_MAJOR}TextDocument PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(CuteleeQt${QT_VERSION_MAJOR}TextDocument
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()

target_link_libraries(CuteleeQt${QT_VERSION_MAJOR}TextDocument
  PUBLIC Qt${QT_VERSION_MAJOR}::Gui
)

set_target_properties(CuteleeQt${QT_VERSION_MAJOR}TextDocument PROPERTIES
  VERSION    ${Cutelee_VERSION}
  SOVERSION  ${Cutelee_VERSION_MAJOR}
)

target_include_directories(CuteleeQt${QT_VERSION_MAJOR}TextDocument PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}>
)

set_property(TARGET CuteleeQt${QT_VERSION_MAJOR}TextDocument PROPERTY PUBLIC_HEADER ${cutelee_textdocument_HEADERS})
install(TARGETS CuteleeQt${QT_VERSION_MAJOR}TextDocument EXPORT cutelee_targets
  RUNTIME DESTINATION bin COMPONENT TextDocument
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT TextDocument
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT TextDocument
  PUBLIC_HEADER DESTINATION include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}/cutelee
)
