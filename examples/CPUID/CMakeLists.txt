add_executable(CPUID header.cpp main.cpp)
add_precompiled_header(CPUID ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (CPUID CrissCross)
