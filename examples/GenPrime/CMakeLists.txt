add_executable(GenPrime header.cpp main.cpp)
add_precompiled_header(GenPrime ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (GenPrime CrissCross)
