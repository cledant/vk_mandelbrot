#include "mandelbrot/VulkanMandelbrotPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"
#include "constants/VulkanConstants.hpp"

void
VulkanMandelbrotPipelineDescription::implInit(VulkanDevices const &devices)
{
    pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(mandelbrotPushConstants);
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout =
      createPipelineLayout(devices, descriptorSetLayout, pushConstantRange);
}

void
VulkanMandelbrotPipelineDescription::implClear()
{
}
