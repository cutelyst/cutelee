
add_library(Cutelee_TextDocument SHARED
  bbcodebuilder.cpp
  markupdirector.cpp
  plaintextmarkupbuilder.cpp
  texthtmlbuilder.cpp
  mediawikimarkupbuilder.cpp

  # Help IDEs find some non-compiled files.
  abstractmarkupbuilder.h
  cutelee_textdocument.h
  markupdirector_p.h
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
  PUBLIC Qt5::Gui
)

set_target_properties(Cutelee_TextDocument PROPERTIES
  VERSION    ${Cutelee5_VERSION}
  SOVERSION  ${Cutelee5_VERSION_MAJOR}
)

install(TARGETS Cutelee_TextDocument EXPORT cutelee_targets
  RUNTIME DESTINATION bin COMPONENT TextDocument
  LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT TextDocument
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT TextDocument
  INCLUDES DESTINATION include
)

install(FILES
  abstractmarkupbuilder.h
  bbcodebuilder.h
  markupdirector.h
  plaintextmarkupbuilder.h
  texthtmlbuilder.h
  mediawikimarkupbuilder.h
  ${CMAKE_CURRENT_BINARY_DIR}/cutelee_textdocument_export.h
  DESTINATION include/cutelee COMPONENT TextDocument
)

install(FILES
  cutelee_textdocument.h
  DESTINATION include COMPONENT TextDocument
)
