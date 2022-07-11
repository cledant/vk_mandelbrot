#ifndef VK_MANDELBROT_VULKAN_TOSCREENPIPELINEDESCRIPTION_HPP
#define VK_MANDELBROT_VULKAN_TOSCREENPIPELINEDESCRIPTION_HPP

#include <array>

#include "glm/glm.hpp"

#include "interface/AVulkanPipelineDescription.hpp"

class VulkanToScreenPipelineDescription
  : public AVulkanPipelineDescription<VulkanToScreenPipelineDescription>
{
  public:
    VulkanToScreenPipelineDescription() = default;
    ~VulkanToScreenPipelineDescription() override = default;
    VulkanToScreenPipelineDescription(
      VulkanToScreenPipelineDescription const &src) = default;
    VulkanToScreenPipelineDescription &operator=(
      VulkanToScreenPipelineDescription const &rhs) = default;
    VulkanToScreenPipelineDescription(
      VulkanToScreenPipelineDescription &&src) noexcept = default;
    VulkanToScreenPipelineDescription &operator=(
      VulkanToScreenPipelineDescription &&rhs) noexcept = default;

    void implInit(VulkanDevices const &devices);
    void implClear();

    static constexpr std::array<VkVertexInputBindingDescription, 1> const
      inputBindingDescription{
          { { 0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX } }
      };
    static constexpr std::array<VkVertexInputAttributeDescription, 1> const
      inputAttributeDescription{ { { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 } } };

  protected:
    static constexpr std::array<VkDescriptorSetLayoutBinding, 1> const
      descriptorBindings{ { { 0,
                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                              1,
                              VK_SHADER_STAGE_FRAGMENT_BIT,
                              nullptr } } };
};

#endif // VK_MANDELBROT_VULKAN_TOSCREENPIPELINEDESCRIPTION_HPP
