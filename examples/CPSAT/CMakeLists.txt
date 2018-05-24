add_executable(CPSAT
	header.cpp
	main.cpp
	rijndael-alg-fst.cpp
	rijndael.cpp)

target_link_libraries(CPSAT CrissCross)
