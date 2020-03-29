add_library(cutelee_i18ntags MODULE
  i18ntags.cpp
  i18n.cpp
  i18nc.cpp
  i18np.cpp
  i18ncp.cpp
  l10n_money.cpp
  with_locale.cpp
)
set_property(TARGET cutelee_i18ntags PROPERTY
  EXPORT_NAME i18ntags
)
target_link_libraries(cutelee_i18ntags PRIVATE
  Cutelee5::Templates
)
target_compile_features(cutelee_i18ntags PRIVATE
  cxx_auto_type
)
cutelee_adjust_plugin_name(cutelee_i18ntags)

install(TARGETS cutelee_i18ntags 
  EXPORT cutelee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
