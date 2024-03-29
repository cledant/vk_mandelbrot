#include "VulkanSync.hpp"

#include <stdexcept>

void
VulkanSync::init(VulkanInstance const &vkInstance)
{
    _devices = vkInstance.devices;
    imageAvailableSem.resize(MAX_FRAME_INFLIGHT);
    allRenderFinishedSem.resize(MAX_FRAME_INFLIGHT);
    inflightFence.resize(MAX_FRAME_INFLIGHT);

    VkSemaphoreCreateInfo sem_info{};
    sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAME_INFLIGHT; ++i) {
        if (vkCreateSemaphore(
              _devices.device, &sem_info, nullptr, &imageAvailableSem[i]) !=
              VK_SUCCESS ||
            vkCreateSemaphore(
              _devices.device, &sem_info, nullptr, &allRenderFinishedSem[i]) !=
              VK_SUCCESS ||
            vkCreateFence(
              _devices.device, &fence_info, nullptr, &inflightFence[i]) !=
              VK_SUCCESS) {
            throw std::runtime_error("VulkanSync: failed to create semaphores");
        }
    }
}

void
VulkanSync::clear()
{
    for (size_t i = 0; i < MAX_FRAME_INFLIGHT; ++i) {
        vkDestroySemaphore(_devices.device, imageAvailableSem[i], nullptr);
        vkDestroySemaphore(_devices.device, allRenderFinishedSem[i], nullptr);
        vkDestroyFence(_devices.device, inflightFence[i], nullptr);
    }
    _devices = VulkanDevices{};
    imageAvailableSem.clear();
    allRenderFinishedSem.clear();
    inflightFence.clear();
    currentFrame = 0;
}