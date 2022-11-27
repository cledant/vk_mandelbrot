#include "common/VulkanCommonStruct.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <array>

#include "utils/VulkanTextureUtils.hpp"

void
VulkanTextureStaging::allocate(VulkanDevices const &devices,
                               int32_t texW,
                               int32_t texH,
                               int32_t nbChan,
                               bool cubemap)
{
    width = texW;
    height = texH;
    isCubemap = cubemap;
    VkDeviceSize img_size = texW * texH * nbChan;
    if (cubemap) {
        img_size *= 6;
    }

    stagingBuffer.allocate(devices,
                           img_size,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(devices.device, stagingBuffer.memory, 0, img_size, 0, &data);
    memset(data, 0, static_cast<size_t>(img_size));
    vkUnmapMemory(devices.device, stagingBuffer.memory);
    mipLevel = computeMipmapLevel(texW, texH);
    nbChannel = nbChan;
}

void
VulkanTextureStaging::stageTexture(VulkanDevices const &devices,
                                   std::string const &filepath)
{
    int img_chan;
    auto pixels =
      stbi_load(filepath.c_str(), &width, &height, &img_chan, STBI_rgb_alpha);
    if (!pixels) {
        throw std::runtime_error(
          "VulkanTextureStaging: failed to load image: " + filepath);
    }
    VkDeviceSize img_size = width * height * 4;

    stagingBuffer.allocate(devices,
                           img_size,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(devices.device, stagingBuffer.memory, 0, img_size, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(img_size));
    vkUnmapMemory(devices.device, stagingBuffer.memory);
    stbi_image_free(pixels);
    mipLevel = computeMipmapLevel(width, height);
    nbChannel = img_chan;
}

void
VulkanTextureStaging::stageTexture(VulkanDevices const &devices,
                                   std::string const &cubemapFolder,
                                   std::string const &filetype)
{
    static std::array<std::string, 6> const faces_name = {
        "front", "back", "top", "bottom", "right", "left",
    };
    std::array<uint8_t *, 6> faces_buffer{};

    uint8_t i = 0;
    int img_chan;
    for (auto const &it : faces_name) {
        auto fullpath = cubemapFolder;
        fullpath += "/";
        fullpath += it;
        fullpath += ".";
        fullpath += filetype;
        faces_buffer[i] = stbi_load(
          fullpath.c_str(), &width, &height, &img_chan, STBI_rgb_alpha);
        if (!faces_buffer[i]) {
            for (auto it_clean : faces_buffer) {
                if (it_clean) {
                    stbi_image_free(it_clean);
                    it_clean = nullptr;
                }
            }
            throw std::runtime_error(
              "VulkanTextureStaging: failed to load cubemap: " + fullpath);
        }
        ++i;
    }

    VkDeviceSize layer_size = width * height * 4;
    VkDeviceSize img_size = layer_size * 6;

    stagingBuffer.allocate(devices,
                           img_size,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(devices.device, stagingBuffer.memory, 0, img_size, 0, &data);
    i = 0;
    for (auto &it : faces_buffer) {
        memcpy(static_cast<uint8_t *>(data) + (layer_size * i),
               it,
               static_cast<size_t>(layer_size));
        stbi_image_free(it);
        ++i;
    }
    vkUnmapMemory(devices.device, stagingBuffer.memory);
    isCubemap = true;
    mipLevel = computeMipmapLevel(width, height);
    nbChannel = img_chan;
}

void
VulkanTextureStaging::stageTexture(VulkanDevices const &devices,
                                   uint8_t const *buff,
                                   int32_t texW,
                                   int32_t texH,
                                   int32_t nbChan,
                                   bool cubemap)
{
    width = texW;
    height = texH;
    isCubemap = cubemap;
    VkDeviceSize img_size = texW * texH * nbChan;
    if (cubemap) {
        img_size *= 6;
    }

    stagingBuffer.allocate(devices,
                           img_size,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(devices.device, stagingBuffer.memory, 0, img_size, 0, &data);
    memcpy(data, buff, static_cast<size_t>(img_size));
    vkUnmapMemory(devices.device, stagingBuffer.memory);
    mipLevel = computeMipmapLevel(texW, texH);
    nbChannel = nbChan;
}

void
VulkanTextureStaging::clear()
{
    stagingBuffer.clear();
    width = 0;
    height = 0;
    mipLevel = 0;
    isCubemap = false;
}

bool
VulkanTextureStaging::saveTextureToFile(VulkanDevices const &devices,
                                        std::string const &filepath) const
{
    // TODO: actual copy on disk
    (void)devices;
    (void)filepath;
    return (true);
}
