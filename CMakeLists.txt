cmake_minimum_required(VERSION 3.10)

project(Cutelee VERSION 6.1.0 LANGUAGES CXX)

# Workaround for http://public.kitware.com/Bug/view.php?id=12301
if (MINGW)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
  options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
  RelWithDebInfo MinSizeRel Coverage." FORCE)
  endif()
endif()

include(GNUInstallDirs)

set(QT_MINIMUM_VERSION 5.6.0)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} ${QT_MINIMUM_VERSION} COMPONENTS Core REQUIRED)

option(BUILD_SHARED_LIBS "Build in shared lib mode" ON)
option( BUILD_TEMPLATES "Build the Cutelee template library" TRUE )
option( BUILD_TEXTDOCUMENT "Build the Cutelee textdocument library" TRUE )
option( BUILD_MAIN_PLUGINS "Build the Cutelee Templates plugins" TRUE )
option( BUILD_I18N_PLUGIN "Build the Cutelee Templates i18n plugin" TRUE )
option( BUILD_TESTS "Build the Cutelee tests" TRUE )
option( BUILD_DOCS "Build the Cutelee API documentation using doxygen" FALSE)
option( BUILD_DOCS_QUIET "Tell doxygen to be quiet while building the documentation" FALSE)

if (BUILD_TESTS)
  enable_testing()
endif()

set(Cutelee_MODULE_DIR ${CMAKE_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${Cutelee_MODULE_DIR})

include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  # create coverage build type
  set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} Coverage )
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "Coverage")
endif()

if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
  set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common -pedantic-errors")

  if (NOT APPLE)
    set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAGS}")
    set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_MODULE_LINKER_FLAGS}")
  endif ()
endif()

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_USE_FAST_OPERATOR_PLUS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050c00
)

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_AUTOMOC ON)

set(Cutelee_MIN_PLUGIN_VERSION 0)

set(Cutelee_MAJOR_MINOR_VERSION_STRING "${Cutelee_VERSION_MAJOR}.${Cutelee_VERSION_MINOR}" )

set (LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")

set( LIB_INSTALL_DIR lib${LIB_SUFFIX} )
set( PLUGIN_INSTALL_DIR ${LIB_INSTALL_DIR}/cutelee-qt${QT_VERSION_MAJOR}/${Cutelee_MAJOR_MINOR_VERSION_STRING} )

# set up RPATH/install_name_dir
set( CMAKE_INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set( CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

include("${Cutelee_MODULE_DIR}/CuteleeMacros.cmake")

if (BUILD_TEMPLATES)
  add_subdirectory(templates)
endif()

if (BUILD_TEXTDOCUMENT)
  add_subdirectory(textdocument)
endif()

set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}" )

configure_file(
  "${Cutelee_MODULE_DIR}/CuteleeConfig.cmake.in"
  "${CMAKE_BINARY_DIR}/Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}Config.cmake"
  @ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}ConfigVersion.cmake"
  VERSION ${Cutelee_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_BINARY_DIR}/Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}Config.cmake"
  "${CMAKE_BINARY_DIR}/Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}ConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Common
)

install(FILES
  "${Cutelee_MODULE_DIR}/CuteleeMacros.cmake"
  RENAME "Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}Macros.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}/"
  COMPONENT Common
)

install(EXPORT cutelee_targets
  DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  FILE "Cutelee${Cutelee_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}Targets.cmake"
  NAMESPACE Cutelee::
  COMPONENT Common
)

# Add support for an uninstall target
configure_file(
  "${Cutelee_MODULE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

if (BUILD_DOCS)
    find_package(Doxygen REQUIRED dot)

    # get qmake location and query some Qt installation paths for later use
    get_property(QMAKE_EXE_PATH TARGET Qt${QT_VERSION_MAJOR}::qmake PROPERTY LOCATION)

    execute_process(COMMAND ${QMAKE_EXE_PATH} -query QT_INSTALL_DOCS RESULT_VARIABLE QMAKE_RESULT OUTPUT_VARIABLE QT_INSTALL_DOCS OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(QMAKE_RESULT)
        message(FATAL_ERROR "Failed to use qmake to query QT_INSTALL_DOCS")
    endif(QMAKE_RESULT)

    execute_process(COMMAND ${QMAKE_EXE_PATH} -query QT_INSTALL_LIBEXECS RESULT_VARIABLE QMAKE_RESULT OUTPUT_VARIABLE QT_INSTALL_LIBEXECS OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(QMAKE_RESULT)
        message(FATAL_ERROR "Failed to use qmake to query QT_INSTALL_LIBEXECS")
    endif(QMAKE_RESULT)

    execute_process(COMMAND ${QMAKE_EXE_PATH} -query QT_INSTALL_BINS RESULT_VARIABLE QMAKE_RESULT OUTPUT_VARIABLE QT_INSTALL_BINS OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (QMAKE_RESULT)
        message(FATAL_ERROR "Failed to use qmake to query QT_INSTALL_BINS")
    endif (QMAKE_RESULT)

    set(DOCS_QTDOCSDIR ${QT_INSTALL_DOCS} CACHE PATH "Path to a directory containing the Qt Documentation")

    # umbrella target to build qtdocs and local htmldocs
    add_custom_target(docs)

    # Project related configuration options
    set(DOXYGEN_REPEAT_BRIEF NO)
    set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_SOURCE_DIR})
    set(DOXYGEN_QT_AUTOBRIEF YES)
    set(DOXYGEN_TAB_SIZE 8)
    set(DOXYGEN_ALIASES
        "gr_tag{1}=<tt>{%&nbsp\;\\1&nbsp\;%}</tt>"
        "gr_var{1}=<tt>{{&nbsp\;\\1&nbsp\;}}</tt>"
    )
    # Build related configuration options
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_INTERNAL_DOCS YES)
    set(DOXYGEN_SORT_BRIEF_DOCS YES)
    set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)
    set(DOXYGEN_MAX_INITIALIZER_LINES 0)
    set(DOXYGEN_SHOW_USED_FILES NO)
    # Configuration options related to warning and progress messages
    if (BUILD_DOCS_QUIET)
        set(DOXYGEN_QUIET YES)
        set(DOXYGEN_WARNINGS NO)
        set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
        set(DOXYGEN_WARN_IF_DOC_ERROR NO)
    else(BUILD_DOCS_QUIET)
        set(DOXYGEN_QUIET NO)
        set(DOXYGEN_WARNINGS YES)
        set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
        set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    endif(BUILD_DOCS_QUIET)
    # Configuration options related to the input files
    set(DOXYGEN_FILE_PATTERN *.h *.dox)
    set(DOXYGEN_EXCLUDE_PATTERNS .* *_p*)
    set(DOXYGEN_IMAGE_PATH ${CMAKE_SOURCE_DIR}/dox)
    #  Configuration options related to source browsing
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_REFERENCED_BY_RELATION YES)
    set(DOXYGEN_REFERENCES_RELATION YES)
    # Configuration options related to the alphabetical class index
    set(DOXYGEN_ALPHABETICAL_INDEX NO)
    # Configuration options related to the HTML output
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_SOURCE_DIR}/dox/stylesheet.css)
    set(DOXYGEN_SEARCHENGINE NO)
    # Configuration options related to the preprocessor
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
    set(DOXYGEN_PREDEFINED Q_QDOC TemplateImpl:Template CUTELEE_TEMPLATES_EXPORT=)
    set(DOXYGEN_EXPAND_AS_DEFINED TemplateImpl)
    # Configuration options related to external references
    set(DOXYGEN_GENERATE_TAGFILE ${CMAKE_CURRENT_BINARY_DIR}/cutelee${PROJECT_VERSION_MAJOR}.tags)
    # Configuration options related to the dot tool
    set(DOXYGEN_COLLABORATION_GRAPH NO)
    set(DOXYGEN_DOTFILE_DIRS ${CMAKE_SOURCE_DIR}/dox)
    set(DOXYGEN_MAX_DOT_GRAPH_DEPTH 1000)
    if (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.9.5)
        set(DOXYGEN_DOT_COMMON_ATTR fontname=FreeSans,fontsize=10)
        set(DOXYGEN_DOT_EDGE_ATTR labelfontname=FreeSans,labelfontsize=10)
    else (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.9.5)
        set(DOXYGEN_DOT_FONTNAME FreeSans)
        set(DOXYGEN_DOT_FONTSIZE 10)
    endif (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.9.5)


    # Available from http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/libstdc++.tag
    set(DOCS_STLTAGFILE "${CMAKE_CURRENT_SOURCE_DIR}/scripts/libstdc++.tag" CACHE FILEPATH "Path to the doxygen tag file of the libstdc++")
    if(EXISTS DOCS_STLTAGFILE)
        list(APPEND ONLINETAGS "${DOCS_STLTAGFILE}=http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/")
    else(EXISTS DOCS_STLTAGFILE)
        set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    endif(EXISTS DOCS_STLTAGFILE)

    set(QT_ONLINE_DOCS_URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/")
    set(QT_DOCS_COMPONENTS "qtcore" "qtgui" "qtqml")

    foreach(_QTDOCCOMP ${QT_DOCS_COMPONENTS})
        find_file(_QTDOCCOMPTAGS "${_QTDOCCOMP}.tags" PATHS "${DOCS_QTDOCSDIR}" PATH_SUFFIXES "${_QTDOCCOMP}" NO_DEFAULT_PATH)
        if (_QTDOCCOMPTAGS)
            list(APPEND ONLINETAGS "${_QTDOCCOMPTAGS}=${QT_ONLINE_DOCS_URL}")
            if (IS_DIRECTORY "${DOCS_QTDOCSDIR}/${_QTDOCCOMP}")
                list(APPEND LOCALHTMLTAGS "${_QTDOCCOMPTAGS}=${DOCS_QTDOCSDIR}/${_QTDOCCOMP}/")
            endif (IS_DIRECTORY "${DOCS_QTDOCSDIR}/${_QTDOCCOMP}")
            if (EXISTS "${DOCS_QTDOCSDIR}/${_QTDOCCOMP}.qch")
                list(APPEND QCHTAGS "${_QTDOCCOMPTAGS}=qthelp://org.qt-project.${_QTDOCCOMP}.${QT_VERSION_MAJOR}${QT_VERSION_MINOR}${QT_VERSION_PATCH}/${_QTDOCCOMP}/")
            endif (EXISTS "${DOCS_QTDOCSDIR}/${_QTDOCCOMP}.qch")
        endif (_QTDOCCOMPTAGS)
        unset(_QTDOCCOMPTAGS CACHE)
    endforeach(_QTDOCCOMP ${QT_DOCS_COMPONENTS})

    set(DOXYGEN_HTML_TIMESTAMP "YES")
    set(DOXYGEN_TAGFILES ${ONLINETAGS})
    set(DOXYGEN_HTML_OUTPUT "webdocs")
    doxygen_add_docs(webdocs
        ${CMAKE_SOURCE_DIR}/Mainpage.dox ${CMAKE_SOURCE_DIR}/dox ${CMAKE_SOURCE_DIR}/templates ${CMAKE_SOURCE_DIR}/textdocument
        COMMENT "Generate HTML documentation for online usage"
    )
    message(STATUS "Adding build target webdocs")

    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_HTML_TIMESTAMP "NO")
    set(DOXYGEN_TAGFILES ${LOCALHTMLTAGS})
    set(DOXYGEN_HTML_OUTPUT "htmldocs")
    doxygen_add_docs(htmldocs
        ${CMAKE_SOURCE_DIR}/Mainpage.dox ${CMAKE_SOURCE_DIR}/dox ${CMAKE_SOURCE_DIR}/templates ${CMAKE_SOURCE_DIR}/textdocument
        COMMENT "Generate HTML documentation for local usage"
    )
    add_dependencies(docs htmldocs)
    message(STATUS "Adding build target htmldocs")

    find_program(DOXYGEN_QHG_LOCATION NAMES qhelpgenerator-qt${QT_VERSION_MAJOR} qhelpgenerator${QT_VERSION_MAJOR} qhelpgenerator HINTS ${QT_INSTALL_LIBEXECS} ${QT_INSTALL_BINS})
    if(DOXYGEN_QHG_LOCATION)
        set(DOXYGEN_TAGFILES ${QCHTAGS})
        set(DOXYGEN_HTML_OUTPUT "qtdocs")
        set(DOXYGEN_GENERATE_QHP "YES")
        set(DOXYGEN_QHP_NAMESPACE "org.cutelee.${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
        set(DOXYGEN_QHP_VIRTUAL_FOLDER "cutelee-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
        set(DOXYGEN_QCH_FILE "${CMAKE_BINARY_DIR}/cutelee${PROJECT_VERSION_MAJOR}.qch")

        doxygen_add_docs(qtdocs
            ${CMAKE_SOURCE_DIR}/Mainpage.dox ${CMAKE_SOURCE_DIR}/dox ${CMAKE_SOURCE_DIR}/templates ${CMAKE_SOURCE_DIR}/textdocument
            COMMENT "Generate Qt QCH documentation for local usage"
        )
        add_dependencies(docs qtdocs)
        message(STATUS "Adding build target qtdocs")
    endif(DOXYGEN_QHG_LOCATION)
endif()

find_package(LCov)

if(LCOV_FOUND)
  get_property(UNITTESTS GLOBAL PROPERTY TEST_COVERAGE)

  set(coverageDir ${CMAKE_BINARY_DIR})
  set(coverage_ARGS)

  foreach(_testpath ${UNITTESTS})
    list(APPEND coverage_ARGS COMMAND ${LCOV_EXECUTABLE} -z -d ${coverageDir})

    get_filename_component(_test ${_testpath} NAME)

    list(APPEND coverage_ARGS COMMAND ${_testpath})
    list(APPEND coverage_ARGS COMMAND ${LCOV_EXECUTABLE} -c -t ${_test} -d ${coverageDir} -o ${CMAKE_BINARY_DIR}/test_${_test}.info)
    set(infoFiles ${infoFiles} -a ${CMAKE_BINARY_DIR}/test_${_test}.info)
  endforeach()

  list(APPEND coverage_ARGS COMMAND ${LCOV_EXECUTABLE} ${infoFiles} -o test_all.info)

  # For an unknown reason lcov does not work with absolute paths such as ${CMAKE_BINARY_DIR}
  file(RELATIVE_PATH relBinaryDir / ${CMAKE_BINARY_DIR})

  find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
  find_package(Qt${QT_VERSION_MAJOR} 5.6.0 COMPONENTS Core REQUIRED)

  get_property(qmake_loc TARGET Qt${QT_VERSION_MAJOR}::qmake PROPERTY LOCATION)
  execute_process(COMMAND "${qmake_loc}" -query QT_INSTALL_HEADERS
    RESULT_VARIABLE return_code
    OUTPUT_VARIABLE output
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
  if(return_code)
    message(FATAL_ERROR "Problem")
  endif()
  file(TO_CMAKE_PATH "${output}" output)
  file(RELATIVE_PATH relQtDir / ${output})
  set(Qt4_exclude -r test_all.info *${relQtDir}*)

  list(APPEND coverage_ARGS COMMAND ${LCOV_EXECUTABLE} -r test_all.info *${relBinaryDir}*
                                                                -r test_all.info *pluginpointer*
                                                                -r test_all.info *usr/include*
                                                                ${Qt4_exclude}
                                                                -o test_all.info
  )

  list(APPEND coverage_ARGS COMMAND ${CMAKE_COMMAND} -E remove_directory coverage)
  list(APPEND coverage_ARGS COMMAND ${GENHTML_EXECUTABLE} -o coverage/ -s --demangle-cpp -f test_all.info --prefix ${CMAKE_SOURCE_DIR})

  add_custom_target(coverage
    ${coverage_ARGS}
  )
endif()

find_package(Cccc)
if (CCCC_FOUND)
  macro(append_target_sources target)
    if (TARGET ${target})
      get_target_property(_tar_files ${target} SOURCES)
      foreach(f ${_tar_files})
        if(IS_ABSOLUTE f)
          list(APPEND target_files ${f})
        else()
          list(APPEND target_files ${${target}_SOURCE_DIR}/${f})
        endif()
      endforeach()
    endif()
  endmacro()


  append_target_sources(Cutelee_Templates)
  append_target_sources(Cutelee_TextDocument)
  append_target_sources(cutelee_defaultfilters)
  append_target_sources(cutelee_defaulttags)
  append_target_sources(cutelee_loadertags)
  append_target_sources(cutelee_i18ntags)

  set(cccc_ARGS ${CCCC_EXECUTABLE} ${target_files} --outdir=${CMAKE_BINARY_DIR}/cccc --html_outfile=${CMAKE_BINARY_DIR}/cccc/index.html)

  add_custom_target(cccc
    ${cccc_ARGS}
  )
endif()

include(CPackConfig)
