#ifndef VK_MANDELBROT_VULKANTEXTURESTAGING_HPP
#define VK_MANDELBROT_VULKANTEXTURESTAGING_HPP

#include <vulkan/vulkan.h>

#include "structs/VulkanBuffer.hpp"
#include "structs/VulkanScreenshot.hpp"

struct VulkanTextureStaging final
{
    VulkanBuffer stagingBuffer{};
    int32_t width{};
    int32_t height{};
    uint32_t mipLevel{};
    uint32_t nbChannel{};
    bool isCubemap{};

    void allocate(VulkanDevices const &devices,
                  int32_t texW,
                  int32_t texH,
                  int32_t nbChan,
                  bool cubemap);
    void stageTexture(VulkanDevices const &devices,
                      std::string const &filepath);
    void stageTexture(VulkanDevices const &devices,
                      std::string const &cubemapFolder,
                      std::string const &filetype);
    void stageTexture(VulkanDevices const &devices,
                      uint8_t const *buff,
                      int32_t texW,
                      int32_t texH,
                      int32_t nbChan,
                      bool cubemap);
    [[nodiscard]] VulkanScreenshot generateScreenshot() const;
    void clear();
};

#endif // VK_MANDELBROT_VULKANTEXTURESTAGING_HPP
