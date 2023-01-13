#include "VulkanSwapChain.hpp"

#include <stdexcept>

#include "utils/VulkanImageUtils.hpp"
#include "utils/VulkanSwapChainUtils.hpp"
#include "utils/VulkanPhysicalDeviceUtils.hpp"

void
VulkanSwapChain::init(VulkanInstance const &vkInstance,
                      uint32_t fbW,
                      uint32_t fbH,
                      bool vsync)
{
    _devices = vkInstance.devices;
    _surface = vkInstance.surface;
    createSwapChain(fbW, fbH, vsync);
    createImageView();
}

void
VulkanSwapChain::resize(uint32_t fbW, uint32_t fbH, bool vsync)
{
    clean();
    createSwapChain(fbW, fbH, vsync);
    createImageView();
}

void
VulkanSwapChain::clean()
{
    vkDeviceWaitIdle(_devices.device);

    for (auto iv : swapChainImageViews) {
        iv.clearSwapchainTexture();
    }
    vkDestroySwapchainKHR(_devices.device, swapChain, nullptr);
}

void
VulkanSwapChain::clear()
{
    clean();
    _devices = VulkanDevices{};
    _surface = nullptr;
    currentSwapChainNbImg = 0;
    swapChain = nullptr;
    swapChainImageFormat = {};
    swapChainExtent = {};
    swapChainImages.clear();
    swapChainImageViews.clear();
}

void
VulkanSwapChain::createSwapChain(uint32_t framebufferW,
                                 uint32_t framebufferH,
                                 bool vsync)
{
    // Creating swap chain
    VkExtent2D actual_extent = { framebufferW, framebufferH };

    auto scs =
      getSwapChainSupport(_devices.physicalDevice, _surface, actual_extent);
    if (!scs.isValid()) {
        throw std::runtime_error("VulkanRenderPass: SwapChain error");
    }

    uint32_t nb_img = scs.capabilities.minImageCount + 1;
    if (scs.capabilities.maxImageCount > 0 &&
        nb_img > scs.capabilities.maxImageCount) {
        nb_img = scs.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = _surface;
    create_info.minImageCount = nb_img;
    create_info.imageFormat = scs.surface_format.value().format;
    create_info.imageColorSpace = scs.surface_format.value().colorSpace;
    create_info.imageExtent = scs.extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    DeviceRequirement dr{};
    getDeviceQueues(_devices.physicalDevice, _surface, dr);
    uint32_t queue_family_indices[] = { dr.presentFamilyIndex.value(),
                                        dr.graphicFamilyIndex.value() };
    if (dr.presentFamilyIndex.value() != dr.graphicFamilyIndex.value()) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }
    create_info.preTransform = scs.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (!vsync) {
        create_info.presentMode = scs.present_mode.value();
    } else {
        create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = nullptr;
    if (vkCreateSwapchainKHR(
          _devices.device, &create_info, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderPass: Failed to create swap chain");
    }

    // Retrieving img buffer + keeping info
    uint32_t nb_img_sc;
    vkGetSwapchainImagesKHR(_devices.device, swapChain, &nb_img_sc, nullptr);
    swapChainImages.resize(nb_img_sc);
    currentSwapChainNbImg = nb_img_sc;
    vkGetSwapchainImagesKHR(
      _devices.device, swapChain, &nb_img_sc, swapChainImages.data());
    swapChainExtent = scs.extent;
    swapChainImageFormat = scs.surface_format.value().format;
}

void
VulkanSwapChain::createImageView()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); ++i) {
        swapChainImageViews[i].createSwapchainTexture(
          _devices, swapChainImages[i], swapChainImageFormat, swapChainExtent);
    }
}
