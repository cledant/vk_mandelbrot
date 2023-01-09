cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_instance)

add_library(vulkan_instance STATIC
        private/VulkanDebug.cpp
        private/VulkanInstance.cpp)
target_include_directories(vulkan_instance
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
set_target_properties(vulkan_instance PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
add_dependencies(vulkan_instance
        vulkan_tools
        fmt)
target_link_libraries(vulkan_instance
        vulkan_tools
        fmt)
target_compile_options(vulkan_instance PRIVATE -Wall -Wextra -Werror)