#ifndef VK_MANDELBROT_VULKAN_SURFACEDISPLAYPIPELINEDESCRIPTION_HPP
#define VK_MANDELBROT_VULKAN_SURFACEDISPLAYPIPELINEDESCRIPTION_HPP

#include <array>

#include "glm/glm.hpp"

#include "interface/AVulkanPipelineDescription.hpp"

class VulkanSurfaceDisplayPipelineDescription
  : public AVulkanPipelineDescription<VulkanSurfaceDisplayPipelineDescription>
{
  public:
    VulkanSurfaceDisplayPipelineDescription() = default;
    ~VulkanSurfaceDisplayPipelineDescription() override = default;
    VulkanSurfaceDisplayPipelineDescription(
      VulkanSurfaceDisplayPipelineDescription const &src) = default;
    VulkanSurfaceDisplayPipelineDescription &operator=(
      VulkanSurfaceDisplayPipelineDescription const &rhs) = default;
    VulkanSurfaceDisplayPipelineDescription(
      VulkanSurfaceDisplayPipelineDescription &&src) noexcept = default;
    VulkanSurfaceDisplayPipelineDescription &operator=(
      VulkanSurfaceDisplayPipelineDescription &&rhs) noexcept = default;

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

#endif // VK_MANDELBROT_VULKAN_SURFACEDISPLAYPIPELINEDESCRIPTION_HPP
