#ifndef VK_MANDELBROT_VULKANSKYBOXRENDERPASS_HPP
#define VK_MANDELBROT_VULKANSKYBOXRENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "interface/AVulkanRenderPass.hpp"

class VulkanSceneRenderPass final
  : public AVulkanRenderPass<VulkanSceneRenderPass>
{
  public:
    VulkanSceneRenderPass() = default;
    ~VulkanSceneRenderPass() override = default;
    VulkanSceneRenderPass(VulkanSceneRenderPass const &src) = default;
    VulkanSceneRenderPass &operator=(VulkanSceneRenderPass const &rhs) =
      default;
    VulkanSceneRenderPass(VulkanSceneRenderPass &&src) noexcept = default;
    VulkanSceneRenderPass &operator=(VulkanSceneRenderPass &&rhs) noexcept =
      default;

    void implInit(VulkanInstance const &vkInstance,
                  VulkanSwapChain const &swapChain);
    void implResize(VulkanSwapChain const &swapChain);
    void implClean();
    void implClear();
};

#endif // VK_MANDELBROT_VULKANSKYBOXRENDERPASS_HPP
