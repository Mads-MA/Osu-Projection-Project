cmake_minimum_required(VERSION 3.19)

project(osu_projection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C++/WinRT support
if(MSVC)
    add_compile_options(/W4 /permissive- /await /bigobj)
    # Ensure C++/WinRT functionality is enabled
    add_definitions(-DWINRT_LEAN_AND_MEAN)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Find or download C++/WinRT if not already included
include(FetchContent)
FetchContent_Declare(
    cppwinrt
    URL https://github.com/microsoft/cppwinrt/releases/download/2.0.230706.1/Microsoft.Windows.CppWinRT.2.0.230706.1.nupkg
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(cppwinrt)


# vcpkg packages
find_package(Eigen3)
find_package(imgui)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenCV REQUIRED)



#Copy assets to build folder
set(ASSETS_SRC_DIR "${PROJECT_SOURCE_DIR}/assets")
set(ASSETS_DST_DIR "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assets")
file(MAKE_DIRECTORY ${ASSETS_DST_DIR})
# Copy the assets folder to the build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SRC_DIR} ${ASSETS_DST_DIR}
    COMMENT "Copying assets folder to build directory"
)



# Create executable and list src files
add_executable(osu_projection 
    src/main.cpp
    src/Window.cpp
    src/Shader.cpp
    src/WindowCapture.cpp
    src/WebcamCapture.cpp
    src/RenderTexture.cpp
    src/Calibration.cpp
    src/Texture.cpp
)

# Headers no?
target_include_directories(osu_projection PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Configure C++/WinRT
set_target_properties(osu_projection PROPERTIES 
    VS_GLOBAL_CppWinRTOptimized true
    VS_GLOBAL_CppWinRTRootNamespaceAutoMerge true
    VS_GLOBAL_UseWinRT true
)

# Link necessary libraries
target_link_libraries(osu_projection PRIVATE
    dxgi
    d3d11
    d3dcompiler
    windowsapp  #Windows.Graphics.Capture
    user32
    imgui::imgui
    glad::glad
    glfw
    Eigen3::Eigen
    ${OpenCV_LIBS}
)

add_dependencies(osu_projection copy_assets)

set_target_properties(osu_projection PROPERTIES OUTPUT_NAME "Osu! Projection Project")