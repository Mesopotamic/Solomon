cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(Solomon VERSION 0.1.0 LANGUAGES C)

# CMake used for every Mesopotamic project ========================

# Builds can be ran for just Solomon or the Mesopotamic root
# The dependencies have different locations based on where the
# build originates from, cache the component variables
# If build root is mesopotamic then these caches won't overwrite
set(MESO_SOLOMON ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "")
set(MESO_CMAKE ${CMAKE_CURRENT_LIST_DIR}/CMake CACHE PATH "")

# Has the user specificed a static build of the library?
# This is done when working on Mesopotamic components 
# because you don't have to copy the components across
# Although by default we build shared libraries with 
# all their symbols exported
if(${MESO_BUILD_STATIC})
    set(BUILD_TYPE "")
else()
    set(BUILD_TYPE "SHARED")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Add in the CMake helpers used by all mesopotamic projects
list(APPEND CMAKE_MODULE_PATH ${MESO_CMAKE})
include(Mesopotamic)
# =================================================================

# Stop the Solomon project from getting added multiple times
if(TARGET Solomon)
    return()
endif()

# Add the Solomon library
add_library(Solomon ${BUILD_TYPE} Source/SolomonCommon.c Include/Solomon.h)

# Set the Solomon include directory 
set(SOLOMON_include_dirs "${CMAKE_CURRENT_LIST_DIR}/Include" CACHE PATH "Path to Solomon header")
target_include_directories(Solomon PUBLIC ${SOLOMON_include_dirs})

# Add the rest of the Solomon source files to build the shared library
add_subdirectory(Source)

# Only build the examples apon the users request 
if(${MESO_BUILD_EXAMPLES})
    message(STATUS "Adding Solomon examples")
    add_subdirectory(Examples)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VulkanSurface)
    set_target_properties(Solomon PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:VulkanSurface>
        LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:VulkanSurface>)
endif()

################################################################
# We need to do the linking in the top level cmake because 
# of our cmake version, so do all the linking in here
if(WIN32)
    # Win32 Vulkan surface
    target_compile_definitions(Solomon PRIVATE "VK_USE_PLATFORM_WIN32_KHR")
elseif(UNIX AND NOT APPLE)
    # Linux builds, for now we only support XCB on linux
    set(LINUX TRUE CACHE BOOL "True when linux is platform is detected")
    target_compile_definitions(Solomon PRIVATE "VK_USE_PLATFORM_XCB_KHR")

    # Link and include XCB by producing an imported target
    target_link_libraries(Solomon PRIVATE /usr/lib/x86_64-linux-gnu/libxcb-randr.so)
    add_library(XCB SHARED IMPORTED)
    set_target_properties(XCB PROPERTIES IMPORTED_LOCATION 
        "/usr/lib/x86_64-linux-gnu/libxcb.so")
    target_include_directories(XCB INTERFACE "/usr/include/X11")

    target_link_libraries(Solomon PRIVATE XCB)

endif()

################################################################
# Solomon also produces Vulkan surface extension abstraction
# We only use the header
find_package(Vulkan QUIET REQUIRED)
if(NOT ${Vulkan_FOUND})
    message(FATAL_ERROR "Couldn't find Vulkan installed on your system")
endif()

target_include_directories(Solomon PRIVATE ${Vulkan_INCLUDE_DIRS})

################################################################
meso_sort_target(Solomon)