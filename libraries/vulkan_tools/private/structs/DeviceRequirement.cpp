#include "structs/DeviceRequirement.hpp"

bool
DeviceRequirement::isValid(VulkanInstanceOptions const &requiredOptions) const
{
    return (graphicFamilyIndex.has_value() && presentFamilyIndex.has_value() &&
            computeFamilyIndex.has_value() &&
            requiredOptions.hasRequiredOptions(options) &&
            allExtensionSupported);
}