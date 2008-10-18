add_executable(TestSuite avltree.cpp bubblesort.cpp combsort.cpp compare.cpp darray.cpp datatypes.cpp dstack.cpp hashtable.cpp header.cpp heapsort.cpp insertionsort.cpp llist.cpp main.cpp md2.cpp md4.cpp md5.cpp quicksort.cpp rbtree.cpp sha1.cpp sha256.cpp sha512.cpp shellsort.cpp splaytree.cpp stopwatch.cpp testutils.cpp tiger.cpp)
add_precompiled_header(TestSuite ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (TestSuite CrissCross)
