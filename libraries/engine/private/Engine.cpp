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
      _ioManager.createVulkanSurface(_vkRenderer.getVkInstance()),
      VulkanInstance::DEFAULT_INSTANCE_OPTIONS);

    auto fb_size = _ioManager.getFramebufferSize();
    _vkRenderer.init(
      _ioManager.createVulkanSurface(_vkRenderer.getVkInstance()),
      VulkanRenderer::DEFAULT_RENDERER_OPTIONS,
      fb_size.x,
      fb_size.y);

    _eventHandler.setIOManager(&_ioManager);
    _eventHandler.setVkRenderer(&_vkRenderer);
    _eventHandler.setUi(&_ui);
}

void
Engine::run()
{
    while (!_ioManager.shouldClose()) {
        _eventHandler.processEvents();
        _ui.draw();
        _vkRenderer.draw();
    }
    _vkRenderer.clear();
    _ui.clear();
    _ioManager.deleteWindow();
}