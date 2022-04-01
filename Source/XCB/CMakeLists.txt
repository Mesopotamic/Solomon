cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

target_sources(Solomon PRIVATE 
	${CMAKE_CURRENT_LIST_DIR}/XCB_Common.h
	${CMAKE_CURRENT_LIST_DIR}/CreateWindow.c
	${CMAKE_CURRENT_LIST_DIR}/Connection.c
	${CMAKE_CURRENT_LIST_DIR}/Events.c
	${CMAKE_CURRENT_LIST_DIR}/Vulkan.c)