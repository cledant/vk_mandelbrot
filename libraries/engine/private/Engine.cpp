#include "Engine.hpp"

#include <cstring>

#include "AppInfo.hpp"

void
Engine::init()
{
    char engine_name[128] = { 0 };
    std::strcat(engine_name, app_info::APP_NAME);
    std::strcat(engine_name, "_engine");
    IOManagerWindowCreationOption win_opts{
        false, false, false, false, DEFAULT_WIN_SIZE, app_info::APP_NAME
    };

    _ioManager.createWindow(std::move(win_opts));
    _ui.init(_ioManager.getWindow());
    _vkInstance.createInstance(app_info::APP_NAME,
                               engine_name,
                               VK_MAKE_VERSION(app_info::APP_VERSION_MAJOR,
                                               app_info::APP_VERSION_MINOR,
                                               app_info::APP_VERSION_PATCH),
                               VK_MAKE_VERSION(app_info::APP_VERSION_MAJOR,
                                               app_info::APP_VERSION_MINOR,
                                               app_info::APP_VERSION_PATCH),
                               IOManager::getRequiredInstanceExtension());
    _vkInstance.createResources(
      _ioManager.createVulkanSurface(_vkInstance.instance),
      VulkanInstance::DEFAULT_INSTANCE_OPTIONS);
    auto fb_size = _ioManager.getFramebufferSize();
    _vkRenderer.init(_vkInstance,
                     fb_size.x,
                     fb_size.y,
                     VulkanInstance::DEFAULT_INSTANCE_OPTIONS.vsync);
    _gfxAssets.init(_vkInstance,
                    _vkRenderer.swapChain.swapChainExtent.width,
                    _vkRenderer.swapChain.swapChainExtent.height,
                    _vkRenderer.swapChain.swapChainImageViews,
                    _vkRenderer.swapChain.swapChainImageFormat,
                    VulkanInstance::DEFAULT_INSTANCE_OPTIONS.rendererScale);

    _eventHandler.setIOManager(&_ioManager);
    _eventHandler.setVkRenderer(&_vkRenderer);
    _eventHandler.setGfxAssets(&_gfxAssets);
    _eventHandler.setUi(&_ui);
}

void
Engine::run()
{
    VkCommandBuffer cmdBuffer{};
    uint32_t imgIndex{};

    while (!_ioManager.shouldClose()) {
        _eventHandler.processEvents();
        _ui.draw();
        _vkRenderer.acquireImage(cmdBuffer, imgIndex);
        _gfxAssets.recordDrawCmds(cmdBuffer,
                                  imgIndex,
                                  VulkanRenderer::DEFAULT_CLEAR_COLOR,
                                  VulkanRenderer::DEFAULT_CLEAR_DEPTH_STENCIL);
        _vkRenderer.presentImage(
          imgIndex, _gfxAssets.imageMandelbrot, _gfxAssets.capturedFrame);
    }
    _gfxAssets.clear();
    _vkRenderer.clear();
    _vkInstance.clear();
    _ui.clear();
    _ioManager.deleteWindow();
}