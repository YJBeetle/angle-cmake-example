cmake_minimum_required(VERSION 3.4)

project(example LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# 源文件列表
set(SRC_LIST main.cpp)

subdirs(3rdParty/angle-cmake)

find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
add_dependencies(${PROJECT_NAME} angle)
target_include_directories(${PROJECT_NAME} PUBLIC 3rdParty/angle-cmake/angle/include)
set(ANGLE_BUILD_OUTPUT
    "${CMAKE_BINARY_DIR}/bin-angle/libEGL${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_BINARY_DIR}/bin-angle/libGLESv2${CMAKE_SHARED_LIBRARY_SUFFIX}"
)
target_link_libraries(${PROJECT_NAME} ${ANGLE_BUILD_OUTPUT})
