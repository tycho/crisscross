if(WIN32 AND NOT CYGWIN)
add_custom_command(
	OUTPUT ${CrissCross_BINARY_DIR}/source/crisscross/build_number.h
	COMMAND ${CrissCross_SOURCE_DIR}/tools/GenerateBuildNumber.exe "${CrissCross_SOURCE_DIR}/source/crisscross" "${CrissCross_BINARY_DIR}/source/crisscross"
)
else(WIN32 AND NOT CYGWIN)
add_custom_command(
	OUTPUT ${CrissCross_BINARY_DIR}/source/crisscross/build_number.h
	COMMAND ${CrissCross_SOURCE_DIR}/tools/GenerateBuildNumber.sh "${CrissCross_SOURCE_DIR}/source/crisscross" "${CrissCross_BINARY_DIR}/source/crisscross"
)
endif(WIN32 AND NOT CYGWIN)

add_custom_target(BuildNumber DEPENDS ${CrissCross_BINARY_DIR}/source/crisscross/build_number.h)
add_library(CrissCross console.cpp core_io_reader.cpp core_io_writer.cpp core_network.cpp core_socket.cpp cpuid.cpp debug.cpp error.cpp filereader.cpp filewriter.cpp md2.cpp md4.cpp md5.cpp mutex.cpp sha1.cpp sha256.cpp sha512.cpp stopwatch.cpp system.cpp tcpsocket.cpp tiger.cpp udpsocket.cpp universal_include.cpp version.cpp)
add_precompiled_header(CrissCross ${CMAKE_CURRENT_SOURCE_DIR}/crisscross/universal_include.h ${CMAKE_CURRENT_SOURCE_DIR}/universal_include.cpp)
add_dependencies(CrissCross BuildNumber)

if(NOT WIN32)
	target_link_libraries(CrissCross pthread)
ENDIF(NOT WIN32)
