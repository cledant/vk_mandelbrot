#ifndef VK_MANDELBROT_VULKANSURFACEDISPLAYPIPELINE_HPP
#define VK_MANDELBROT_VULKANSURFACEDISPLAYPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSurfaceDisplayPipelineData.hpp"
#include "VulkanSurfaceDisplayPipelineDescription.hpp"
#include "renderPass/VulkanSceneRenderPass.hpp"
#include "constants/VulkanConstants.hpp"

class VulkanSurfaceDisplayPipeline final
{
  public:
    VulkanSurfaceDisplayPipeline() = default;
    ~VulkanSurfaceDisplayPipeline() = default;
    VulkanSurfaceDisplayPipeline(VulkanSurfaceDisplayPipeline const &src) =
      delete;
    VulkanSurfaceDisplayPipeline &operator=(
      VulkanSurfaceDisplayPipeline const &rhs) = delete;
    VulkanSurfaceDisplayPipeline(VulkanSurfaceDisplayPipeline &&src) = delete;
    VulkanSurfaceDisplayPipeline &operator=(
      VulkanSurfaceDisplayPipeline &&rhs) = delete;

    mandelbrotConstants pushConstants{};

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              VulkanSceneRenderPass const &renderPass);
    void resize(VulkanSwapChain const &swapChain,
                VulkanSceneRenderPass const &renderPass);
    void clear();

    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanCommandPools _cmdPools;
    VulkanQueues _queues;

    // Vertex / Fragment shaders related
    VulkanSurfaceDisplayPipelineDescription _pipelineDescription;
    VkPipeline _gfxPipeline{};

    // Global
    VulkanSurfaceDisplayPipelineData _pipelineData;
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorPool _descriptorPool{};

    inline void createGfxPipeline(VulkanSwapChain const &swapChain,
                                  VulkanSceneRenderPass const &renderPass);
    inline void createDescriptorSets(
      VulkanSurfaceDisplayPipelineData &pipelineData,
      uint32_t descriptorCount);
    void createDescriptorPool(uint32_t descriptorCount);
};

#endif // VK_MANDELBROT_VULKANSURFACEDISPLAYPIPELINE_HPP
