#ifndef VK_MANDELBROT_VULKANSHADERUTILS_HPP
#define VK_MANDELBROT_VULKANSHADERUTILS_HPP

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

std::vector<char> readFileContent(std::string const &filepath);
VkShaderModule createShaderModule(VkDevice device,
                                  std::vector<char> const &shaderData,
                                  std::string const &shaderName);
VkShaderModule loadShader(VkDevice device, std::string const &filepath);

#endif // VK_MANDELBROT_VULKANSHADERUTILS_HPP
