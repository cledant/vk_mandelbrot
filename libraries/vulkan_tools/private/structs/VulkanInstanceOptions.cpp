#include "common/VulkanInstanceOptions.hpp"

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
    if (shaderFloat64 &&
        shaderFloat64 != physicalDeviceSupportedOpt.shaderFloat64) {
        return (false);
    }
    return (true);
}
