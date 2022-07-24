#ifndef VK_MANDELBROT_VULKANTOSCREENPIPELINE_HPP
#define VK_MANDELBROT_VULKANTOSCREENPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanToScreenPipelineData.hpp"
#include "VulkanToScreenPipelineDescription.hpp"
#include "renderPass/VulkanDefaultOnscreenRenderPass.hpp"
#include "constants/VulkanConstants.hpp"

class VulkanToScreenPipeline final
{
  public:
    VulkanToScreenPipeline() = default;
    ~VulkanToScreenPipeline() = default;
    VulkanToScreenPipeline(VulkanToScreenPipeline const &src) = delete;
    VulkanToScreenPipeline &operator=(VulkanToScreenPipeline const &rhs) =
      delete;
    VulkanToScreenPipeline(VulkanToScreenPipeline &&src) = delete;
    VulkanToScreenPipeline &operator=(VulkanToScreenPipeline &&rhs) = delete;

    void init(VulkanInstance const &vkInstance,
              VulkanSwapChain const &swapChain,
              VulkanDefaultOnscreenRenderPass const &renderPass,
              VkDescriptorImageInfo const &toDisplayImageInfo,
              bool forceSquareRatio);
    void resize(VulkanSwapChain const &swapChain,
                VulkanDefaultOnscreenRenderPass const &renderPass,
                VkDescriptorImageInfo const &toDisplayImageInfo,
                bool forceSquareRatio);
    void clear();

    void generateCommands(VkCommandBuffer cmdBuffer, size_t descriptorSetIndex);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanCommandPools _cmdPools;
    VulkanQueues _queues;

    // Vertex / Fragment shaders related
    VkDescriptorImageInfo _toDisplayImageInfo{};
    VulkanToScreenPipelineDescription _pipelineDescription;
    VkPipeline _gfxPipeline{};

    // Global
    VulkanToScreenPipelineData _pipelineData;
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorPool _descriptorPool{};

    inline void createGfxPipeline(
      VulkanSwapChain const &swapChain,
      VulkanDefaultOnscreenRenderPass const &renderPass);
    inline void createDescriptorSets(VulkanToScreenPipelineData &pipelineData,
                                     uint32_t descriptorCount);
    void createDescriptorPool(uint32_t descriptorCount);
};

#endif // VK_MANDELBROT_VULKANTOSCREENPIPELINE_HPP
