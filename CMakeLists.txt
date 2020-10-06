cmake_minimum_required(VERSION 3.17)
project(Fluid)
set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb)

add_library(libglad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/src/glad.c)

add_executable(Fluid main.cpp Fluid2D.cpp ComplexPlane.cpp GraphicalUtil.cpp)

target_link_libraries(Fluid libglad)
target_link_libraries(Fluid ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/lib/win32/release/glfw3.lib)

set(CMAKE_INSTALL_PREFIX ".")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Res DESTINATION ${CMAKE_INSTALL_PREFIX})