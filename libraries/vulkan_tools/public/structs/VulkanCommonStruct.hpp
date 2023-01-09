#ifndef VK_MANDELBROT_VULKANCOMMONSTRUCT_HPP
#define VK_MANDELBROT_VULKANCOMMONSTRUCT_HPP

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"

struct VulkanDevices final
{
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
};

struct VulkanCommandPools final
{
    VkCommandPool renderCommandPool{};
};

struct VulkanQueues final
{
    VkQueue graphicQueue{};
    VkQueue presentQueue{};
    uint32_t graphicFamilyIndex{};
    uint32_t presentFamilyIndex{};
};

struct VulkanBuffer final
{
    VkBuffer buffer{};
    VkDeviceMemory memory{};
    VkDeviceSize size{};

    void allocate(VulkanDevices const &devices,
                  VkDeviceSize bufferSize,
                  VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties);
    void clear();

  private:
    VulkanDevices _devices{};

    friend struct VulkanTextureStaging;
};

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
    void clear();
};

struct VulkanTexture final
{
    VkImage textureImg{};
    VkDeviceMemory textureImgMemory{};
    VkImageView textureImgView{};
    VkSampler textureSampler{};
    int32_t width{};
    int32_t height{};
    uint32_t mipLevel = 1;
    VkFormat textureFormat{};
    bool isCubemap{};

    void loadTextureOnGPU(VulkanDevices const &devices,
                          VulkanCommandPools const &cmdPools,
                          VulkanQueues const &queues,
                          VulkanTextureStaging const &stagingTexture,
                          VkFormat format);
    void loadTextureOnCPU(VulkanCommandPools const &cmdPools,
                          VulkanQueues const &queues,
                          VulkanTextureStaging const &stagingTexture) const;
    void createColorTexture(VulkanDevices const &devices,
                            int32_t texW,
                            int32_t texH,
                            VkFormat depthFormat);
    void createDepthTexture(VulkanDevices const &devices,
                            VulkanCommandPools const &cmdPools,
                            VulkanQueues const &queues,
                            int32_t texW,
                            int32_t texH,
                            VkFormat depthFormat);
    void createSwapchainTexture(VulkanDevices const &devices,
                                VkImage swapchainImg,
                                VkFormat swapchainFormat,
                                VkExtent2D extent);
    void clear();
    void clearSwapchainTexture();

  private:
    VulkanDevices _devices;
};

struct VulkanSimpleVertex final
{
    glm::vec3 position{};
    glm::vec2 texCoord{};
};

struct VulkanScreenshot final
{
    std::unique_ptr<uint8_t[]> data;
    int32_t width{};
    int32_t height{};
    uint32_t nbChannel{};

    [[nodiscard]] bool saveScreenshotToFile(std::string const &filepath) const;
};

#endif // VK_MANDELBROT_VULKANCOMMONSTRUCT_HPP
