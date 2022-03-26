cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

target_include_directories(Solomon PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_sources(Solomon PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/SolomonCommon.h
	${CMAKE_CURRENT_LIST_DIR}/SolomonCommon.c
	${CMAKE_CURRENT_LIST_DIR}/SolomonCommonUI.c)

# Add in the platform dependant stuff
if(WIN32)
	add_subdirectory(Win32)
endif()

