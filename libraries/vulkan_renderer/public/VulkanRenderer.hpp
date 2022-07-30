#ifndef VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP
#define VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP

#include "glm/glm.hpp"

#include <vector>
#include <array>
#include <string>
#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSync.hpp"
#include "common/VulkanDefaultImageBuffer.hpp"
#include "renderPass/VulkanDefaultOnscreenRenderPass.hpp"
#include "renderPass/VulkanDefaultOffscreenRenderPass.hpp"
#include "toScreen/VulkanToScreenPipeline.hpp"
#include "mandelbrot/VulkanMandelbrotPipeline.hpp"

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
        VK_FALSE,
        VK_TRUE,
        VK_FALSE
    };
    static constexpr VkClearColorValue const DEFAULT_CLEAR_COLOR = {
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
    static constexpr VkClearDepthStencilValue const
      DEFAULT_CLEAR_DEPTH_STENCIL = { 1.0f, 0 };

    // Public values
    VkClearColorValue clearColor = DEFAULT_CLEAR_COLOR;
    mandelbrotPushConstants mandelbrotConstants{};

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
    void resize(uint32_t winW, uint32_t winH);
    void clear();

    // Info related
    [[nodiscard]] std::string const &getAppName() const;
    [[nodiscard]] uint32_t getAppVersion() const;
    [[nodiscard]] std::string const &getEngineName() const;
    [[nodiscard]] uint32_t getEngineVersion() const;

    // Render related
    void draw();

  private:
    std::string _appName;
    std::string _engineName;
    uint32_t _appVersion{};
    uint32_t _engineVersion{};

    VulkanInstance _vkInstance;
    VulkanSwapChain _swapChain;
    VulkanSync _sync;
    VulkanDefaultOnscreenRenderPass _toScreenRenderPass;
    VulkanToScreenPipeline _toScreen;
    VulkanDefaultImageBuffer _imageDisplayed;
    VulkanDefaultOffscreenRenderPass _mandelbrotRenderPass;
    VulkanMandelbrotPipeline _mandelbrot;

    // Cmd Buffers
    std::vector<VkCommandBuffer> _renderCommandBuffers;

    inline void emitDrawCmds(uint32_t imgIndex);
    inline void recordRenderCmd(uint32_t imgIndex,
                                VkClearColorValue const &clearColor);
    inline void recordMandelbrotRenderCmd(
      uint32_t imgIndex,
      VkClearColorValue const &cmdClearColor);
    inline void recordToScreenRenderCmd(uint32_t imgIndex,
                                        VkClearColorValue const &cmdClearColor);
};

#endif // VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP
