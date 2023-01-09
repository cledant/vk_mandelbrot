cmake_minimum_required(VERSION 3.17)
project(lib_vulkan_assets)

add_library(vulkan_assets STATIC
        private/imageTextures/VulkanDefaultImageTexture.cpp
        private/renderPasses/VulkanDefaultOnscreenRenderPass.cpp
        private/renderPasses/VulkanDefaultOffscreenRenderPass.cpp
        private/renderPasses/VulkanMultipleOffscreenRenderPass.cpp
        private/renderPasses/VulkanUiOnscreenRenderPass.cpp
        private/pipelines/ui/VulkanUiPipeline.cpp
        private/pipelines/toScreen/VulkanToScreenPipeline.cpp
        private/pipelines/toScreen/VulkanToScreenPipelineData.cpp
        private/pipelines/toScreen/VulkanToScreenPipelineDescription.cpp
        private/pipelines/mandelbrot/VulkanMandelbrotPipeline.cpp
        private/pipelines/mandelbrot/VulkanMandelbrotPipelineData.cpp
        private/pipelines/mandelbrot/VulkanMandelbrotPipelineDescription.cpp)
target_include_directories(vulkan_assets
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)
set_target_properties(vulkan_assets PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
add_dependencies(vulkan_assets
        vulkan_tools
        vulkan_instance
        imgui_glfw_vulkan
        fmt)
target_link_libraries(vulkan_assets
        vulkan_tools
        vulkan_instance
        glm
        fmt
        imgui_glfw_vulkan)
target_compile_options(vulkan_assets PRIVATE -Wall -Wextra -Werror)