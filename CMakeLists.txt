cmake_minimum_required(VERSION 3.30)
project(ONYX VERSION 1.0)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_executable(ONYX WIN32)
set_target_properties(ONYX PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(res)
message("
==================================================
 ONYX ENGINE — Build Configuration
==================================================
Version:     ${PROJECT_VERSION}
Compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
Build Type:  ${CMAKE_BUILD_TYPE}
Graphics:    Direct3D 12 (Win32)
Audio:       XAUDIO2
==================================================
")
