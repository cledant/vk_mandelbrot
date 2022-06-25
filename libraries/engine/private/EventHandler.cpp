#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"

#include <functional>

void
EventHandler::setIOManager(IOManager *io_manager)
{
    _ioManager = io_manager;
}

/*void
EventHandler::setVkRenderer(VulkanRenderer *renderer)
{
    _renderer = renderer;
}*/

void
EventHandler::processEvents(IOEvents const &ioEvents)
{
    assert(_ioManager);
    // assert(_renderer);

    // Resetting movement tracking
    _movements = glm::ivec3(0);

    static const std::array<void (EventHandler::*)(), IOET_NB>
      keyboard_events = {
          &EventHandler::_mouse_exclusive,
          &EventHandler::_close_win_event,
          &EventHandler::_toggle_fullscreen,
          &EventHandler::_up,
          &EventHandler::_down,
          &EventHandler::_right,
          &EventHandler::_left,
          &EventHandler::_left_mouse,
          &EventHandler::_middle_mouse,
          &EventHandler::_right_mouse,
      };

    // Checking Timers
    auto now = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        std::chrono::duration<double> time_diff = now - _timers.time_ref[i];
        _timers.timer_diff[i] = time_diff.count();
        _timers.accept_event[i] = (time_diff.count() > _timers.timer_values[i]);
    }

    // Looping over io events types
    for (uint32_t i = 0; i < IOET_NB; ++i) {
        if (ioEvents.events[i]) {
            std::invoke(keyboard_events[i], this);
        }
    }

    // Resized window case
    /*    if (_io_manager->wasResized()) {
            // VK Renderer related
            auto fb_size = _io_manager->getFramebufferSize();
            _renderer->resize(fb_size.x, fb_size.y);
        }*/

    // Setting timers origin
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        if (_timers.updated[i]) {
            _timers.time_ref[i] = now;
        }
        _timers.updated[i] = 0;
    }
}

void
EventHandler::_mouse_exclusive()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ioManager->toggleMouseExclusive();
        _mousePosSkip = true;
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_close_win_event()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ioManager->triggerClose();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_toggle_fullscreen()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ioManager->toggleFullscreen();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_up()
{
    _movements.y += 1;
}

void
EventHandler::_down()
{
    _movements.y -= 1;
}

void
EventHandler::_right()
{
    _movements.x += 1;
}

void
EventHandler::_left()
{
    _movements.x -= 1;
}

void
EventHandler::_left_mouse()
{
    _timers.updated[ET_LEFT_MOUSE] = 1;
}

void
EventHandler::_middle_mouse()
{
    if (_timers.accept_event[ET_MIDDLE_MOUSE]) {
        _timers.accept_event[ET_MIDDLE_MOUSE] = 0;
        _timers.updated[ET_MIDDLE_MOUSE] = 1;
    }
}

void
EventHandler::_right_mouse()
{
    if (_timers.accept_event[ET_RIGHT_MOUSE]) {
        _timers.accept_event[ET_RIGHT_MOUSE] = 0;
        _timers.updated[ET_RIGHT_MOUSE] = 1;
    }
}
