#ifndef VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
#define VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanMandelbrotPipelineData.hpp"
#include "VulkanMandelbrotPipelineDescription.hpp"
#include "renderPasses/VulkanDefaultOffscreenRenderPass.hpp"
#include "pipelines/mandelbrot/MandelbrotPushConstants.hpp"
#include "imageTextures/VulkanDefaultImageTexture.hpp"

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
              VkRenderPass renderPass,
              int32_t imgW,
              int32_t imgH);
    void resize(VkRenderPass renderPass, int32_t imgW, int32_t imgH);
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

    inline void createGfxPipeline(VkRenderPass renderPass,
                                  int32_t imgW,
                                  int32_t imgH);
    inline void createDescriptorSets(
      VulkanMandelbrotPipelineData &pipelineData);
    inline void createDescriptorPool();
};

#endif // VK_MANDELBROT_VULKANMANDELBROTPIPELINE_HPP
