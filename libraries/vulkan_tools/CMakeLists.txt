cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_tools)

add_library(vulkan_tools STATIC
        private/utils/VulkanPhysicalDeviceUtils.cpp
        private/utils/VulkanSwapChainUtils.cpp
        private/utils/VulkanShaderUtils.cpp
        private/utils/VulkanImageUtils.cpp
        private/utils/VulkanMemoryUtils.cpp
        private/utils/VulkanCommandBufferUtils.cpp
        private/utils/VulkanTextureUtils.cpp
        private/utils/VulkanDescriptorUtils.cpp
        private/utils/VulkanPipelineUtils.cpp
        private/utils/VulkanTransitionLayoutUtils.cpp
        private/structs/VulkanInstanceOptions.cpp
        private/structs/VulkanBuffer.cpp
        private/structs/VulkanTexture.cpp
        private/structs/VulkanTextureStaging.cpp
        private/structs/VulkanScreenshot.cpp
        private/structs/DeviceRequirement.cpp)
target_include_directories(vulkan_tools
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
set_target_properties(vulkan_tools PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
add_dependencies(vulkan_tools
        fmt)
target_link_libraries(vulkan_tools
        glm
        stb
        fmt)
target_compile_options(vulkan_tools PRIVATE -Wall -Wextra -Werror)