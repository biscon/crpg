cmake_minimum_required(VERSION 3.8)
project(game)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "-O3 -funroll-loops -fomit-frame-pointer -ftree-loop-distribution -ftree-vectorize")
# uncomment this to compile as gui app (doesnt open the console)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
#set(CMAKE_EXE_LINKER_FLAGS "-lGL")

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


#if(WIN32)
#endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../")

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/include)
#message(0, ${FREETYPE_INCLUDE_DIRS})

set(CGLM_SOURCE_FILES
        src/util/cglm/affine.c
        src/util/cglm/box.c
        src/util/cglm/cam.c
        src/util/cglm/ease.c
        src/util/cglm/euler.c
        src/util/cglm/frustum.c
        src/util/cglm/io.c
        src/util/cglm/mat3.c
        src/util/cglm/mat4.c
        src/util/cglm/plane.c
        src/util/cglm/project.c
        src/util/cglm/quat.c
        src/util/cglm/sphere.c
        src/util/cglm/vec3.c
        src/util/cglm/vec4.c)

set(UTIL_SOURCE_FILES
        src/include/store.h
        src/util/store.c
        src/include/list.h
        src/util/list.c
        src/util/vector.c
        src/include/vector.h
        src/util/string_util.c
        src/util/string_util.h
        src/util/cJSON/cJSON.h
        src/util/cJSON/cJSON.c
        src/util/list.c
        src/include/list.h
        src/include/stb_rect_pack.h
        src/util/lodepng.h
        src/util/lodepng.c src/util/math_util.c src/util/math_util.h)

set(GAME_SOURCE_FILES
        src/include/defs.h
        src/main.c
        src/glad/glad.c
        src/rpg/AStar.h
        src/rpg/AStar.c
        src/rpg/dice.c
        src/rpg/ability_score.c
        src/rpg/ability_score.h
        src/rpg/entity.c
        src/rpg/entity.h
        src/rpg/entity_class.c
        src/rpg/entity_class.h
        src/rpg/rpg.c
        src/rpg/rpg.h
        src/rpg/armor.c
        src/rpg/armor.h
        src/rpg/weapon.c
        src/rpg/weapon.h
        src/rpg/monster.c
        src/rpg/monster.h
        src/rpg/rpg_defs.h
        src/rpg/rpg_log.c
        src/rpg/rpg_log.h
        src/rpg/grammar.c
        src/rpg/grammar.h
        src/rpg/faction.c
        src/rpg/faction.h
        src/rpg/attack.c
        src/rpg/attack.h
        src/rpg/combat.c
        src/rpg/combat.h
        src/util/list.c
        src/include/list.h
        src/renderer/renderer.c
        src/renderer/renderer.h
        src/renderer/opengl_renderer.c
        src/renderer/opengl_renderer.h
        src/renderer/pixel_buffer.c
        src/renderer/pixel_buffer.h src/renderer/font.c src/renderer/font.h src/renderer/texture_atlas.c src/renderer/texture_atlas.h src/util/hashtable.c src/renderer/terminal.c src/renderer/terminal.h)

add_executable(game ${GAME_SOURCE_FILES} ${UTIL_SOURCE_FILES} ${CGLM_SOURCE_FILES})

target_link_libraries(game m ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${FREETYPE_LIBRARIES})