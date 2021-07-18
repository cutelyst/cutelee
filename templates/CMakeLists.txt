find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Qml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 COMPONENTS Core Qml REQUIRED)

add_subdirectory(lib)

if (BUILD_MAIN_PLUGINS)
  add_subdirectory(loadertags)
  add_subdirectory(defaulttags)

  add_subdirectory(defaultfilters)
endif()


if (BUILD_I18N_PLUGIN)
  add_subdirectory(i18n)
endif()

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
