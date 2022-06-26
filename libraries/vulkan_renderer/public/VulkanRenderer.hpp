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
#include "renderPass/VulkanSceneRenderPass.hpp"
#include "surfaceDisplay/VulkanSurfaceDisplayPipeline.hpp"

class VulkanRenderer final
{
  public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;
    VulkanRenderer(VulkanRenderer const &src) = delete;
    VulkanRenderer &operator=(VulkanRenderer const &rhs) = delete;
    VulkanRenderer(VulkanRenderer &&src) = delete;
    VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

    // Default Renderer Options
    static constexpr VulkanInstanceOptions const DEFAULT_RENDERER_OPTIONS = {
        VK_FALSE,
        VK_TRUE,
        VK_FALSE
    };

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
    VulkanSceneRenderPass _sceneRenderPass;
    VulkanSurfaceDisplayPipeline _surfaceDisplay;

    // Cmd Buffers
    std::vector<VkCommandBuffer> _renderCommandBuffers;

    inline void emitDrawCmds(uint32_t imgIndex);
    inline void recordRenderCmds();
};

#endif // VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP
