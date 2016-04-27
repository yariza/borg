CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( Borg )

FILE( GLOB GLEW lib/glew/*.c lib/glew/*.h )
FILE( GLOB GLSW lib/glsw/*.c lib/glsw/*.h )
FILE( GLOB LZMA lib/liblzma/*.c lib/liblzma/*.h )
FILE( GLOB OPENCTM lib/openctm/*.c lib/openctm/*.h )
FILE( GLOB PNGLITE lib/pnglite/*.c lib/pnglite/*.h )
FILE( GLOB VECTORMATH lib/vectormath/cpp/*.h )
FILE( GLOB ZLIB lib/zlib/*.c lib/zlib/*.h )
FILE( GLOB GL lib/gl/*.c lib/gl/*.h )

IF( WIN32 )
    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.windows.c
    )

    SOURCE_GROUP( "glew" FILES ${GLEW} )
    SOURCE_GROUP( "glsw" FILES ${GLSW} )
    SOURCE_GROUP( "lzma" FILES ${LZMA} )
    SOURCE_GROUP( "openctm" FILES ${OPENCTM} )
    SOURCE_GROUP( "pnglite" FILES ${PNGLITE} )
    SOURCE_GROUP( "vectormath" FILES ${VECTORMATH} )
    SOURCE_GROUP( "zlib" FILES ${ZLIB} )
    SOURCE_GROUP( "pez" FILES ${PEZ} )
    SOURCE_GROUP( "gl" FILES ${GL} )
    SOURCE_GROUP( "shaders" FILES ${SHADERS} )

    ADD_DEFINITIONS( -DGLEW_STATIC )
    ADD_DEFINITIONS( -DOPENCTM_STATIC )
    ADD_DEFINITIONS( /wd4996 )
    SET( PLATFORM_LIBS opengl32 )
    SET( CONSOLE_SYSTEM WIN32 )

ELSEIF( APPLE )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.cocoa.m
    )

    FIND_LIBRARY( COCOA_LIBRARY Cocoa )
    FIND_LIBRARY( OPENGL_LIBRARY OpenGL )
    MESSAGE(STATUS ${COCOA_LIBRARY})

    SET( PLATFORM_LIBS
        ${COCOA_LIBRARY}
        ${OPENGL_LIBRARY} )

ELSEIF( UNIX )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.linux.c
    )

    SET( PLATFORM_LIBS X11 GL Xext m )

ENDIF()

INCLUDE_DIRECTORIES(
#    lib/glew
    lib/glsw
    lib/liblzma
    lib/openctm
    lib/pnglite
    lib/zlib
    lib/gl
    lib/libpez
)

ADD_LIBRARY( PezEcosystem
#    ${GLEW}
    ${GLSW}
    ${LZMA}
    ${OPENCTM}
    ${PNGLITE}
    ${VECTORMATH}
    ${ZLIB}
    ${GL}
    ${PEZ}
)

FILE( GLOB BORG
    src/test.c
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

ADD_EXECUTABLE( borg ${CONSOLE_SYSTEM} ${BORG} )

IF ( APPLE )
    set_property (TARGET borg APPEND_STRING PROPERTY
                  COMPILE_FLAGS "-fobjc-arc")
ENDIF()

TARGET_LINK_LIBRARIES( borg PezEcosystem ${PLATFORM_LIBS} )

# ANSI C SAMPLES:

INCLUDE_DIRECTORIES( lib/vectormath/c )
ADD_EXECUTABLE( PezIntro1 ${CONSOLE_SYSTEM} demo/PezIntro1.Clear.c )
TARGET_LINK_LIBRARIES( PezIntro1 PezEcosystem ${PLATFORM_LIBS} )
ADD_EXECUTABLE( PezIntro2 ${CONSOLE_SYSTEM} demo/PezIntro2.Triangle.c demo/Simple.glsl )
TARGET_LINK_LIBRARIES( PezIntro2 PezEcosystem ${PLATFORM_LIBS} )
ADD_EXECUTABLE( PezIntro3 ${CONSOLE_SYSTEM} demo/PezIntro3.Texture.c demo/Simple.glsl demo/Gandhi.png )
TARGET_LINK_LIBRARIES( PezIntro3 PezEcosystem ${PLATFORM_LIBS} )
ADD_EXECUTABLE( Fluid ${CONSOLE_SYSTEM} demo/Fluid.c demo/CreateObstacles.c demo/CreateProgram.c demo/CreateQuad.c demo/CreateSlab.c demo/SlabOps.c demo/Fluid.glsl )
TARGET_LINK_LIBRARIES( Fluid PezEcosystem ${PLATFORM_LIBS} )

# C++ SAMPLES:

INCLUDE_DIRECTORIES( BEFORE lib/vectormath/cpp )
ADD_EXECUTABLE( PezIntro4 ${CONSOLE_SYSTEM} demo/PezIntro4.Mesh.cpp demo/PixelLighting.glsl demo/HeadlessGiant.ctm )
TARGET_LINK_LIBRARIES( PezIntro4 PezEcosystem ${PLATFORM_LIBS} )
