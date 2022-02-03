find_package(Git QUIET)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
    WORKING_DIRECTORY ${SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE TAG
    ERROR_VARIABLE ERROR_CODE
  )

  if(ERROR_CODE STREQUAL "")
    execute_process(
	  COMMAND ${GIT_EXECUTABLE} rev-list --count ${TAG}..HEAD
	  WORKING_DIRECTORY ${SOURCE_DIR}
	  OUTPUT_STRIP_TRAILING_WHITESPACE
	  OUTPUT_VARIABLE VERSION_BUILD
    )

    execute_process(
	  COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
	  WORKING_DIRECTORY ${SOURCE_DIR}
	  OUTPUT_STRIP_TRAILING_WHITESPACE
	  OUTPUT_VARIABLE GIT_TAG
    )

    set(GIT_TAG "${TAG}-${VERSION_BUILD}-g${GIT_TAG}")
  endif()
endif()

if(NOT ERROR_CODE STREQUAL "" OR NOT Git_FOUND)
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/release_ver")
    file(READ "${CMAKE_CURRENT_LIST_DIR}/release_ver" TAG)
	string(STRIP ${TAG} TAG)
  else()
    message( FATAL_ERROR "Can't open release_ver" )
  endif()
  set(VERSION_BUILD 0)
  set(GIT_TAG ${TAG})
endif()

string(REPLACE "." ";" TAG ${TAG})
list(GET TAG 0 VERSION_MAJOR)
list(GET TAG 1 VERSION_MINOR)
list(GET TAG 2 VERSION_REVISION)

if(VERSION_MAJOR MATCHES "^v")
  STRING(REGEX REPLACE "([0-9]+)" "\\1"  VERSION_MAJOR ${VERSION_MAJOR})
  list(GET TAG 3 VERSION_BUILD)
endif()

file(MAKE_DIRECTORY ${HEADER_OUTPUT_DIR})
configure_file("${CMAKE_CURRENT_LIST_DIR}/build_number.h.in" ${HEADER_OUTPUT}.tmp)

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${HEADER_OUTPUT} ${HEADER_OUTPUT}.tmp
    RESULT_VARIABLE COMPARE_RESULT)

if (NOT COMPARE_RESULT EQUAL 0)
	file(REMOVE ${HEADER_OUTPUT})
	file(RENAME ${HEADER_OUTPUT}.tmp ${HEADER_OUTPUT})
else()
	file(REMOVE ${HEADER_OUTPUT}.tmp)
endif()