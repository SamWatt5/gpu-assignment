cmake_minimum_required(VERSION 3.14)
project(OpenGL_Workspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force all compiled executables to output to the root build/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

# Copy everything in src/shaders into the same flat output directory as the
# executable, so LoadShader() can find shaders by filename at runtime.
function(copy_shaders TARGET)
  file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*")
  if(SHADER_FILES)
    # Use an OUTPUT-based command (stamp file) rather than POST_BUILD so the
    # copy re-runs whenever a shader's mtime changes, even if no .cpp changed
    # and the target itself doesn't need relinking.
    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_shaders.stamp")
    add_custom_command(
      OUTPUT ${STAMP_FILE}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILES}
              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      COMMAND ${CMAKE_COMMAND} -E touch ${STAMP_FILE}
      DEPENDS ${SHADER_FILES}
      COMMENT "Copying shaders for ${TARGET}")
    add_custom_target(${TARGET}_copy_shaders DEPENDS ${STAMP_FILE})
    add_dependencies(${TARGET} ${TARGET}_copy_shaders)
  endif()
endfunction()

# --- Auto-Download GLFW ---
include(FetchContent)
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8)
FetchContent_MakeAvailable(glfw)

# --- OS-Specific OpenGL ---
if(APPLE)
  find_library(OPENGL_LIBRARIES OpenGL REQUIRED)
elseif(UNIX AND NOT APPLE)
  find_package(OpenGL REQUIRED)
  set(OPENGL_LIBRARIES OpenGL::GL)
endif()

# --- Compile Shared Code ---
# Bundle glad and the uni's wrapper class into one library
file(GLOB EXAMPLE_OBJECT_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/example_objects/*.cpp")

add_library(core_engine src/tools/glad.c src/tools/wrapper_glfw.cpp
                        ${EXAMPLE_OBJECT_SOURCES})

# Ensure the core engine knows where its headers are
target_include_directories(core_engine
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link the core engine to OpenGL and GLFW so the wrapper functions work
target_link_libraries(core_engine glfw ${OPENGL_LIBRARIES})

# --- Main Executable ---
# All assignment code lives directly in src/ (src/shaders and src/tools are
# handled separately above), so just glob every .cpp file there.
file(GLOB APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(assignment WIN32 ${APP_SOURCES})
target_link_libraries(assignment core_engine)

copy_shaders(assignment)
