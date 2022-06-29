#include "surfaceDisplay/VulkanSurfaceDisplayPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"
#include "constants/VulkanConstants.hpp"

void
VulkanSurfaceDisplayPipelineDescription::implInit(VulkanDevices const &devices)
{
    pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(mandelbrotConstants);
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout =
      createPipelineLayout(devices, descriptorSetLayout, pushConstantRange);
}

void
VulkanSurfaceDisplayPipelineDescription::implClear()
{
}
