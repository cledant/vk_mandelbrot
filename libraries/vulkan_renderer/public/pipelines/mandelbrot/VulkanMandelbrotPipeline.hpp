#ifndef VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
#define VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanMandelbrotPipelineData.hpp"
#include "VulkanMandelbrotPipelineDescription.hpp"
#include "defaultRenderPass/VulkanDefaultOffscreenRenderPass.hpp"
#include "constants/VulkanConstants.hpp"
#include "common/VulkanDefaultImageTexture.hpp"

class VulkanMandelbrotPipeline final
{
  public:
    VulkanMandelbrotPipeline() = default;
    ~VulkanMandelbrotPipeline() = default;
    VulkanMandelbrotPipeline(VulkanMandelbrotPipeline const &src) = delete;
    VulkanMandelbrotPipeline &operator=(VulkanMandelbrotPipeline const &rhs) =
      delete;
    VulkanMandelbrotPipeline(VulkanMandelbrotPipeline &&src) = delete;
    VulkanMandelbrotPipeline &operator=(VulkanMandelbrotPipeline &&rhs) =
      delete;

    void init(VulkanInstance const &vkInstance,
              VulkanDefaultOffscreenRenderPass const &renderPass,
              int32_t imgW,
              int32_t imgH);
    void resize(VulkanDefaultOffscreenRenderPass const &renderPass,
                int32_t imgW,
                int32_t imgH);
    void clear();

    void generateCommands(VkCommandBuffer cmdBuffer,
                          mandelbrotPushConstants const &pushConstants);

  private:
    // Vulkan related
    VulkanDevices _devices;
    VulkanCommandPools _cmdPools;
    VulkanQueues _queues;

    // Vertex / Fragment shaders related
    VulkanMandelbrotPipelineDescription _pipelineDescription;
    VkPipeline _gfxPipeline{};

    // Global
    VulkanMandelbrotPipelineData _pipelineData;
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorPool _descriptorPool{};

    inline void createGfxPipeline(
      VulkanDefaultOffscreenRenderPass const &renderPass,
      int32_t imgW,
      int32_t imgH);
    inline void createDescriptorSets(
      VulkanMandelbrotPipelineData &pipelineData);
    inline void createDescriptorPool();
};

#endif // VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
