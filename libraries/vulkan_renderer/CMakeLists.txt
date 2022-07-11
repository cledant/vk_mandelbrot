cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_renderer)

add_library(vulkan_renderer STATIC
        private/VulkanRenderer.cpp
        private/VulkanDebug.cpp
        private/VulkanInstance.cpp
        private/VulkanSwapChain.cpp
        private/VulkanSync.cpp
        private/VulkanInstanceOptions.cpp
        private/renderPass/VulkanDefaultOnscreenRenderPass.cpp
#[[        private/renderPass/VulkanDefaultOffscreenRenderPass.cpp]]
        private/utils/VulkanPhysicalDevice.cpp
        private/utils/VulkanSwapChainUtils.cpp
        private/utils/VulkanShader.cpp
        private/utils/VulkanImageUtils.cpp
        private/utils/VulkanMemory.cpp
        private/utils/VulkanCommandBuffer.cpp
        private/utils/VulkanTextureUtils.cpp
        private/utils/VulkanDescriptorUtils.cpp
        private/utils/VulkanPipelineUtils.cpp
        private/toScreen/VulkanToScreenPipeline.cpp
        private/toScreen/VulkanToScreenPipelineData.cpp
        private/toScreen/VulkanToScreenPipelineDescription.cpp
#[[        private/mandelbrot/VulkanMandelbrotPipeline.cpp
        private/mandelbrot/VulkanMandelbrotPipelineData.cpp
        private/mandelbrot/VulkanMandelbrotPipelineDescription.cpp]]
        private/common/VulkanBuffer.cpp
        private/common/VulkanTexture.cpp
        private/common/VulkanTextureStaging.cpp)
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
        fmt)
target_link_libraries(vulkan_renderer
        glm
        stb
        fmt)
target_compile_options(vulkan_renderer PRIVATE -Wall -Wextra -Werror)