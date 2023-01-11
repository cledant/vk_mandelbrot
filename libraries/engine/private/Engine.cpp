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
    _swapChain.init(_vkInstance,
                    fb_size.x,
                    fb_size.y,
                    VulkanInstance::DEFAULT_INSTANCE_OPTIONS.vsync);
    _vkRenderer.init(_vkInstance);
    _gfxAssets.init(_vkInstance,
                    _swapChain.swapChainExtent.width,
                    _swapChain.swapChainExtent.height,
                    _swapChain.swapChainImageViews,
                    _swapChain.swapChainImageFormat,
                    VulkanInstance::DEFAULT_INSTANCE_OPTIONS.rendererScale);

    _eventHandler.setIOManager(&_ioManager);
    _eventHandler.setSwapChain(&_swapChain);
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
        _vkRenderer.acquireImage(_swapChain.swapChain, cmdBuffer, imgIndex);
        _gfxAssets.recordDrawCmds(cmdBuffer,
                                  imgIndex,
                                  VulkanRenderer::DEFAULT_CLEAR_COLOR,
                                  VulkanRenderer::DEFAULT_CLEAR_DEPTH_STENCIL);
        _vkRenderer.presentImage(_swapChain.swapChain,
                                 _gfxAssets.imageMandelbrot,
                                 _gfxAssets.capturedFrame,
                                 imgIndex);
    }
    _gfxAssets.clear();
    _vkRenderer.clear();
    _swapChain.clear();
    _vkInstance.clear();
    _ui.clear();
    _ioManager.deleteWindow();
}