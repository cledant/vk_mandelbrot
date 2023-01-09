#ifndef VK_MANDELBROT_VULKANTOSCREENPIPELINE_HPP
#define VK_MANDELBROT_VULKANTOSCREENPIPELINE_HPP

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

#include "VulkanInstance.hpp"
#include "VulkanToScreenPipelineData.hpp"
#include "VulkanToScreenPipelineDescription.hpp"
#include "renderPasses/VulkanDefaultOnscreenRenderPass.hpp"
#include "pipelines/mandelbrot/MandelbrotPushConstants.hpp"

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
              uint32_t currentSwapChainNbImg,
              uint32_t swapChainImgW,
              uint32_t swapChainImgH,
              VkRenderPass renderPass,
              VkDescriptorImageInfo const &toDisplayImageInfo);
    void resize(uint32_t currentSwapChainNbImg,
                uint32_t swapChainImgW,
                uint32_t swapChainImgH,
                VkRenderPass renderPass,
                VkDescriptorImageInfo const &toDisplayImageInfo);
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

    inline void createGfxPipeline(uint32_t swapChainImgW,
                                  uint32_t swapChainImgH,
                                  VkRenderPass renderPass);
    inline void createDescriptorSets(VulkanToScreenPipelineData &pipelineData,
                                     uint32_t descriptorCount);
    inline void createDescriptorPool(uint32_t descriptorCount);
};

#endif // VK_MANDELBROT_VULKANTOSCREENPIPELINE_HPP
