# /cmake/FindImGUI.cmake
include(FetchContent)

# Fetch Dear ImGui from GitHub
FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG "docking"  # Use the "docking" branch or specify a specific commit/tag as needed
)

FetchContent_MakeAvailable(ImGui)

# Set the ImGui source path manually for clarity
set(IMGUI_SOURCE_DIR ${imgui_SOURCE_DIR})

# Add ImGui library
add_library(ImGui STATIC
    ${IMGUI_SOURCE_DIR}/imgui.cpp
    ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
)

# Include SDL2 and DirectX11 backends for ImGui
target_sources(ImGui PRIVATE
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

# Add include directories for ImGui headers
target_include_directories(ImGui PUBLIC
    ${IMGUI_SOURCE_DIR}                # Main ImGui headers (e.g., imgui.h)
    ${IMGUI_SOURCE_DIR}/backends       # Backend headers (e.g., imgui_impl_sdl.h, imgui_impl_dx11.h)
)