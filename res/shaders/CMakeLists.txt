# ============================================
# res/shaders/CMakeLists.txt (Alternative)
# ============================================

# 1. Call the batch script to compile shaders
add_custom_target(ONYX_Shaders ALL
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/Compile.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling HLSL shaders..."
    VERBATIM
)

# 2. Make the main target depend on shaders
if(TARGET ONYX)
    add_dependencies(ONYX ONYX_Shaders)
endif()

# 3. Create a copy command that runs after shaders are compiled
if(TARGET ONYX)
    # Get the output directory for the executable
    get_target_property(ONYX_OUTPUT_DIR ONYX RUNTIME_OUTPUT_DIRECTORY)
    if(NOT ONYX_OUTPUT_DIR)
        set(ONYX_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
    endif()
    
    add_custom_command(
        TARGET ONYX_Shaders
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${ONYX_OUTPUT_DIR}
        COMMENT "Copying .cso shaders to ${ONYX_OUTPUT_DIR}"
        VERBATIM
    )
endif()

message("
ONYX Engine — Shader Compilation:
  Batch script: res/shaders/Compile.bat
  Output: .cso files in res/shaders/
  Copied to: executable directory at build
")