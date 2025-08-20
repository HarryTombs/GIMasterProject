cmake_minimum_required(VERSION 3.10.0)
project(SDL_Proj VERSION 0.1.0 LANGUAGES C CXX)

set(TargetName GIProj)

add_executable(${TargetName})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)
target_include_directories(${TargetName} PRIVATE ${stb_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${TargetName} ${SDL2_LIBRARIES} )

enable_testing()
find_package(GTest REQUIRED)

add_executable(GITests tests/GITests.cpp)
target_include_directories(GITests PRIVATE include)

target_link_libraries(GITests PRIVATE GTest::GTest GTest::Main pthread)
add_test(NAME GITests COMMAND GITests)


target_sources(${TargetName} PRIVATE ${PROJECT_SOURCE_DIR}/src/main.cpp
			${PROJECT_SOURCE_DIR}/src/FrameBufferObject.cpp
			${PROJECT_SOURCE_DIR}/src/Pass.cpp
			${PROJECT_SOURCE_DIR}/src/Graph.cpp
			${PROJECT_SOURCE_DIR}/src/Scene.cpp
			${PROJECT_SOURCE_DIR}/src/ShaderUtils.cpp
			${PROJECT_SOURCE_DIR}/src/TextureUtils.cpp
			${PROJECT_SOURCE_DIR}/src/stb_image.cpp
			${PROJECT_SOURCE_DIR}/include/SDLWindow.h
			${PROJECT_SOURCE_DIR}/include/ShaderUtils.h
			${PROJECT_SOURCE_DIR}/include/TextureUtils.h
			${PROJECT_SOURCE_DIR}/include/Camera.h
			${PROJECT_SOURCE_DIR}/include/FrameBufferObject.h
			${PROJECT_SOURCE_DIR}/include/Pass.h
			${PROJECT_SOURCE_DIR}/include/Graph.h
			${PROJECT_SOURCE_DIR}/include/Scene.h
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