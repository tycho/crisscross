add_executable(CheckMark adler32.cpp crc32.cpp header.cpp main.cpp random.cpp)
add_precompiled_header(CheckMark ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (CheckMark CrissCross)
