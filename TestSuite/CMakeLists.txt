add_executable(TestSuite
	avltree.cpp
	backtrace.cpp
	compare.cpp
	darray.cpp
	dstack.cpp
	hashtable.cpp
	header.cpp
	llist.cpp
	main.cpp
	rbtree.cpp
	sort.cpp
	splaytree.cpp
	stltree.cpp
	stopwatch.cpp
	stree.cpp
	testutils.cpp
)

target_include_directories(TestSuite
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TestSuite CrissCross)
