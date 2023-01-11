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
    void init(VulkanInstance const &vkInstance,
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

    void recordDrawCmds(VkCommandBuffer cmdBuffer,
                        uint32_t imgIndex,
                        VkClearColorValue const &cmdClearColor,
                        VkClearDepthStencilValue const &cmdClearDepth);

    // Textures
    VulkanDefaultImageTexture imageMandelbrot;
    VulkanTextureStaging capturedFrame{};

    // Render passes
    VulkanDefaultOffscreenRenderPass mandelbrotFirstRenderPass;
    VulkanMultipleOffscreenRenderPass mandelbrotMultipleRenderPass;
    VulkanDefaultOnscreenRenderPass toScreenRenderPass;
    VulkanUiOnscreenRenderPass uiRenderPass;

    // Pipelines
    VulkanMandelbrotPipeline mandelbrotFirst;
    VulkanMandelbrotPipeline mandelbrotMultiple;
    VulkanToScreenPipeline toScreen;
    VulkanUiPipeline vkUi;

    // Push Constants
    mandelbrotPushConstants mandelbrotConstants{};

    // Others
    bool mandelbrotComputeDone{};

  private:
    VulkanDevices _devices;
    VkFormat _depthFormat{};

    // Mandelbrot renderpasses chunk size
    static constexpr int32_t const CHUNK_WIDTH = 320;
    static constexpr int32_t const CHUNK_HEIGHT = 180;

    // Sub-functions for recordDrawCmds
    inline void recordMandelbrotFirstRenderCmd(
      VkCommandBuffer cmdBuffer,
      VkClearColorValue const &cmdClearColor,
      VkClearDepthStencilValue const &cmdClearDepth);
    inline void recordMandelbrotMultipleRenderCmd(
      VkCommandBuffer cmdBuffer,
      VkClearColorValue const &cmdClearColor,
      VkClearDepthStencilValue const &cmdClearDepth);
    inline void recordUiRenderCmd(
      VkCommandBuffer cmdBuffer,
      uint32_t imgIndex,
      VkClearColorValue const &cmdClearColor,
      VkClearDepthStencilValue const &cmdClearDepth);
    inline void recordToScreenRenderCmd(
      VkCommandBuffer cmdBuffer,
      uint32_t imgIndex,
      VkClearColorValue const &cmdClearColor,
      VkClearDepthStencilValue const &cmdClearDepth);
};

#endif // VK_MANDELBROT_GFXASSETS_HPP
