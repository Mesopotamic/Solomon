cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

target_sources(Solomon PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/SolomonWin32.h
	${CMAKE_CURRENT_LIST_DIR}/SolomonWin32Entry.c
	${CMAKE_CURRENT_LIST_DIR}/Proc.c)