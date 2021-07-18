
include(CMakeParseArguments)

macro(cutelee_adjust_plugin_name pluginname)
  set_target_properties(${pluginname}
    PROPERTIES PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cutelee/${Cutelee_VERSION_MAJOR}.${Cutelee_VERSION_MINOR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cutelee/${Cutelee_VERSION_MAJOR}.${Cutelee_VERSION_MINOR}"
    DEBUG_POSTFIX "d"
  )
  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(${pluginname}
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_BINARY_DIR}/cutelee/${Cutelee_VERSION_MAJOR}.${Cutelee_VERSION_MINOR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_BINARY_DIR}/cutelee/${Cutelee_VERSION_MAJOR}.${Cutelee_VERSION_MINOR}"
      )
  endforeach()
endmacro()
