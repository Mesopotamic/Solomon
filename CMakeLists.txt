cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(Solomon VERSION 0.1.0 LANGUAGES C)

# All of the options that control how Solomon is built
# You can edit these by setting cache variables before 
# Adding this CMake sub-directory
option(SOLOMON_BUILD_EXAMPLES "Build the examples for Solomon" ON)

# Export all of the symbols on Windows 
# So there's something to link at compile time
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Stop the Solomon project from getting added multiple times
if(TARGET Solomon)
    return()
endif()

# On Windows, GUI applications need to use WinMain
# Or else they spawn a console unnecessarily 
# To handle that abstraction we introduce SolomonMain
# which uses macros to abstract between main and win main
add_library(Solomon SHARED SolomonWin32Entry.c Include/Solomon.h)

# Now lets add the include public include directories for
# Solomon.
set(SOLOMON_include_dirs "${CMAKE_CURRENT_LIST_DIR}/Include" CACHE PATH "Path to Solomon header")
target_include_directories(Solomon PUBLIC ${SOLOMON_include_dirs})

# Add the rest of the Solomon source files to build the shared library
add_subdirectory(Source)

# Only add the examples at the users request.
if(${SOLOMON_BUILD_EXAMPLES})
    message(STATUS "Adding Solomon examples")
    add_subdirectory(Examples)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HelloSolomon)
    set_target_properties(Solomon PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:HelloSolomon>
        LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:HelloSolomon>)
endif()

################################################################
# Organize all source files, this is done last
# Get all of the source files added to the project
get_target_property(PROJ_SOURCES Solomon SOURCES)

# Add a little function that sorts all of the files to be
# Yoinked from https://stackoverflow.com/a/31423421
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${PROJ_SOURCES})