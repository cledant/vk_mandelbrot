#include "pipelines/toScreen/VulkanToScreenPipelineData.hpp"

#include "utils/VulkanMemory.hpp"

void
VulkanToScreenPipelineData::init(VulkanDevices const &devices,
                                 VulkanCommandPools const &cmdPools,
                                 VulkanQueues const &queues)
{
    static constexpr std::array const SURFACE_DISPLAY_INDICES = { 0, 1, 2,
                                                                  2, 3, 0 };
    static constexpr std::array const SURFACE_DISPLAY_VERTICES = {
        VulkanSimpleVertex{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
        VulkanSimpleVertex{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
        VulkanSimpleVertex{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        VulkanSimpleVertex{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } }
    };

    // Computing sizes and offsets
    indicesDrawNb = SURFACE_DISPLAY_INDICES.size();
    verticesSize = sizeof(VulkanSimpleVertex) * SURFACE_DISPLAY_VERTICES.size();
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
