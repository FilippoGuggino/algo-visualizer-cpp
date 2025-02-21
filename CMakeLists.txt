cmake_minimum_required(VERSION 3.5)

project(WasmExample VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

fetchcontent_declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.0
)

fetchcontent_makeavailable(raylib)

fetchcontent_declare(
  tweeny
  GIT_REPOSITORY https://github.com/mobius3/tweeny
  GIT_TAG master
)

fetchcontent_makeavailable(tweeny)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib tweeny)

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

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 -sEXPORTED_FUNCTIONS=_main,_on_click_button -sEXPORTED_RUNTIME_METHODS=ccall,cwrap --shell-file ${CMAKE_SOURCE_DIR}/src/shell.html")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM=Web)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--preload-file resources/")
endif()
