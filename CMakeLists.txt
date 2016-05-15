CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( Borg )

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
include (Common)

# Find GLFW
find_package( GLFW REQUIRED )
if ( GLFW_FOUND )
   MESSAGE("GLFW found")
   MESSAGE(STATUS ${GLFW_LIBRARY})
else ( GLFW_FOUND )
   message(ERROR " GLFW not found!")
endif( GLFW_FOUND )

find_package( GLM REQUIRED )
if ( GLM_FOUND )
    MESSAGE("GLM found")
    MESSAGE(STATUS ${GLM_INCLUDE_DIRS})
else( GLM_FOUND )
    message(ERROR " GLM not found")
endif( GLM_FOUND )

find_package( JPEG REQUIRED )
if ( JPEG_FOUND )
    MESSAGE("JPEG found")
    MESSAGE(STATUS ${JPEG_LIBRARY})
else( JPEG_FOUND )
    MESSAGE(ERROR " JPEG not found")
endif( JPEG_FOUND )

FILE( GLOB GLEW lib/glew/*.c lib/glew/*.h )
FILE( GLOB GLSW lib/glsw/*.c lib/glsw/*.h )
FILE( GLOB LZMA lib/liblzma/*.c lib/liblzma/*.h )
FILE( GLOB OPENCTM lib/openctm/*.c lib/openctm/*.h )
FILE( GLOB PNGLITE lib/pnglite/*.c lib/pnglite/*.h )
FILE( GLOB VECTORMATH lib/vectormath/cpp/*.h )
FILE( GLOB ZLIB lib/zlib/*.c lib/zlib/*.h )
FILE( GLOB GLAD lib/glad/*.c lib/glad/*.h )

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
    SOURCE_GROUP( "glad" FILES ${GLAD} )
    SOURCE_GROUP( "shaders" FILES ${SHADERS} )

    ADD_DEFINITIONS( -DGLEW_STATIC )
    ADD_DEFINITIONS( -DOPENCTM_STATIC )
    ADD_DEFINITIONS( /wd4996 )
    SET( PLATFORM_LIBS opengl32 )
    SET( CONSOLE_SYSTEM WIN32 )

ELSEIF( APPLE )

    # FIND_LIBRARY( COCOA_LIBRARY Cocoa )
    # FIND_LIBRARY( OPENGL_LIBRARY OpenGL )

    # SET( PLATFORM_LIBS
    #     ${COCOA_LIBRARY}
    #     ${OPENGL_LIBRARY} )

ELSEIF( UNIX )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.linux.c
    )

    SET( PLATFORM_LIBS X11 GL Xext m )

ENDIF()

FIND_PATH( CMATH_INCLUDE_DIR math.h )
MESSAGE(STATUS ${CMAATH_INCLUDE_DIR})

INCLUDE_DIRECTORIES(
    lib/glsw
    lib/liblzma
    lib/openctm
    lib/pnglite
    lib/zlib
    lib/glad
    lib/libpez
    ${GLM_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIR}
    ${JPEG_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

FILE( GLOB BORG
    src/GLFWViewer.cpp
    src/GLFWViewer.h
)

append_files (
    BORG "h" src/ /src/*/
)
append_files (
    BORG "cpp" src/ /src/*/
)

ADD_LIBRARY( BorgEcosystem
    ${GLSW}
    ${LZMA}
    ${OPENCTM}
    ${PNGLITE}
    ${VECTORMATH}
    ${ZLIB}
    ${GLAD}
    ${JPEG_LIBRARY}
    ${BORG}
    # ${GLFW_LIBRARY}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

ADD_EXECUTABLE( borg demo/test.cpp ${CONSOLE_SYSTEM} ${BorgEcosystem} )

TARGET_LINK_LIBRARIES( borg BorgEcosystem ${GLFW_LIBRARY} ${PLATFORM_LIBS} )

# ANSI C SAMPLES:

# INCLUDE_DIRECTORIES( lib/vectormath/c )
# ADD_EXECUTABLE( PezIntro1 ${CONSOLE_SYSTEM} demo/PezIntro1.Clear.c )
# TARGET_LINK_LIBRARIES( PezIntro1 BorgEcosystem ${PLATFORM_LIBS} )
# ADD_EXECUTABLE( PezIntro2 ${CONSOLE_SYSTEM} demo/PezIntro2.Triangle.c demo/Simple.glsl )
# TARGET_LINK_LIBRARIES( PezIntro2 BorgEcosystem ${PLATFORM_LIBS} )
# ADD_EXECUTABLE( PezIntro3 ${CONSOLE_SYSTEM} demo/PezIntro3.Texture.c demo/Simple.glsl demo/Gandhi.png )
# TARGET_LINK_LIBRARIES( PezIntro3 BorgEcosystem ${PLATFORM_LIBS} )
ADD_EXECUTABLE( Fluid ${CONSOLE_SYSTEM} demo/Fluid.cpp demo/CreateObstacles.cpp demo/CreateProgram.cpp demo/CreateQuad.cpp demo/CreateSlab.cpp demo/SlabOps.cpp demo/Fluid.glsl )
TARGET_LINK_LIBRARIES( Fluid BorgEcosystem ${GLFW_LIBRARY} ${PLATFORM_LIBS} )

# C++ SAMPLES:

INCLUDE_DIRECTORIES( BEFORE lib/vectormath/cpp )
ADD_EXECUTABLE( PezIntro4 ${CONSOLE_SYSTEM} demo/PezIntro4.Mesh.cpp demo/PixelLighting.glsl demo/HeadlessGiant.ctm )
TARGET_LINK_LIBRARIES( PezIntro4 BorgEcosystem ${GLFW_LIBRARY} ${PLATFORM_LIBS} )
