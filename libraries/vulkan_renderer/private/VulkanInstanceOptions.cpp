#include "VulkanInstanceOptions.hpp"

bool
VulkanInstanceOptions::hasRequiredOptions(
  VulkanInstanceOptions const &physicalDeviceSupportedOpt) const
{
    if (geometryShader &&
        geometryShader != physicalDeviceSupportedOpt.geometryShader) {
        return (false);
    }
    if (samplerAniso &&
        samplerAniso != physicalDeviceSupportedOpt.samplerAniso) {
        return (false);
    }
    if (fillModeNonSolid &&
        fillModeNonSolid != physicalDeviceSupportedOpt.fillModeNonSolid) {
        return (false);
    }
    if (shaderDouble &&
        shaderDouble != physicalDeviceSupportedOpt.shaderDouble) {
        return (false);
    }
    return (true);
}
