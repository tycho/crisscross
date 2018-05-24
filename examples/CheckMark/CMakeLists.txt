add_executable(CheckMark
	adler32.cpp
	crc32.cpp
	header.cpp
	main.cpp
	random.cpp)

target_link_libraries(CheckMark CrissCross)
