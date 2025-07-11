cmake_minimum_required(VERSION 3.10.0)
project(SDL_Proj VERSION 0.1.0 LANGUAGES C CXX)

set(TargetName SDL_Proj)

add_executable(${TargetName})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${TargetName} ${SDL2_LIBRARIES} )


include_directories(include $ENV{HOME}/NGL/include)
target_sources(${TargetName} PRIVATE ${PROJECT_SOURCE_DIR}/src/main.cpp
			${PROJECT_SOURCE_DIR}/src/FrameBufferObject.cpp
			${PROJECT_SOURCE_DIR}/include/SDLWindow.h
			${PROJECT_SOURCE_DIR}/include/ShaderUtils.h
			${PROJECT_SOURCE_DIR}/include/Camera.h
			${PROJECT_SOURCE_DIR}/include/FrameBufferObject.h
			${PROJECT_SOURCE_DIR}/include/Mesh.h)


# Find and link GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(${TargetName} ${GLEW_LIBRARIES})

# Find and link GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
target_link_libraries(${TargetName} ${GLUT_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(${TargetName}  ${OPENGL_LIBRARIES})

find_package(assimp REQUIRED)
target_link_libraries(${TargetName}  ${ASSIMP_LIBRARIES})

add_compile_definitions(ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")



add_custom_command(TARGET ${TargetName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:${TargetName}>/shaders)