cmake_minimum_required(VERSION 3.16)

project(SnakeII VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

fetchcontent_declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.0
)

fetchcontent_makeavailable(raylib)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/shared_context.cpp
  src/asset_manager.cpp
  src/scenes/logo_scene.cpp
  src/scenes/game_scene.cpp
  src/scenes/menu_scene.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  raylib)

# Copy assets in output folder
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets
  $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
