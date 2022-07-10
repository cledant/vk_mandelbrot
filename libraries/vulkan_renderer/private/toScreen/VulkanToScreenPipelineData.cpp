#include "toScreen/VulkanToScreenPipelineData.hpp"

#include "utils/VulkanMemory.hpp"

void
VulkanToScreenPipelineData::init(VulkanDevices const &devices,
                                 VulkanCommandPools const &cmdPools,
                                 VulkanQueues const &queues,
                                 VkExtent2D const &screenSize)
{
    static constexpr std::array const SURFACE_DISPLAY_INDICES = { 0, 1, 2,
                                                                  2, 3, 0 };
    std::array SURFACE_DISPLAY_VERTICES = {
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(-1.0f, 1.0f, 0.0f),
    };
    forceSquareRatio(screenSize, SURFACE_DISPLAY_VERTICES);

    // Computing sizes and offsets
    indicesDrawNb = SURFACE_DISPLAY_INDICES.size();
    verticesSize = sizeof(glm::vec3) * SURFACE_DISPLAY_VERTICES.size();
    indicesSize = sizeof(uint32_t) * SURFACE_DISPLAY_INDICES.size();
    indicesOffset = verticesSize;
    VkDeviceSize total_size = verticesSize + indicesSize;

    // Creating transfer buffer CPU to GPU
    VulkanBuffer staging_buff{};
    staging_buff.allocate(devices,
                          total_size,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Copying data into staging buffer
    copyOnCpuCoherentMemory(devices.device,
                            staging_buff.memory,
                            0,
                            verticesSize,
                            SURFACE_DISPLAY_VERTICES.data());
    copyOnCpuCoherentMemory(devices.device,
                            staging_buff.memory,
                            indicesOffset,
                            indicesSize,
                            SURFACE_DISPLAY_INDICES.data());

    // Creating GPU buffer + copying transfer buffer
    data.allocate(devices,
                  total_size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copyBufferOnGpu(devices.device,
                    cmdPools.renderCommandPool,
                    queues.graphicQueue,
                    data.buffer,
                    staging_buff.buffer,
                    total_size);

    staging_buff.clear();
}

void
VulkanToScreenPipelineData::clear()
{
    data.clear();
    verticesSize = 0;
    indicesSize = 0;
    indicesOffset = 0;
    verticesSize = 0;
    indicesDrawNb = 0;
}

void
VulkanToScreenPipelineData::forceSquareRatio(VkExtent2D const &screenSize,
                                             std::array<glm::vec3, 4> &vertices)
{
    if (screenSize.height == screenSize.width) {
        return;
    }

    if (screenSize.width > screenSize.height) {
        glm::vec3 ratio{ static_cast<float>(screenSize.height) /
                           static_cast<float>(screenSize.width),
                         1.0f,
                         1.0f };

        for (auto &item : vertices) {
            item *= ratio;
        }
    } else {
        glm::vec3 ratio{ 1.0f,
                         static_cast<float>(screenSize.width) /
                           static_cast<float>(screenSize.height),
                         1.0f };

        for (auto &item : vertices) {
            item *= ratio;
        }
    }
}
