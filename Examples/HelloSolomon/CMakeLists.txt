cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

# We need to specify win32 here to stop an unnecacary terminal from spawning
# Solomon creates a terminal in debug mode for you, so you can have debug logs
# But the terminal disapears in release mode
add_executable(HelloSolomon WIN32 Main.c)

# Find the solomon library cmake
add_subdirectory(../../ Solomon)

# Link and include the solomon directories
target_link_libraries(HelloSolomon Solomon)
target_include_directories(HelloSolomon PRIVATE ${SOLOMON_include_dirs})


