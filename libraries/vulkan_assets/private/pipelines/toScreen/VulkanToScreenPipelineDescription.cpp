#include "pipelines/toScreen/VulkanToScreenPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"
#include "pipelines/mandelbrot/MandelbrotPushConstants.hpp"

void
VulkanToScreenPipelineDescription::implInit(VulkanDevices const &devices)
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
VulkanToScreenPipelineDescription::implClear()
{
}
