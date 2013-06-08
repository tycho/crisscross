if(WIN32 AND MSVC)
add_custom_command(
	OUTPUT ${CrissCross_BINARY_DIR}/source/crisscross/build_number.h
	COMMAND "${CrissCross_SOURCE_DIR}/tools/gen_buildnum_h_helper.cmd" "${CrissCross_SOURCE_DIR}"
)
else(WIN32 AND MSVC)
add_custom_command(
	OUTPUT ${CrissCross_BINARY_DIR}/source/crisscross/build_number.h
	COMMAND "${CrissCross_SOURCE_DIR}/tools/gen_buildnum_h.pl" "${CrissCross_BINARY_DIR}/source/crisscross/build_number.h"
)
endif(WIN32 AND MSVC)

add_custom_target(BuildNumber DEPENDS ${CrissCross_BINARY_DIR}/source/crisscross/build_number.h)
add_library(CrissCross compare.cpp console.cpp core_io_reader.cpp core_io_writer.cpp core_network.cpp core_socket.cpp debug.cpp error.cpp filesystem.cpp filereader.cpp filewriter.cpp md4.cpp md5.cpp mutex.cpp sha1.cpp sha256.cpp sha512.cpp stopwatch.cpp string_utils.cpp system.cpp tcpsocket.cpp tiger.cpp udpsocket.cpp universal_include.cpp version.cpp zombiehunter.cpp)
add_precompiled_header(CrissCross ${CMAKE_CURRENT_SOURCE_DIR}/crisscross/universal_include.h ${CMAKE_CURRENT_SOURCE_DIR}/universal_include.cpp)
add_dependencies(CrissCross BuildNumber)

install(TARGETS CrissCross ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/crisscross/ DESTINATION include/crisscross)

if(NOT WIN32)
	target_link_libraries(CrissCross pthread)
ENDIF(NOT WIN32)
