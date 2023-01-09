#ifndef VK_MANDELBROT_VULKANTEXTUREUTILS_HPP
#define VK_MANDELBROT_VULKANTEXTUREUTILS_HPP

#include <cmath>
#include <string>

#include <vulkan/vulkan.h>

#include "common/VulkanCommonStruct.hpp"

uint32_t constexpr computeMipmapLevel(int32_t texW, int32_t texH)
{
    return (static_cast<uint32_t>(std::floor(std::log2(std::max(texW, texH)))) +
            1);
}
void generateMipmapsSingleCmd(VulkanDevices const &devices,
                              VulkanCommandPools const &cmdPools,
                              VulkanQueues const &queues,
                              VulkanTexture const &texture);
void createTextureSampler(VulkanDevices const &devices, VulkanTexture &texture);
void loadTextureFromFile(VulkanDevices const &devices,
                         VulkanCommandPools const &cmdPools,
                         VulkanQueues const &queues,
                         std::string const &filePath,
                         VulkanTexture &toLoad);

#endif // VK_MANDELBROT_VULKANTEXTUREUTILS_HPP
