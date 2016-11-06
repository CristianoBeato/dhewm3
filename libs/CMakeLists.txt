if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


### GLEW ###
set(GLEW_SOURCE
	glew/src/glew.c
	glew/src/glewinfo.c
	glew/src/visualinfo.c
)

set(GLEW_HEADERS
	glew/include/gl/glew.h
	glew/include/gl/glxew.h
	glew/include/gl/wglew.h
)

set(INCLUDE_GLEW
	glew/include
	)

add_library( GLEW STATIC
	${GLEW_SOURCE}
	${GLEW_HEADERS}
)

include_directories(${INCLUDE_GLEW})
target_link_libraries(GLEW
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

set( LIB_INSTALL_DIR "libs")

### KTX ###
set(KTX_SOURCE
	ktx/source/checkheader.c
	ktx/source/errstr.c
	ktx/source/etcdec.cxx
	ktx/source/etcunpack.cxx
	ktx/source/hashtable.c	
	ktx/source/loader.c
	ktx/source/swap.c
	ktx/source/writer.c
)

set(KTX_HEADERS
	

	ktx/source/gl_funcptrs.h
	ktx/source/gles1_funcptrs.h
	ktx/source/gles2_funcptrs.h
	ktx/source/gles3_funcptrs.h
	ktx/source/ktxint.h
	ktx/source/uthash.h
	
	ktx/include/ktx.h
	ktx/include/KHR/khrplatform.h
)

set(INCLUDE_KTX
	ktx/include
	)

add_library( KTX STATIC
	${KTX_SOURCE}
	${KTX_HEADERS}
)


include_directories(${INCLUDE_KTX})
target_link_libraries(KTX
	${OPENGL_LIBRARY}
	${GLEW_LIBRARY}
	//${EXTRA_LIBS}
)

