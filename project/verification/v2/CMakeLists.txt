cmake_minimum_required(VERSION 3.2.0)
project(upper)

add_compile_options(-std=c++11)

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	find_package(OpenCV REQUIRED)
	include_directories(
		${OpenCV_INCLUDE_DIRECTORIES}
		)
endif()

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
	include_directories(
		./lib/opencv/include
		)
	
endif()

add_executable(upper
  main.cpp
)

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(upper
		${OpenCV_LIBRARIES}
		)
endif()

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
	execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin)
	execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
	execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
	set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/")
	file(COPY ./lib/opencv/bin/opencv_world455d.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
	file(COPY ./lib/opencv/bin/opencv_world455d.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
	link_directories(./lib/opencv/lib)
	find_library(opencv_lib NAMES opencv_world455d PATHS ./lib/opencv/lib)
	target_link_libraries(upper
		${opencv_lib}
		)
endif()

