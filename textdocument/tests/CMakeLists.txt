find_package(QT NAMES Qt6 Qt5 COMPONENTS Test  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

add_library(textdocument_test_builtins STATIC
  coverageobject.cpp
)
target_link_libraries(textdocument_test_builtins Qt${QT_VERSION_MAJOR}::Test)
target_compile_features(textdocument_test_builtins PUBLIC cxx_auto_type)

macro(CUTELEE_TEXTDOCUMENT_UNIT_TESTS)
  foreach(_testname ${ARGN})
    add_executable(${_testname}_exec ${_testname}.cpp)
    add_test(${_testname} ${_testname}_exec )
    target_link_libraries(${_testname}_exec Cutelee::TextDocument textdocument_test_builtins)
    set_property(GLOBAL APPEND PROPERTY TEST_COVERAGE "${CMAKE_CURRENT_BINARY_DIR}/${_testname}_exec" )
  endforeach(_testname)
endmacro(CUTELEE_TEXTDOCUMENT_UNIT_TESTS)

cutelee_textdocument_unit_tests(
  htmlbuildertest
  plainmarkupbuildertest
)
