find_package(QT NAMES Qt6 Qt5 COMPONENTS Test LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test LinguistTools REQUIRED)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

set( CUTELEE_PLUGIN_PATH "${CMAKE_BINARY_DIR}/" )

configure_file(cutelee_paths.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/cutelee_paths.h)

if (LinguistTools_FOUND)
  set(TESTS_LOCALES "fr_FR" "de_DE")

  set(QT_LUPDATE_EXECUTABLE Qt${QT_VERSION_MAJOR}::lupdate)
  set(QT_LRELEASE_EXECUTABLE Qt${QT_VERSION_MAJOR}::lrelease)

  set(LINGUIST_TS_ARGS COMMAND "${QT_LUPDATE_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test_input.cpp" "-ts")

  foreach(TESTS_LOCALE ${TESTS_LOCALES})
      set( LINGUIST_TS_ARGS ${LINGUIST_TS_ARGS} "${CMAKE_CURRENT_SOURCE_DIR}/test_${TESTS_LOCALE}.ts" )
  endforeach()

  add_custom_target(test_l10n
    ${LINGUIST_TS_ARGS}
  )

  set( LINGUIST_QM_ARGS COMMAND "${QT_LRELEASE_EXECUTABLE}" )

  foreach(TESTS_LOCALE ${TESTS_LOCALES})
      set( generate_QMs ${generate_QMs} ${LINGUIST_QM_ARGS} "${CMAKE_CURRENT_SOURCE_DIR}/test_${TESTS_LOCALE}.ts" "-qm" "${CMAKE_CURRENT_BINARY_DIR}/test_${TESTS_LOCALE}.qm" )
  endforeach()

  add_custom_command(
    OUTPUT test_de_DE.qm test_fr_FR.qm
    ${generate_QMs}
    DEPENDS test_de_DE.ts test_fr_FR.ts
  )

  set(QM_FILES "
<qresource>
    <file alias=\"test_de_DE.qm\">${CMAKE_CURRENT_BINARY_DIR}/test_de_DE.qm</file>
    <file alias=\"test_fr_FR.qm\">${CMAKE_CURRENT_BINARY_DIR}/test_fr_FR.qm</file>
</qresource>")
endif()

configure_file(testresource.qrc.cmake "${CMAKE_CURRENT_BINARY_DIR}/testresource.qrc")

set (_rcc_file "${CMAKE_CURRENT_BINARY_DIR}/testresource.qrc")

if (${QT_VERSION_MAJOR} STREQUAL "5")
  qt5_add_resources(_testresource_rcc_src ${_rcc_file} OPTIONS -root "/" )
else()
  qt_add_resources(_testresource_rcc_src ${_rcc_file} OPTIONS -root "/" )
endif()

add_library(template_test_builtins STATIC coverageobject.cpp
  coverageobject.cpp
  ${_testresource_rcc_src}
)
target_link_libraries(template_test_builtins Qt${QT_VERSION_MAJOR}::Test)
target_compile_definitions(template_test_builtins PUBLIC cxx_auto_type)

macro(cutelee_templates_unit_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname}_exec
                  ${_testname}.cpp
    )
    add_test(${_testname} ${_testname}_exec )
    target_link_libraries(${_testname}_exec Cutelee::Templates template_test_builtins)

    if (Qml_FOUND)
      target_compile_definitions(${_testname}_exec PRIVATE HAVE_QTQML_LIB)
    endif()

    set_property(GLOBAL APPEND PROPERTY TEST_COVERAGE "${CMAKE_CURRENT_BINARY_DIR}/${_testname}_exec" )
  endforeach(_testname)
endmacro()

cutelee_templates_unit_tests(
  testsafestring
  testbuiltins
  testloadertags
  testdefaulttags
  testcachingloader
  testfilters
  testgenerictypes
  testgenericcontainers
  testinsertlibrary
)

if (Qml_FOUND)
  cutelee_templates_unit_tests(
    testscriptabletags
  )
endif()

if (LinguistTools_FOUND)
  cutelee_templates_unit_tests(
    testinternationalization
  )
  if ("x${CMAKE_CXX_COMPILER_ID}x" MATCHES "xMSVCx")
    target_compile_options(testinternationalization_exec PRIVATE "/utf-8")
  endif()
endif()
