project(path_tracing)
cmake_minimum_required(VERSION 3.1)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    message(STATUS ">>> Linux")
else()
    message(STATUS ">>> Macos")
endif()

if(LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
else()
    set(CMAKE_C_COMPILER gcc-7)
    set(CMAKE_CXX_COMPILER g++-7)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Ofast -march=native")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE)


include_directories(SYSTEM
        3rdparty/include
        3rdparty/include/sol2
        3rdparty/include/lua
        )

set(SRC src/AABB.cpp
        src/SceneViewer.cpp
        src/KdTree.cpp
        src/Main.cpp
        src/Model.cpp
        src/PathTracer.cpp
        src/Ray.cpp
        src/Scene.cpp
        src/SceneConfigLoader.cpp
        )

set(INC
        src/AABB.h
        src/SceneViewer.h
        src/AbstractObject.h
        src/KdTree.h
        src/PathTracer.h
        src/Ray.h
        src/Scene.h
        src/SceneConfigLoader.h
        src/Types.h
        src/LuaConfigParser.h src/LuaConfigParser.cpp)

add_executable(path_tracing
        src/Main.cpp
        ${SRC} ${INC} )

find_package(OpenGL REQUIRED)


target_link_libraries(path_tracing
        ${OPENGL_LIBRARY}
        glut
        -lpthread
        stdc++fs
        )
if(LINUX)
target_link_libraries(path_tracing
        ${PROJECT_SOURCE_DIR}/3rdparty/linux_lib/liblua.a
        -ldl
        )
else()
    find_package(lua REQUIRED)
    target_link_libraries(path_tracing
            ${LUA_LIBRARIES}
            )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
