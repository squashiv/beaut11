cmake_minimum_required(VERSION 3.5)
project(beaut11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# SDL2
FetchContent_Declare(
	SDL2
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-2.30.9
)
FetchContent_MakeAvailable(SDL2)
include_directories(${SDL2_INCLUDE_DIRS})
# SDL2

# DIRETX11
find_package(DirectX11 REQUIRED)
include_directories(beaut11 PRIVATE ${DirectX11_INCLUDE_DIRS})
# DIRETX11

# GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG        master
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR})
# GLM

# IMGUI
include(${CMAKE_SOURCE_DIR}/cmake/FindImGUI.cmake)
# IMGUI

# BEAUT11
add_executable(beaut11 ${SOURCES} ${HEADERS})
target_include_directories(beaut11 PRIVATE src)
add_custom_command(TARGET beaut11 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/src/assets"
      $<TARGET_FILE_DIR:beaut11>/assets)
# BEAUT11

# DIRETX11
target_link_libraries(beaut11 PRIVATE SDL2::SDL2main SDL2::SDL2 ${DirectX11_LIBRARIES} glm::glm ImGui)
# DIRETX11

# SDL2
add_custom_command(TARGET beaut11 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_BINARY_DIR}/Debug/SDL2d.dll"
        $<TARGET_FILE_DIR:beaut11>)
# SDL2

# IMGUI
target_link_libraries(ImGui PUBLIC
    SDL2
    ${DIRECTX11_LIBRARIES}
)
# IMGUI