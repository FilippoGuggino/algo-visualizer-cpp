cmake_minimum_required(VERSION 3.16)

project(WasmExample VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
find_package(glfw3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/glad)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad)

if (${PLATFORM} MATCHES "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
endif()

if (${PLATFORM} MATCHES "Web")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 -sMAX_WEBGL_VERSION=2 --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM=Web)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--preload-file resources/")
endif()
