find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui REQUIRED)

add_subdirectory(lib)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
