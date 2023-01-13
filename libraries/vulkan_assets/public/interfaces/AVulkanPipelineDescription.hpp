#ifndef VK_MANDELBROT_AVULKANPIPELINEDESCRIPTION_HPP
#define VK_MANDELBROT_AVULKANPIPELINEDESCRIPTION_HPP

#include "structs/VulkanBasicStructs.hpp"

template<class Child>
class AVulkanPipelineDescription
{
  public:
    AVulkanPipelineDescription() = default;
    virtual ~AVulkanPipelineDescription() = default;
    AVulkanPipelineDescription(AVulkanPipelineDescription const &src) = default;
    AVulkanPipelineDescription &operator=(
      AVulkanPipelineDescription const &rhs) = default;
    AVulkanPipelineDescription(AVulkanPipelineDescription &&src) noexcept =
      default;
    AVulkanPipelineDescription &operator=(
      AVulkanPipelineDescription &&rhs) noexcept = default;

    void init(VulkanDevices const &devices);
    void clear();

    VkDescriptorSetLayout descriptorSetLayout{};
    VkPushConstantRange pushConstantRange{};
    VkPipelineLayout pipelineLayout{};

  protected:
    VulkanDevices _devices;
};

template<class Child>
void
AVulkanPipelineDescription<Child>::init(VulkanDevices const &devices)
{
    _devices = devices;
    static_cast<Child &>(*this).implInit(devices);
}

template<class Child>
void
AVulkanPipelineDescription<Child>::clear()
{
    static_cast<Child &>(*this).implClear();
    vkDestroyPipelineLayout(_devices.device, pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(_devices.device, descriptorSetLayout, nullptr);
    descriptorSetLayout = nullptr;
    pipelineLayout = nullptr;
    pushConstantRange = {};
}

#endif // VK_MANDELBROT_AVULKANPIPELINEDESCRIPTION_HPP
