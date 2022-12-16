#ifndef VK_MANDELBROT_VULKANRENDERER_HPP
#define VK_MANDELBROT_VULKANRENDERER_HPP

#include "glm/glm.hpp"

#include <vector>
#include <array>
#include <string>
#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSync.hpp"
#include "common/VulkanDefaultImageTexture.hpp"
#include "renderPasses/VulkanDefaultOnscreenRenderPass.hpp"
#include "renderPasses/VulkanDefaultOffscreenRenderPass.hpp"
#include "renderPasses/VulkanMultipleOffscreenRenderPass.hpp"
#include "renderPasses/VulkanUiOnscreenRenderPass.hpp"
#include "pipelines/toScreen/VulkanToScreenPipeline.hpp"
#include "pipelines/mandelbrot/VulkanMandelbrotPipeline.hpp"
#include "pipelines/ui/VulkanUiPipeline.hpp"

class VulkanRenderer final
{
  public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;
    VulkanRenderer(VulkanRenderer const &src) = delete;
    VulkanRenderer &operator=(VulkanRenderer const &rhs) = delete;
    VulkanRenderer(VulkanRenderer &&src) = delete;
    VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

    // Default Values
    static constexpr VulkanInstanceOptions const DEFAULT_RENDERER_OPTIONS = {
        VK_FALSE, VK_TRUE, VK_FALSE, VK_TRUE, VK_TRUE,
    };
    static constexpr VkClearColorValue const DEFAULT_CLEAR_COLOR = {
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
    static constexpr VkClearDepthStencilValue const
      DEFAULT_CLEAR_DEPTH_STENCIL = { 1.0f, 0 };

    // Public values
    VkClearColorValue clearColor = DEFAULT_CLEAR_COLOR;
    mandelbrotPushConstants mandelbrotConstants{};
    bool mandelbrotComputeDone{};
    bool saveNextFrame{};

    // Instance related
    void createInstance(std::string &&appName,
                        std::string &&engineName,
                        uint32_t appVersion,
                        uint32_t engineVersion,
                        std::vector<char const *> &&requiredExtensions);
    [[nodiscard]] VkInstance getVkInstance() const;
    void init(VkSurfaceKHR surface,
              VulkanInstanceOptions const &options,
              uint32_t winW,
              uint32_t winH);
    void resize(uint32_t winW, uint32_t winH, float rendererScale, bool vsync);
    void clear();

    // Info related
    [[nodiscard]] std::string const &getAppName() const;
    [[nodiscard]] uint32_t getAppVersion() const;
    [[nodiscard]] std::string const &getEngineName() const;
    [[nodiscard]] uint32_t getEngineVersion() const;

    // Render related
    void draw();

    // Screenshot related
    [[nodiscard]] VulkanScreenshot generateScreenshot() const;

  private:
    static constexpr int32_t const CHUNK_WIDTH = 320;
    static constexpr int32_t const CHUNK_HEIGHT = 180;

    std::string _appName;
    std::string _engineName;
    uint32_t _appVersion{};
    uint32_t _engineVersion{};

    // Vulkan related
    VulkanInstance _vkInstance;
    VulkanSwapChain _swapChain;
    VulkanSync _sync;

    // Textures
    VulkanDefaultImageTexture _imageMandelbrot;

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

    // Cmd Buffers
    std::vector<VkCommandBuffer> _renderCommandBuffers;

    // Screenshot
    VulkanTextureStaging _capturedFrame{};

    // Cmd buffer related
    inline void recordRenderCmd(VkCommandBuffer cmdBuffer,
                                uint32_t imgIndex,
                                VkClearColorValue const &clearColor);
    inline void emitDrawCmds(VkCommandBuffer cmdBuffer);

    // Sub-functions for recordRenderCmd
    inline void recordMandelbrotFirstRenderCmd(
      VkCommandBuffer cmdBuffer,
      VkClearColorValue const &cmdClearColor);
    inline void recordMandelbrotMultipleRenderCmd(
      VkCommandBuffer cmdBuffer,
      VkClearColorValue const &cmdClearColor);
    inline void recordUiRenderCmd(VkCommandBuffer cmdBuffer,
                                  uint32_t imgIndex,
                                  VkClearColorValue const &cmdClearColor);
    inline void recordToScreenRenderCmd(VkCommandBuffer cmdBuffer,
                                        uint32_t imgIndex,
                                        VkClearColorValue const &cmdClearColor);

    // Screenshot related fct
    inline void copyFrameToHostMemory();
};

#endif // VK_MANDELBROT_VULKANRENDERER_HPP
