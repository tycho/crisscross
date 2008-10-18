add_executable(CPSAT header.cpp main.cpp rijndael-alg-fst.cpp rijndael.cpp)
add_precompiled_header(CPSAT ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (CPSAT CrissCross)
