#include "ui/VulkanUiPipeline.hpp"

#include <stdexcept>

#include "backends/imgui_impl_vulkan.h"

#include "utils/VulkanCommandBuffer.hpp"

void
VulkanUiPipeline::init(VulkanInstance const &vkInstance,
                       VulkanUiOffscreenRenderPass const &renderPass,
                       uint32_t nbImgSwapchain)
{
    _instance = vkInstance.instance;
    _devices = vkInstance.devices;
    _queues = vkInstance.queues;
    _cmdPools = vkInstance.cmdPools;

    initImgui(renderPass, nbImgSwapchain);
    loadFonts();
}

void
VulkanUiPipeline::resize(VulkanUiOffscreenRenderPass const &renderPass,
                         uint32_t nbImgSwapchain)
{
    vkDeviceWaitIdle(_devices.device);
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(_devices.device, _descriptorPool, nullptr);
    initImgui(renderPass, nbImgSwapchain);
    loadFonts();
}

void
VulkanUiPipeline::clear()
{
    vkDeviceWaitIdle(_devices.device);
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(_devices.device, _descriptorPool, nullptr);
    _instance = nullptr;
    _devices = VulkanDevices{};
    _queues = VulkanQueues{};
}

void
VulkanUiPipeline::generateCommands(VkCommandBuffer cmdBuffer)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void
VulkanUiPipeline::initImgui(VulkanUiOffscreenRenderPass const &renderPass,
                            uint32_t nbImgSwapchain)
{
    ImGui_ImplVulkan_InitInfo init_info = {};
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;
    if (vkCreateDescriptorPool(
          _devices.device, &pool_info, nullptr, &_descriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanUiPipeline: failed to create descriptor pool");
    }
    init_info.DescriptorPool = _descriptorPool;
    init_info.Instance = _instance;
    init_info.PhysicalDevice = _devices.physicalDevice;
    init_info.Device = _devices.device;
    init_info.QueueFamily = _queues.graphicFamilyIndex;
    init_info.Queue = _queues.graphicQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.MinImageCount = 2;
    init_info.ImageCount = nbImgSwapchain;
    init_info.CheckVkResultFn = [](VkResult err) {
        if (err != VK_SUCCESS) {
            throw std::runtime_error("Imgui: Vulkan operation failed");
        }
    };
    ImGui_ImplVulkan_Init(&init_info, renderPass.renderPass);
}

void
VulkanUiPipeline::loadFonts() const
{
    auto cmd_buffer =
      beginSingleTimeCommands(_devices.device, _cmdPools.renderCommandPool);
    ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
    endSingleTimeCommands(_devices.device,
                          _cmdPools.renderCommandPool,
                          cmd_buffer,
                          _queues.graphicQueue);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}