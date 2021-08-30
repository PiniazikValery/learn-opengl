cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME LearnOpenGL)

project(${PROJECT_NAME})

file(GLOB_RECURSE HEADERS "src/**.h")
include_directories(${HEADERS})

file(GLOB_RECURSE REALISATIONS "src/**.cpp")

add_executable(${PROJECT_NAME} ${REALISATIONS} ${HEADERS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory(external/glm)
target_link_libraries(${PROJECT_NAME} glm)

add_subdirectory(external/pybullet)
target_link_libraries(${PROJECT_NAME} pybullet)

add_subdirectory(external/assimp)
target_link_libraries(${PROJECT_NAME} assimp)

# Copy vertex and fragment shaders
set(SHADERS_FOLDER "src/shaders")
file(COPY ${SHADERS_FOLDER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy textures
set(TEXTURES_FOLDER "src/textures")
file(COPY ${TEXTURES_FOLDER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#Copy resources
set(RESOURCES_FOLDER "src/resources")
file(COPY ${RESOURCES_FOLDER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
