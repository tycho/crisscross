find_package(Threads REQUIRED)
if(USE_OPENMP)
	find_package(OpenMP)
endif()

if(WIN32 AND MSVC)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_number.h
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../tools/gen_buildnum_h_helper.cmd" "${CMAKE_CURRENT_BINARY_DIR}"
)
else(WIN32 AND MSVC)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_number.h
	COMMAND perl "${CMAKE_CURRENT_SOURCE_DIR}/../tools/gen_buildnum_h.pl" "${CMAKE_CURRENT_BINARY_DIR}/build_number.h"
)
endif(WIN32 AND MSVC)

add_custom_target(BuildNumber
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build_number.h)

add_library(CrissCross STATIC
	compare.cpp
	console.cpp
	core_io_reader.cpp
	core_io_writer.cpp
	debug.cpp
	error.cpp
	filesystem.cpp
	filereader.cpp
	filewriter.cpp
	mutex.cpp
	stopwatch.cpp
	string_utils.cpp
	system.cpp
	universal_include.cpp
	version.cpp
	zombiehunter.cpp
)

if(OPENMP_CXX_FOUND)
	target_compile_options(CrissCross PUBLIC
		${OpenMP_CXX_FLAGS})
	target_link_libraries(CrissCross PUBLIC
		${OpenMP_CXX_FLAGS})
endif()

set_target_properties(CrissCross PROPERTIES CXX_STANDARD 11)

target_include_directories(CrissCross PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(CrissCross PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
)

add_dependencies(CrissCross BuildNumber)

install(TARGETS CrissCross ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/crisscross/ DESTINATION include/crisscross)

target_link_libraries(CrissCross PUBLIC Threads::Threads)
