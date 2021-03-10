cmake_minimum_required (VERSION 3.5)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project (CrissCross CXX)

option(USE_OPENMP "Build with OpenMP support" ON)

include(CheckCXXCompilerFlag)

set(CC_COMMON_FLAGS
	-fno-strict-aliasing
	-fvisibility=hidden
	-rdynamic
	-Wall
	-Wundef
	-Wno-long-long
	-pedantic)

set(CC_CXX_FLAGS
	-std=gnu++17
	-Wno-non-virtual-dtor
	-Wno-reorder
	-fno-exceptions
	-fno-rtti
	-fvisibility-inlines-hidden)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU"
OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	foreach(FLAG ${CC_COMMON_FLAGS} ${CC_CXX_FLAGS})
		string(MAKE_C_IDENTIFIER ${FLAG} FLAG_ID)
		check_cxx_compiler_flag(${FLAG} ${FLAG_ID}_TEST)
		if(${FLAG_ID}_TEST)
			add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
		endif()
	endforeach()
endif()

# settings for Microsoft Visual C
IF(MSVC)
   IF(NOT WITH_SHARED_LIBS)
     IF(CMAKE_GENERATOR MATCHES "Visual Studio.*")
       SET(CMAKE_C_FLAGS "/nologo /W3 /Gy /G6 /GS- /Zi")
       SET(CMAKE_C_FLAGS_DEBUG "/MTd /Zi /Od")
       SET(CMAKE_C_FLAGS_RELEASE "/MT /O2 /Ot")
       SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2 /Os")
       SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Od")
     ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio.*")
   ENDIF(NOT WITH_SHARED_LIBS)
ENDIF(MSVC)

add_subdirectory (source)
add_subdirectory (TestSuite)
add_subdirectory (examples)

IF(NOT MSVC)
	add_custom_target (check COMMAND $<TARGET_FILE:TestSuite> DEPENDS TestSuite)
ENDIF(NOT MSVC)
