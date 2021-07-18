add_library(cutelee_defaultfilters MODULE
  defaultfilters.cpp
  datetime.cpp
  integers.cpp
  lists.cpp
  logic.cpp
  misc.cpp
  stringfilters.cpp
)
set_property(TARGET cutelee_defaultfilters PROPERTY
    EXPORT_NAME defaultfilters
)
target_link_libraries(cutelee_defaultfilters PRIVATE
  Cutelee::Templates
)
target_compile_features(cutelee_defaultfilters PRIVATE
  cxx_auto_type
)
cutelee_adjust_plugin_name(cutelee_defaultfilters)

install(TARGETS cutelee_defaultfilters
  EXPORT cutelee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
