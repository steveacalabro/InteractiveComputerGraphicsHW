cmake_minimum_required(VERSION 2.8.1)
project(bh_final_project)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(EXCUTABLE_NAME modeling_hierarchy) 
set(GLUT_DIR $ENV{GLUT_DIR})
message(${GLUT_DIR})
set(GLEW_DIR $ENV{GLEW_DIR})
message(${GLEW_DIR})
#########################################################
# FIND GLUT
#########################################################
if (WIN32)
    FIND_PATH(GLUT_INCLUDE_DIR NAMES GL/glut.h
    PATHS ${GLUT_DIR}/include)
  FIND_LIBRARY(GLUT_LIBRARIES NAMES freeglut
    PATHS     ${GLUT_DIR}/lib/x64	)
	#message(${GLUT_LIBRARIES})
	if (GLUT_INCLUDE_DIR AND GLUT_LIBRARIES)
		set(GLUT_FOUND TRUE CACHE BOOL "GLUT found?") 
	endif(GLUT_INCLUDE_DIR AND GLUT_LIBRARIES)
endif(WIN32)

if(NOT GLUT_FOUND)
	#message(calling kitware find)
	# Previous find failed, so let's try kitware find. Cleanup variables first.
	set(GLUT_INCLUDE_DIR)
	set(GLUT_LIBRARIES)
	set(GLUT_FOUND)
	find_package(GLUT)
endif(NOT GLUT_FOUND)

if (GLUT_FOUND)
	set(GLUT_INCLUDE_DIRS ${GLUT_INCLUDE_DIR})
	set(GLUT_LIBRARY_DIRS ${GLUT_LIBRARY_DIR})
endif(GLUT_FOUND)

#find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
#########################################################
# FIND GLEW
#########################################################

if (WIN32)
    FIND_PATH(GLEW_INCLUDE_DIR NAMES GL/GLEW.h 
    PATHS ${GLEW_DIR}/include)
  FIND_LIBRARY(GLEW_LIBRARIES NAMES GLEW32
    PATHS
    ${GLEW_DIR}/lib/Release/x64
	)
	#message(${GLEW_LIBRARIES})
	if (GLEW_INCLUDE_DIR AND GLEW_LIBRARIES)
		set(GLEW_FOUND TRUE CACHE BOOL "GLEW found?") 
	endif(GLEW_INCLUDE_DIR AND GLEW_LIBRARIES)
endif(WIN32)

if(NOT GLEW_FOUND)
	#message(calling kitware find)
	# Previous find failed, so let's try kitware find. Cleanup variables first.
	set(GLEW_INCLUDE_DIR)
	set(GLEW_LIBRARIES)
	set(GLEW_FOUND)
	find_package(GLEW)
endif(NOT GLEW_FOUND)

if (GLEW_FOUND)
	set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
	set(GLEW_LIBRARY_DIRS ${GLEW_LIBRARY_DIR})
endif(GLEW_FOUND)


#find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(${GLEW_DEFINITIONS})
if(NOT GLEW_FOUND)
    message(ERROR " GLEW not found!")
endif(NOT GLEW_FOUND)
#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)



#########################################################
# Include Files
#########################################################
include_directories("include")
 
#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cpp")
 
add_executable(${EXCUTABLE_NAME} ${SOURCES})


########################################################
# Linking & stuff
#########################################################

  # create the program "EXCUTABLE_NAME"
  target_link_libraries(${EXCUTABLE_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
