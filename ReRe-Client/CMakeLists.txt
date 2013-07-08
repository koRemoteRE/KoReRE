project(ReReClient)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#find_package(GLFW REQUIRED)
#include_directories(${GLFW_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/ext)
INCLUDE_DIRECTORIES ($ENV{OPENCV_DIR}/include)
INCLUDE_DIRECTORIES ($ENV{BOOST_ROOT})
file(GLOB_RECURSE SOURCES ./ReRe-Client/*.cpp)
file(GLOB_RECURSE HEADER ./ReRe-Client/*.h*)
file(GLOB_RECURSE VERTEXSHADER ./Shader/*.vert)
file(GLOB_RECURSE FRAGMENTSHADER ./Shader/*.frag)
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
	${CMAKE_SOURCE_DIR}/MacLibs/libglfw.a
	${CMAKE_SOURCE_DIR}/MacLibs/libGLEW.a
	${CMAKE_SOURCE_DIR}/MacLibs/libboost_system-mt.a
	${CMAKE_SOURCE_DIR}/MacLibs/libboost_thread-mt.a
	${CMAKE_SOURCE_DIR}/MacLibs/libboost_serialization-mt.a
	${CMAKE_SOURCE_DIR}/MacLibs/libopencv_core.2.4.dylib
	${CMAKE_SOURCE_DIR}/MacLibs/libopencv_features2d.dylib
	${CMAKE_SOURCE_DIR}/MacLibs/libopencv_flann.dylib
	${CMAKE_SOURCE_DIR}/MacLibs/libopencv_highgui.dylib
	${CMAKE_SOURCE_DIR}/MacLibs/libopencv_imgproc.2.4.dylib)

   add_library( assimp STATIC IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.a )

   add_library( devil STATIC IMPORTED )
   set_target_properties( devil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libIL.a )

   add_library( dyassimp SHARED IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.3.0.0.dylib )

   add_library( dydevil SHARED IMPORTED )
   set_target_properties( devil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libIL.1.dylib )

   		
   target_link_libraries(ReReClient assimp)
   target_link_libraries(ReReClient devil)
   
    set(EXT_LIBS ${CMAKE_SOURCE_DIR}/MacLibs)
#    link_directories(${EXT_LIBS})
#   	file(GLOB_RECURSE OpenCV_LIBS /opt/local/lib/*libopencv_*.dylib)
#	file(GLOB_RECURSE Boost_LIBS /opt/local/lib/*libboost_*.a)
#    MESSAGE(STATUS ${OpenCV_LIBS} ${Boost_LIBS})
#	target_link_libraries( ReReClient ${OpenCV_LIBS} ${Boost_LIBS})
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
	 target_link_libraries(ReReClient ${VS_LIBS})
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
    target_link_libraries(ReReClient ${MINGW_LIBS})
  endif()
endif()


#aux_source_directory(./ReRe-Client SRC_LIST)
#aux_header_directory(./ReRe-Client HEADER_LIST)

#add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})


#${GLFW_LIBRARY} 