project(ReReClient)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#find_package(GLFW REQUIRED)
#include_directories(${GLFW_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/ext)
file(GLOB_RECURSE SOURCES ./ReRe-Client/*.cpp)
file(GLOB_RECURSE HEADER ./ReRe-Client/*.h)
file(GLOB_RECURSE VERTEXSHADER ./Shader/*.vert)
file(GLOB_RECURSE FRAGMENTSHADER ./Shader/*.frag)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER} ${VERTEXSHADER} ${FRAGMENTSHADER})

IF(APPLE)

 #INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/ext)
  
#include_directories(${GLFW_INCLUDE_DIR} /usr/local/include/ )

   find_library(COCOA_LIBRARY Cocoa)
   find_library(GLUT_LIBRARY GLUT )
   find_library(OpenGL_LIBRARY OpenGL )
   find_library(IOKIT NAMES IOKit)
   MARK_AS_ADVANCED (COCOA_LIBRARY
                     GLUT_LIBRARY
                     OpenGL_LIBRARY)
   SET(EXTRA_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT})

   add_library( assimp STATIC IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.a )

   add_library( glew STATIC IMPORTED )
   set_target_properties( glew PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libGLEW.a )

   add_library( glfw STATIC IMPORTED )
   set_target_properties( glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libglfw.a )

   add_library( dyassimp SHARED IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.3.0.0.dylib )

   add_library( dyglew SHARED IMPORTED )
   set_target_properties( glew PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libGLEW.1.9.0.dylib )

   add_library( dyglfw SHARED IMPORTED )
   set_target_properties( glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libglfw.dylib )

   
   target_link_libraries(ReReClient assimp)
   target_link_libraries(ReReClient glfw)
   target_link_libraries(ReReClient glew)

   target_link_libraries(ReReClient ${EXTRA_LIBS})
else()
  if(MSVC)
	set(EXT_LIBS ${CMAKE_SOURCE_DIR}/VSLibs)
    link_directories(${EXT_LIBS})
	  set(VS_LIBS
	    OpenGL32.lib
	    ${EXT_LIBS}/GLFW.lib
	    ${EXT_LIBS}/glew32.lib
	    ${EXT_LIBS}/assimp.lib
	  )
	 target_link_libraries(ReReClient ${VS_LIBS})
  else()
    link_directories(${EXT_LIBS})
    link_directories(${CMAKE_LIBRARY_PATH})
    include_directories(${CMAKE_LIBRARY_PATH})
    set(
    assimp
    glfw
    opengl32
    )
  endif()
endif()

#aux_source_directory(./ReRe-Client SRC_LIST)
#aux_header_directory(./ReRe-Client HEADER_LIST)

#add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})


#${GLFW_LIBRARY} 