set_source_files_properties(blockcontext.h
  PROPERTIES SKIP_AUTOMOC TRUE
)

add_library(cutelee_loadertags MODULE
  loadertags.cpp
  blockcontext.cpp
  block.cpp
  extends.cpp
  include.cpp
)
set_property(TARGET cutelee_loadertags PROPERTY
  EXPORT_NAME loadertags
)
target_link_libraries(cutelee_loadertags PRIVATE
  Cutelee5::Templates
)
target_compile_features(cutelee_loadertags PRIVATE
  cxx_auto_type
)
cutelee_adjust_plugin_name(cutelee_loadertags)

install(TARGETS cutelee_loadertags
  EXPORT cutelee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
