#include "surfaceDisplay/VulkanSurfaceDisplayPipelineDescription.hpp"

#include "utils/VulkanDescriptorUtils.hpp"
#include "utils/VulkanPipelineUtils.hpp"

void
VulkanSurfaceDisplayPipelineDescription::implInit(VulkanDevices const &devices)
{
    descriptorSetLayout =
      createDescriptorSetLayout(devices, descriptorBindings);
    pipelineLayout = createPipelineLayout(devices, descriptorSetLayout);
}

void
VulkanSurfaceDisplayPipelineDescription::implClear()
{
}
