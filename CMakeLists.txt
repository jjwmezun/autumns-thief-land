cmake_minimum_required(VERSION 3.2)

project(AutumnsThiefLand)
set(OpenGL_GL_PREFERENCE "GLVND")
add_executable(AutumnsThiefLand
    src/engine.c
    src/main.c
    src/map.c
    src/sprite.c
    src/tile.c
)
set_property(TARGET AutumnsThiefLand PROPERTY C_STANDARD 23)
target_link_libraries(AutumnsThiefLand m)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
find_path(GLEW_INCLUDE_DIR GL/glew.h)
find_path(GLU_INCLUDE_DIR GL/glu.h)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(include)
target_link_libraries(AutumnsThiefLand ${OPENGL_LIBRARIES})
target_link_libraries(AutumnsThiefLand ${GLEW_LIBRARIES})
target_link_libraries(AutumnsThiefLand SDL2)