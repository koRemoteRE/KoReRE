project(ReReServer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#find_package(GLFW REQUIRED)
#include_directories(${GLFW_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/ext)
INCLUDE_DIRECTORIES ($ENV{OPENCV_DIR}/include)
INCLUDE_DIRECTORIES ($ENV{BOOST_ROOT})
file(GLOB_RECURSE SOURCES ./Server/*.cpp)
file(GLOB_RECURSE HEADER ./Server/*.h*)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER} ${VERTEXSHADER} ${FRAGMENTSHADER})

IF(APPLE)

   find_library(COCOA_LIBRARY Cocoa)
   find_library(GLUT_LIBRARY GLUT )
   find_library(OpenGL_LIBRARY OpenGL )
   find_library(IOKIT NAMES IOKit)
   MARK_AS_ADVANCED (COCOA_LIBRARY GLUT_LIBRARY OpenGL_LIBRARY)

   SET(EXTRA_LIBS 
	${COCOA_LIBRARY} 
	${GLUT_LIBRARY} 
	${OpenGL_LIBRARY} 
	${IOKIT}
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libglfw.a
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libGLEW.a
	${CMAKE_SOURCE_DIR}/MacLibs/libKoRE.a
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libboost_system-mt.a
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libboost_thread-mt.a
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libboost_serialization-mt.a
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libopencv_core.dylib
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libopencv_features2d.dylib
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libopencv_flann.dylib
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libopencv_highgui.dylib
	${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libopencv_imgproc.dylib)

   add_library( assimp STATIC IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libassimp.a )

   add_library( dyassimp SHARED IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../ReRe-Client/MacLibs/libassimp.3.0.0.dylib )

   		
   target_link_libraries(ReReServer assimp)
   
    set(EXT_LIBS ${CMAKE_SOURCE_DIR}/MacLibs)
    target_link_libraries(ReReServer ${EXTRA_LIBS})
    
else()
  if(MSVC)
	set(EXT_LIBS ${CMAKE_SOURCE_DIR}/VSLibs)
    link_directories(${EXT_LIBS})
	  set(VS_LIBS
	    OpenGL32.lib
	    ${EXT_LIBS}/GLFW.lib
	    ${EXT_LIBS}/glew32.lib
	    ${EXT_LIBS}/assimp.lib
            ${EXT_LIBS}/DevIL.lib
            ${EXT_LIBS}/ILU.lib
            ${EXT_LIBS}/ILUT.lib
	    $ENV{BOOST_ROOT}/stage/lib/libboost_thread-vc110-mt-gd-1_53.lib
	    $ENV{BOOST_ROOT}/stage/lib/libboost_system-vc110-mt-gd-1_53.lib	
            $ENV{BOOST_ROOT}/stage/lib/libboost_chrono-vc110-mt-gd-1_53.lib	
            $ENV{BOOST_ROOT}/stage/lib/libboost_date_time-vc110-mt-gd-1_53.lib
            $ENV{BOOST_ROOT}/stage/lib/libboost_regex-vc110-mt-gd-1_53.lib
	    $ENV{OPENCV_DIR}/x86/vc11/lib/opencv_core245d.lib
 	    $ENV{OPENCV_DIR}/x86/vc11/lib/opencv_imgproc245d.lib
 	    $ENV{OPENCV_DIR}/x86/vc11/lib/opencv_highgui245d.lib
	  )
	 target_link_libraries(ReReServer ${VS_LIBS})
  else()
    set(EXT_LIBS ${CMAKE_SOURCE_DIR}/MinGWLibs)
    link_directories(${EXT_LIBS})
    link_directories(${CMAKE_LIBRARY_PATH})
    include_directories(${CMAKE_LIBRARY_PATH})
    set(MINGW_LIBS
    ${EXT_LIBS}/libassimp.dll.a
    ${EXT_LIBS}/libglfw.a
    opengl32
    ${EXT_LIBS}/libglew32.dll.a
    ${EXT_LIBS}/libIL.dll.a
    )
    target_link_libraries(ReReServer ${MINGW_LIBS})
  endif()
endif()