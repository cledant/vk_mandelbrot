cmake_minimum_required(VERSION 3.17)
project(lib_engine)

add_library(engine STATIC
        private/Engine.cpp
        private/EventHandler.cpp
        private/GfxAssets.cpp)
target_include_directories(engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
add_dependencies(engine
        iomanager
        vulkan_tools
        vulkan_instance
        vulkan_assets
        vulkan_renderer
        app_info
        ui)
target_link_libraries(engine PUBLIC
        iomanager
        vulkan_tools
        vulkan_instance
        vulkan_assets
        vulkan_renderer
        app_info
        ui)
set_target_properties(engine PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
target_compile_options(engine PRIVATE -Wall -Wextra -Werror)