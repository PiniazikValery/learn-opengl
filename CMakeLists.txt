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
file(GLOB_RECURSE V_SHADER_SOURCES "src/shaders/**.vs")
file(GLOB_RECURSE F_SHADER_SOURCES "src/shaders/**.fs")
set(BUILD_VERTEX_SHADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex")
set(BUILD_FRAGMENT_SHADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment")
file(MAKE_DIRECTORY ${BUILD_VERTEX_SHADER_DIR})
file(MAKE_DIRECTORY ${BUILD_FRAGMENT_SHADER_DIR})

foreach(SHADER_SOURCE ${V_SHADER_SOURCES})
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_SOURCE} ${BUILD_VERTEX_SHADER_DIR}
    )
endforeach(SHADER_SOURCE)

foreach(SHADER_SOURCE ${F_SHADER_SOURCES})
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_SOURCE} ${BUILD_FRAGMENT_SHADER_DIR}
    )
endforeach(SHADER_SOURCE)

# Copy textures
file(GLOB_RECURSE TEXTURE_SOURCES "src/textures/**.*")
set(BUILD_TEXTURE_DIR "${CMAKE_CURRENT_BINARY_DIR}/textures")
file(MAKE_DIRECTORY ${BUILD_TEXTURE_DIR})

foreach(TEXTURE_SOURCE ${TEXTURE_SOURCES})
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${TEXTURE_SOURCE} ${BUILD_TEXTURE_DIR}
    )
endforeach(TEXTURE_SOURCE)
