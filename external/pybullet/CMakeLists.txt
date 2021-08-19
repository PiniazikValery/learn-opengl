cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME pybullet)

project(${PROJECT_NAME})

set(INCLUDE_DIRECTORIES
        .
        BulletCollision/BroadphaseCollision
        BulletCollision/CollisionDispatch
        BulletCollision/CollisionShapes
        BulletCollision/NarrowPhaseCollision
        BulletDynamics/ConstraintSolver
        BulletDynamics/Dynamics
        BulletDynamics/Vehicle
        BulletSoftBody/
        LinearMath
)

file(GLOB BULLET_SRCGLOB
        BulletCollision/BroadphaseCollision/*.cpp
        BulletCollision/CollisionDispatch/*.cpp
        BulletCollision/CollisionShapes/*.cpp
        BulletCollision/NarrowPhaseCollision/*.cpp
        BulletDynamics/ConstraintSolver/*.cpp
        BulletDynamics/Dynamics/*.cpp
        BulletDynamics/Vehicle/*.cpp
        BulletSoftBody/*.cpp
        LinearMath/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${BULLET_SRCGLOB})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRECTORIES})