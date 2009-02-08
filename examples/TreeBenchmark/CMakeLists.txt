add_executable(TreeBenchmark header.cpp main.cpp)
add_precompiled_header(TreeBenchmark ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (TreeBenchmark CrissCross)
