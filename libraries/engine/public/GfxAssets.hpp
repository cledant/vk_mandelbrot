#ifndef VK_MANDELBROT_GFXASSETS_HPP
#define VK_MANDELBROT_GFXASSETS_HPP

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "imageTextures/VulkanDefaultImageTexture.hpp"
#include "renderPasses/VulkanDefaultOnscreenRenderPass.hpp"
#include "renderPasses/VulkanDefaultOffscreenRenderPass.hpp"
#include "renderPasses/VulkanMultipleOffscreenRenderPass.hpp"
#include "renderPasses/VulkanUiOnscreenRenderPass.hpp"
#include "pipelines/toScreen/VulkanToScreenPipeline.hpp"
#include "pipelines/mandelbrot/VulkanMandelbrotPipeline.hpp"
#include "pipelines/ui/VulkanUiPipeline.hpp"

struct GfxAssets final
{
    void init(VulkanInstance vkInstance,
              uint32_t winW,
              uint32_t winH,
              std::vector<VulkanTexture> const &swapChainImageViews,
              VkFormat swapChainImageFormat,
              float scale);
    void resize(uint32_t winW,
                uint32_t winH,
                std::vector<VulkanTexture> const &swapChainImageViews,
                VkFormat swapChainImageFormat,
                float scale);
    void clear();

    // Textures
    VulkanDefaultImageTexture _imageMandelbrot;
    VulkanTextureStaging _capturedFrame{};

    // Render passes
    VulkanDefaultOffscreenRenderPass _mandelbrotFirstRenderPass;
    VulkanMultipleOffscreenRenderPass _mandelbrotMultipleRenderPass;
    VulkanDefaultOnscreenRenderPass _toScreenRenderPass;
    VulkanUiOnscreenRenderPass _uiRenderPass;

    // Pipelines
    VulkanMandelbrotPipeline _mandelbrotFirst;
    VulkanMandelbrotPipeline _mandelbrotMultiple;
    VulkanToScreenPipeline _toScreen;
    VulkanUiPipeline _ui;

    // Push Constants
    mandelbrotPushConstants mandelbrotConstants{};

  private:
    VulkanDevices _devices;
    VkFormat _depthFormat{};
};

#endif // VK_MANDELBROT_GFXASSETS_HPP
