cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_renderer)

add_library(vulkan_renderer STATIC
        private/VulkanRenderer.cpp
        private/VulkanDebug.cpp
        private/VulkanInstance.cpp
        private/VulkanSwapChain.cpp
        private/VulkanSync.cpp
        private/VulkanInstanceOptions.cpp
        private/defaultRenderPass/VulkanDefaultOnscreenRenderPass.cpp
        private/defaultRenderPass/VulkanDefaultOffscreenRenderPass.cpp
        private/utils/VulkanPhysicalDevice.cpp
        private/utils/VulkanSwapChainUtils.cpp
        private/utils/VulkanShader.cpp
        private/utils/VulkanImageUtils.cpp
        private/utils/VulkanMemory.cpp
        private/utils/VulkanCommandBuffer.cpp
        private/utils/VulkanTextureUtils.cpp
        private/utils/VulkanDescriptorUtils.cpp
        private/utils/VulkanPipelineUtils.cpp
        private/utils/VulkanTransitionLayout.cpp
        private/pipelines/ui/VulkanUiPipeline.cpp
        private/pipelines/ui/VulkanUiOffscreenRenderPass.cpp
        private/pipelines/toScreen/VulkanToScreenPipeline.cpp
        private/pipelines/toScreen/VulkanToScreenPipelineData.cpp
        private/pipelines/toScreen/VulkanToScreenPipelineDescription.cpp
        private/pipelines/mandelbrot/VulkanMandelbrotPipeline.cpp
        private/pipelines/mandelbrot/VulkanMandelbrotPipelineData.cpp
        private/pipelines/mandelbrot/VulkanMandelbrotPipelineDescription.cpp
        private/common/VulkanBuffer.cpp
        private/common/VulkanTexture.cpp
        private/common/VulkanTextureStaging.cpp
        private/common/VulkanDefaultImageBuffer.cpp)
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
        imgui_glfw_vulkan
        fmt)
target_link_libraries(vulkan_renderer
        glm
        stb
        fmt
        imgui_glfw_vulkan)
target_compile_options(vulkan_renderer PRIVATE -Wall -Wextra -Werror)