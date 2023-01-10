#include "GfxAssets.hpp"

void
GfxAssets::init(VulkanInstance vkInstance,
                uint32_t winW,
                uint32_t winH,
                std::vector<VulkanTexture> const &swapChainImageViews,
                VkFormat swapChainImageFormat,
                float scale)
{
    _devices = vkInstance.devices;
    _depthFormat = vkInstance.depthFormat;

    // Textures
    int32_t rendererW = winW * scale;
    int32_t rendererH = winH * scale;
    _imageMandelbrot.init(
      vkInstance, VK_FORMAT_R8G8B8A8_UNORM, _depthFormat, rendererW, rendererH);
    _capturedFrame.allocate(_devices, rendererW, rendererH, 4, false);

    // Render Passes + pipelines
    _toScreenRenderPass.init(
      vkInstance, swapChainImageViews, swapChainImageFormat, winW, winH);
    _toScreen.init(vkInstance,
                   swapChainImageViews.size(),
                   winW,
                   winH,
                   _toScreenRenderPass.renderPass,
                   _imageMandelbrot.descriptorImage);

    _mandelbrotFirstRenderPass.init(vkInstance,
                                    VK_FORMAT_R8G8B8A8_UNORM,
                                    _depthFormat,
                                    _imageMandelbrot.colorTex.textureImgView,
                                    _imageMandelbrot.depthTex.textureImgView,
                                    rendererW,
                                    rendererH);
    _mandelbrotFirst.init(
      vkInstance, _mandelbrotFirstRenderPass.renderPass, rendererW, rendererH);

    _mandelbrotMultipleRenderPass.init(vkInstance,
                                       VK_FORMAT_R8G8B8A8_UNORM,
                                       _depthFormat,
                                       _imageMandelbrot.colorTex.textureImgView,
                                       _imageMandelbrot.depthTex.textureImgView,
                                       rendererW,
                                       rendererH);
    _mandelbrotMultiple.init(vkInstance,
                             _mandelbrotMultipleRenderPass.renderPass,
                             rendererW,
                             rendererH);

    _uiRenderPass.init(
      vkInstance, swapChainImageViews, swapChainImageFormat, winW, winH);
    _ui.init(vkInstance, _uiRenderPass.renderPass, swapChainImageViews.size());
}

void
GfxAssets::resize(uint32_t winW,
                  uint32_t winH,
                  std::vector<VulkanTexture> const &swapChainImageViews,
                  VkFormat swapChainImageFormat,
                  float scale)
{
    vkDeviceWaitIdle(_devices.device);
    if (winW <= 0 || winH <= 0 || scale <= 0.0f) {
        return;
    }

    // Textures
    int32_t rendererW = winW * scale;
    int32_t rendererH = winH * scale;
    _imageMandelbrot.resize(
      VK_FORMAT_R8G8B8A8_UNORM, _depthFormat, rendererW, rendererH);
    _capturedFrame.clear();
    _capturedFrame.allocate(_devices, rendererW, rendererH, 4, false);

    // Render passes + pipelines
    _toScreenRenderPass.resize(
      swapChainImageViews, swapChainImageFormat, winW, winH);
    _toScreen.resize(swapChainImageViews.size(),
                     winW,
                     winH,
                     _toScreenRenderPass.renderPass,
                     _imageMandelbrot.descriptorImage);

    _mandelbrotFirstRenderPass.resize(VK_FORMAT_R8G8B8A8_UNORM,
                                      _depthFormat,
                                      _imageMandelbrot.colorTex.textureImgView,
                                      _imageMandelbrot.depthTex.textureImgView,
                                      rendererW,
                                      rendererH);
    _mandelbrotFirst.resize(
      _mandelbrotFirstRenderPass.renderPass, rendererW, rendererH);

    _mandelbrotMultipleRenderPass.resize(
      VK_FORMAT_R8G8B8A8_UNORM,
      _depthFormat,
      _imageMandelbrot.colorTex.textureImgView,
      _imageMandelbrot.depthTex.textureImgView,
      rendererW,
      rendererH);
    _mandelbrotMultiple.resize(
      _mandelbrotMultipleRenderPass.renderPass, rendererW, rendererH);

    _uiRenderPass.resize(swapChainImageViews, swapChainImageFormat, winW, winH);
    _ui.resize(_uiRenderPass.renderPass, swapChainImageViews.size());
}

void
GfxAssets::clear()
{
    vkDeviceWaitIdle(_devices.device);
    _ui.clear();
    _uiRenderPass.clear();
    _mandelbrotFirst.clear();
    _mandelbrotFirstRenderPass.clear();
    _mandelbrotMultiple.clear();
    _mandelbrotMultipleRenderPass.clear();
    _toScreen.clear();
    _toScreenRenderPass.clear();
    _capturedFrame.clear();
    _imageMandelbrot.clear();
    _devices = {};
    _depthFormat = {};
}
