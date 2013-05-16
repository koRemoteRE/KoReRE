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

#____assimp______
   add_library( assimp STATIC IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.a )

  add_library( dyassimp SHARED IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.3.0.0.dylib )

#____glew______
   add_library( glew STATIC IMPORTED )
   set_target_properties( glew PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libGLEW.a )

   add_library( dyglew SHARED IMPORTED )
   set_target_properties( glew PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libGLEW.1.9.0.dylib )

#____glfw______
   add_library( glfw STATIC IMPORTED )
   set_target_properties( glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libglfw.a )

   add_library( dyglfw SHARED IMPORTED )
   set_target_properties( glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libglfw.dylib )

#____libav (including all components)______

 add_library( libavformat STATIC IMPORTED )
   set_target_properties( libavformat PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavformat.a )

  add_library( dylibavformat SHARED IMPORTED )
   set_target_properties( libavformat PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavformat.dylib )

add_library( libavfilter STATIC IMPORTED )
   set_target_properties( libavfilter PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavfilter.a )

  add_library( dylibavfilter SHARED IMPORTED )
   set_target_properties( libavfilter PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavfilter.dylib )

add_library( libavdevice STATIC IMPORTED )
   set_target_properties( libavdevice PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavdevice.a )

  add_library( dylibavdevice SHARED IMPORTED )
   set_target_properties( libavdevice PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavdevice.dylib )

add_library( libavcodec STATIC IMPORTED )
   set_target_properties( libavcodec PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavcodec.a )

  add_library( dylibavcodec SHARED IMPORTED )
   set_target_properties( libavcodec PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavcodec.dylib )

add_library( libavutil STATIC IMPORTED )
   set_target_properties( libavutil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavutil.a )

add_library( dylibavutil SHARED IMPORTED )
   set_target_properties( libavutil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libavutil.dylib )

add_library( libswscale STATIC IMPORTED )
   set_target_properties( libswscale PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libswscale.a )

add_library( dylibswscale SHARED IMPORTED )
   set_target_properties( libswscale PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libswscale.dylib )


#__________________________________________
   
   target_link_libraries(ReReClient assimp)
   target_link_libraries(ReReClient glfw)
   target_link_libraries(ReReClient glew)
   target_link_libraries(ReReClient libavformat)
   target_link_libraries(ReReClient libavfilter)
   target_link_libraries(ReReClient libavdevice)
   target_link_libraries(ReReClient libavcodec)
   target_link_libraries(ReReClient libavutil)
   target_link_libraries(ReReClient libswscale)

#__________________________________________
   
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
  endif(MSVC)
   
endif()

#aux_source_directory(./ReRe-Client SRC_LIST)
#aux_header_directory(./ReRe-Client HEADER_LIST)

#add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})


#${GLFW_LIBRARY} 