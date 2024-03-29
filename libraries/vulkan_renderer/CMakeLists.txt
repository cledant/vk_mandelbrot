cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_renderer)

add_library(vulkan_renderer STATIC
        private/VulkanRenderer.cpp
        private/VulkanSync.cpp)
target_include_directories(vulkan_renderer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
set_target_properties(vulkan_renderer PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
add_dependencies(vulkan_renderer
        vulkan_tools
        vulkan_assets
        vulkan_instance
        fmt)
target_link_libraries(vulkan_renderer
        vulkan_tools
        vulkan_assets
        vulkan_instance
        glm
        fmt)
target_compile_options(vulkan_renderer PRIVATE -Wall -Wextra -Werror)