add_library(cutelee_defaulttags MODULE
  defaulttags.cpp
  autoescape.cpp
  comment.cpp
  cycle.cpp
  debug.cpp
  filtertag.cpp
  firstof.cpp
  for.cpp
  if.cpp
  ifchanged.cpp
  ifequal.cpp
  load.cpp
  mediafinder.cpp
  now.cpp
  range.cpp
  regroup.cpp
  spaceless.cpp
  templatetag.cpp
  widthratio.cpp
  with.cpp
)
set_property(TARGET cutelee_defaulttags PROPERTY
  EXPORT_NAME defaulttags
)
target_link_libraries(cutelee_defaulttags PRIVATE
  Cutelee5::Templates
)
target_compile_features(cutelee_defaulttags PRIVATE
  cxx_auto_type
  cxx_variadic_templates
)
cutelee_adjust_plugin_name(cutelee_defaulttags)

install(TARGETS cutelee_defaulttags 
  EXPORT cutelee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
