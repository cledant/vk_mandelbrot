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

    _event_handler.setIOManager(&_io_manager);
    /*    _event_handler.setVkRenderer(&_vk_renderer);*/
    _io_manager.createWindow(std::move(win_opts));
    /*    _vk_renderer.createInstance(app_info::APP_NAME,
                                    engine_name,
                                    VK_MAKE_VERSION(app_info::APP_VERSION_MAJOR,
                                                    app_info::APP_VERSION_MINOR,
                                                    app_info::APP_VERSION_PATCH),
                                    VK_MAKE_VERSION(app_info::APP_VERSION_MAJOR,
                                                    app_info::APP_VERSION_MINOR,
                                                    app_info::APP_VERSION_PATCH),
                                    IOManager::getRequiredInstanceExtension());*/
    /*    auto fb_size = _io_manager.getFramebufferSize();
        _vk_renderer.init(
          _io_manager.createVulkanSurface(_vk_renderer.getVkInstance()),
          fb_size.x,
          fb_size.y);*/
}

void
Engine::run()
{
    while (!_io_manager.shouldClose()) {
        _event_handler.processEvents(_io_manager.getEvents());
        /*        _vk_renderer.draw();*/
    }
    /*    _vk_renderer.clear();*/
    _io_manager.deleteWindow();
}