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
#include "renderPass/VulkanDefaultOffscreenRenderPass.hpp"
#include "constants/VulkanConstants.hpp"
#include "common/VulkanDefaultImageBuffer.hpp"

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

    mandelbrotConstants pushConstants{};

    void init(VulkanInstance const &vkInstance,
              VulkanDefaultImageBuffer const &imgBuffer,
              VulkanDefaultOffscreenRenderPass const &renderPass);
    void resize(VulkanDefaultImageBuffer const &imgBuffer,
                VulkanDefaultOffscreenRenderPass const &renderPass);
    void clear();

    void generateCommands(VkCommandBuffer cmdBuffer);

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
      VulkanDefaultImageBuffer const &imgBuffer,
      VulkanDefaultOffscreenRenderPass const &renderPass);
    inline void createDescriptorSets(
      VulkanMandelbrotPipelineData &pipelineData);
    void createDescriptorPool();
};

#endif // VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
