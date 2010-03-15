add_executable(TestSuite avltree.cpp backtrace.cpp compare.cpp darray.cpp datatypes.cpp dstack.cpp hashtable.cpp header.cpp llist.cpp ltree.cpp main.cpp md4.cpp md5.cpp rbtree.cpp sanity.cpp sha1.cpp sha256.cpp sha512.cpp sort.cpp splaytree.cpp stltree.cpp stopwatch.cpp stree.cpp testutils.cpp tiger.cpp)
add_precompiled_header(TestSuite ${CMAKE_CURRENT_SOURCE_DIR}/header.h ${CMAKE_CURRENT_SOURCE_DIR}/header.cpp)

target_link_libraries (TestSuite CrissCross)
