### === IDLIB === ###
file(GLOB IDLIB_INCLUDES *.h)
file(GLOB IDLIB_SOURCES *.cpp)
source_group("" FILES ${IDLIB_INCLUDES})
source_group("" FILES ${IDLIB_SOURCES})

file(GLOB IDLIB_BV_INCLUDES bv/*.h)
file(GLOB IDLIB_BV_SOURCES bv/*.cpp)
source_group("bv" FILES ${IDLIB_BV_INCLUDES})
source_group("bv" FILES ${IDLIB_BV_SOURCES})

file(GLOB IDLIB_CONTEINER_INCLUDES containers/*.h)
file(GLOB IDLIB_CONTEINER_SOURCES containers/*.cpp)
source_group("containers" FILES ${IDLIB_CONTEINER_INCLUDES})
source_group("containers" FILES ${IDLIB_CONTEINER_SOURCES})

file(GLOB IDLIB_GEOMETRY_INCLUDES geometry/*.h)
file(GLOB IDLIB_GEOMETRY_SOURCES geometry/*.cpp)
source_group("geometry" FILES ${IDLIB_GEOMETRY_INCLUDES})
source_group("geometry" FILES ${IDLIB_GEOMETRY_SOURCES})

file(GLOB IDLIB_HASH_INCLUDES hashing/*.h)
file(GLOB IDLIB_HASH_SOURCES hashing/*.cpp)
source_group("hashing" FILES ${IDLIB_HASH_INCLUDES})
source_group("hashing" FILES ${IDLIB_HASH_SOURCES})

file(GLOB IDLIB_MATH_INCLUDES math/*.h)
file(GLOB IDLIB_MATH_SOURCES math/*.cpp)
source_group("hashing" FILES ${IDLIB_MATH_INCLUDES})
source_group("hashing" FILES ${IDLIB_MATH_SOURCES})

set(src_idlib
	${IDLIB_SOURCES}
	${IDLIB_BV_SOURCES}
	${IDLIB_CONTEINER_SOURCES}
	${IDLIB_GEOMETRY_SOURCES}
	${IDLIB_HASH_SOURCES}
	${IDLIB_MATH_SOURCES}
)

set(inc_idlib
	${IDLIB_INCLUDES}
	${IDLIB_BV_INCLUDES}
	${IDLIB_CONTEINER_INCLUDES}
	${IDLIB_GEOMETRY_INCLUDES}
	${IDLIB_HASH_INCLUDES}
	${IDLIB_MATH_INCLUDES}
)

include_directories(${ZLIB_INCLUDE_DIRS})

# compiler specific flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-pipe)
	add_compile_options(-Wall)

	if(NOT CMAKE_CROSSCOMPILING AND ONATIVE)
		add_compile_options(-march=native)
	elseif(NOT APPLE AND cpu STREQUAL "x86")
		add_compile_options(-march=pentium3)
	endif()

	set(CMAKE_C_FLAGS_DEBUG "-g -D_DEBUG -O1")
	set(CMAKE_C_FLAGS_DEBUGALL "-g -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_PROFILE "-g -ggdb -D_DEBUG -O1 -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math -fno-unsafe-math-optimizations -fomit-frame-pointer")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O3 -ffast-math -fno-unsafe-math-optimizations -fomit-frame-pointer")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -ffast-math -fno-unsafe-math-optimizations -fomit-frame-pointer")

	set(CMAKE_CXX_FLAGS_DEBUGALL ${CMAKE_C_FLAGS_DEBUGALL})
	set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})

	add_compile_options(-fno-strict-aliasing)

	CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" cxx_has_fvisibility)
	if(NOT cxx_has_fvisibility)
		message(FATAL_ERROR "Compiler does not support -fvisibility")
	endif()
	add_compile_options(-fvisibility=hidden)

	# TODO fix these warnings
	add_compile_options(-Wno-sign-compare)
	add_compile_options(-Wno-switch)
	add_compile_options(-Wno-format-security)

	CHECK_CXX_COMPILER_FLAG("-Woverloaded-virtual" cxx_has_Woverload_virtual)
	if(cxx_has_Woverload_virtual)
		add_compile_options(-Woverloaded-virtual)
	endif()

	if(APPLE)
		add_definitions(-DMACOS_X=1)

		if(cpu STREQUAL "x86_64")
			add_compile_options(-arch x86_64 -mmacosx-version-min=10.6)
			set(ldflags "${ldflags} -arch x86_64 -mmacosx-version-min=10.6")
		elseif(cpu STREQUAL "x86")
			CHECK_CXX_COMPILER_FLAG("-arch i386" cxx_has_arch_i386)
			if(cxx_has_arch_i386)
				add_compile_options(-arch i386)
				set(ldflags "${ldflags} -arch i386")
			endif()

			add_compile_options(-mmacosx-version-min=10.4)
			set(ldflags "${ldflags} -mmacosx-version-min=10.4")
		elseif(cpu STREQUAL "ppc")
			CHECK_CXX_COMPILER_FLAG("-arch ppc" cxx_has_arch_ppc)
			if(cxx_has_arch_ppc)
				add_compile_options(-arch ppc)
				set(ldflags "${ldflags} -arch ppc")
			endif()

			add_compile_options(-mmacosx-version-min=10.4)
			set(ldflags "${ldflags} -mmacosx-version-min=10.4")
		else()
			message(FATAL_ERROR "Unsupported CPU architecture for OSX")
		endif()

		set(sys_libs ${sys_libs} "-framework Carbon -framework Cocoa -framework IOKit")
	elseif(WIN32)
		set(ldflags "${ldflags} -static-libgcc -static-libstdc++")
	elseif(os STREQUAL "linux")
		set(sys_libs ${sys_libs} dl)
	endif()
elseif(MSVC)
	add_compile_options(/W4)
	add_compile_options(/wd4100) # unreferenced formal parameter
	add_compile_options(/wd4127) # conditional expression is constant
	add_compile_options(/wd4244) # possible loss of data
	add_compile_options(/wd4245) # signed/unsigned mismatch
	add_compile_options(/wd4267) # possible loss of data
	add_compile_options(/wd4714) # 'function' marked as __forceinline not inlined
	add_compile_options(/wd4996) # 'function': was declared deprecated
	add_compile_options(/wd4068) # unknown pragma
	add_definitions(-D_ALLOW_KEYWORD_MACROS) # because of the "#define private public" and "#define protected public" in TypeInfo.cpp
	set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG /Od /Zi /MDd")
	set(CMAKE_C_FLAGS_RELEASE "/Ox /Oy /MD")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ox /Oy /Zi /MD")
	set(CMAKE_C_FLAGS_MINSIZEREL "/Ox /Oy /Os /MD")
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
add_library(idlib STATIC ${src_idlib} ${inc_idlib})
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang" AND NOT MINGW)
	set_target_properties(idlib PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
