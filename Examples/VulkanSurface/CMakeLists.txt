cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

# We need to specify win32 here to stop an unnecacary terminal from spawning
# Solomon creates a terminal in debug mode for you, so you can have debug logs
# But the terminal disapears in release mode
add_executable(VulkanSurface WIN32 Main.c)

# Find the solomon library cmake
add_subdirectory(../../ Solomon)

# Link and include the solomon directories
target_link_libraries(VulkanSurface PRIVATE Solomon)
target_include_directories(VulkanSurface PRIVATE ${SOLOMON_include_dirs})

# Add a custom build step which copies the Solomon shared library 
# into the working directory of the demo
add_custom_command(
	TARGET VulkanSurface POST_BUILD
	COMMENT "Copying Solomon Shared lib"
	COMMAND ${CMAKE_COMMAND} -E copy
	"$<TARGET_FILE:Solomon>"
	"$<TARGET_FILE_DIR:VulkanSurface>")

# Now add Vulkan to the example
find_package(Vulkan QUIET REQUIRED)
target_link_libraries(VulkanSurface PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(VulkanSurface PRIVATE ${Vulkan_INCLUDE_DIR})

# Set Visual studio to read files where the executable is located
set_target_properties(VulkanSurface PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:VulkanSurface>)

# Copy shaders to the executable's location
add_custom_command(
	TARGET VulkanSurface POST_BUILD
	COMMENT "Copying Shaders"
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_CURRENT_LIST_DIR}/Shader.vert.spv"
	"$<TARGET_FILE_DIR:VulkanSurface>")

add_custom_command(
	TARGET VulkanSurface POST_BUILD
	COMMENT "Copying Shaders"
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_CURRENT_LIST_DIR}/Shader.frag.spv"
	"$<TARGET_FILE_DIR:VulkanSurface>")