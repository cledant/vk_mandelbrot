#ifndef VK_MANDELBROT_VULKANMANDELBROTPIPELINEDESCRIPTION_HPP
#define VK_MANDELBROT_VULKANMANDELBROTPIPELINEDESCRIPTION_HPP

#include <array>

#include "glm/glm.hpp"

#include "interfaces/AVulkanPipelineDescription.hpp"

class VulkanMandelbrotPipelineDescription
  : public AVulkanPipelineDescription<VulkanMandelbrotPipelineDescription>
{
  public:
    VulkanMandelbrotPipelineDescription() = default;
    ~VulkanMandelbrotPipelineDescription() override = default;
    VulkanMandelbrotPipelineDescription(
      VulkanMandelbrotPipelineDescription const &src) = default;
    VulkanMandelbrotPipelineDescription &operator=(
      VulkanMandelbrotPipelineDescription const &rhs) = default;
    VulkanMandelbrotPipelineDescription(
      VulkanMandelbrotPipelineDescription &&src) noexcept = default;
    VulkanMandelbrotPipelineDescription &operator=(
      VulkanMandelbrotPipelineDescription &&rhs) noexcept = default;

    void implInit(VulkanDevices const &devices);
    void implClear();

    static constexpr std::array<VkVertexInputBindingDescription, 1> const
      inputBindingDescription{
          { { 0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX } }
      };
    static constexpr std::array<VkVertexInputAttributeDescription, 1> const
      inputAttributeDescription{ { { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 } } };

  protected:
    static constexpr std::array<VkDescriptorSetLayoutBinding, 0> const
      descriptorBindings{};
};

#endif // VK_MANDELBROT_VULKANMANDELBROTPIPELINEDESCRIPTION_HPP
