#ifndef VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
#define VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanMandelbrotPipelineDescription.hpp"
#include "renderPass/VulkanToScreenRenderPass.hpp"
#include "constants/VulkanConstants.hpp"

class VulkanMandelbrotPipeline final
{
  public:
    VulkanMandelbrotPipeline() = default;
    ~VulkanMandelbrotPipeline() = default;
    VulkanMandelbrotPipeline(VulkanMandelbrotPipeline const &src) =
      delete;
    VulkanMandelbrotPipeline &operator=(
      VulkanMandelbrotPipeline const &rhs) = delete;
    VulkanMandelbrotPipeline(VulkanMandelbrotPipeline &&src) = delete;
    VulkanMandelbrotPipeline &operator=(
      VulkanMandelbrotPipeline &&rhs) = delete;

    mandelbrotConstants pushConstants{};

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              VulkanToScreenRenderPass const &renderPass);
    void resize(VulkanSwapChain const &swapChain,
                VulkanToScreenRenderPass const &renderPass);
    void clear();

    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanCommandPools _cmdPools;
    VulkanQueues _queues;

    // Vertex / Fragment shaders related
    VulkanMandelbrotPipelineDescription _pipelineDescription;
    VkPipeline _gfxPipeline{};

    // Global
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorPool _descriptorPool{};

    inline void createGfxPipeline(VulkanSwapChain const &swapChain,
                                  VulkanToScreenRenderPass const &renderPass);
    inline void createDescriptorSets(
      uint32_t descriptorCount);
    void createDescriptorPool(uint32_t descriptorCount);
};

#endif // VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
