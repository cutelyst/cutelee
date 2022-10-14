set(textdocs_target_name CuteleeQt${QT_VERSION_MAJOR}TextDocument)

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

add_library(${textdocs_target_name} SHARED
  bbcodebuilder.cpp
  markupdirector.cpp
  plaintextmarkupbuilder.cpp
  texthtmlbuilder.cpp
  mediawikimarkupbuilder.cpp

  markupdirector_p.h
  ${cutelee_textdocument_HEADERS}
)

generate_export_header(${textdocs_target_name})
add_library(Cutelee::TextDocument ALIAS ${textdocs_target_name})
set_property(TARGET ${textdocs_target_name} PROPERTY
  EXPORT_NAME TextDocument
)
target_compile_features(${textdocs_target_name}
  PRIVATE
    cxx_auto_type
  PUBLIC
    cxx_override
)
if (CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(TARGET ${textdocs_target_name} PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(${textdocs_target_name}
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()

target_link_libraries(${textdocs_target_name}
  PUBLIC Qt${QT_VERSION_MAJOR}::Gui
)

set_target_properties(${textdocs_target_name} PROPERTIES
  VERSION    ${Cutelee_VERSION}
  SOVERSION  ${Cutelee_VERSION_MAJOR}
)

target_include_directories(${textdocs_target_name} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}>
)

set_property(TARGET ${textdocs_target_name} PROPERTY PUBLIC_HEADER ${cutelee_textdocument_HEADERS})
install(TARGETS ${textdocs_target_name} EXPORT cutelee_targets
  RUNTIME DESTINATION bin COMPONENT TextDocument
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT TextDocument
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT TextDocument
  PUBLIC_HEADER DESTINATION include/cutelee${PROJECT_VERSION_MAJOR}-qt${QT_VERSION_MAJOR}/cutelee
)
