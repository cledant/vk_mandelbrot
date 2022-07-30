#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"

#include <functional>

void
EventHandler::setIOManager(IOManager *io_manager)
{
    _ioManager = io_manager;
}

void
EventHandler::setVkRenderer(VulkanRenderer *renderer)
{
    _renderer = renderer;
}

void
EventHandler::processEvents(IOEvents const &ioEvents)
{
    assert(_ioManager);
    assert(_renderer);

    // Resetting movement tracking
    _movements = glm::ivec3(0);

    static const std::array<void (EventHandler::*)(), IOET_NB>
      keyboard_events = { &EventHandler::_closeWinEvent,
                          &EventHandler::_toggleFullscreen,
                          &EventHandler::_up,
                          &EventHandler::_down,
                          &EventHandler::_right,
                          &EventHandler::_left,
                          &EventHandler::_setScreenCenter,
                          &EventHandler::_resetZoomScreenCenter,
                          &EventHandler::_singleIncIter,
                          &EventHandler::_singleDecIter,
                          &EventHandler::_multipleIncIter,
                          &EventHandler::_multipleDecIter,
                          &EventHandler::_resetIter };

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
    if (_ioManager->wasResized()) {
        // VK Renderer related
        auto fb_size = _ioManager->getFramebufferSize();
        _renderer->resize(fb_size.x, fb_size.y);
    }

    // Setting timers origin
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        if (_timers.updated[i]) {
            _timers.time_ref[i] = now;
        }
        _timers.updated[i] = 0;
    }
}

void
EventHandler::_closeWinEvent()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ioManager->triggerClose();
        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::_toggleFullscreen()
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
EventHandler::_setScreenCenter()
{
    if (_timers.accept_event[ET_LEFT_MOUSE]) {
        _timers.accept_event[ET_LEFT_MOUSE] = 0;
        _timers.updated[ET_LEFT_MOUSE] = 1;
    }
}

void
EventHandler::_resetZoomScreenCenter()
{
    if (_timers.accept_event[ET_RIGHT_MOUSE]) {
        _timers.accept_event[ET_RIGHT_MOUSE] = 0;
        _timers.updated[ET_RIGHT_MOUSE] = 1;
    }
}

void
EventHandler::_singleIncIter()
{
    if (_timers.accept_event[ET_KEYBOARD_CONTROLS]) {
        ++_renderer->mandelbrotConstants.maxIter;
        _renderer->mandelbrotComputeDone = false;
        _timers.accept_event[ET_KEYBOARD_CONTROLS] = 0;
        _timers.updated[ET_KEYBOARD_CONTROLS] = 1;
    }
}

void
EventHandler::_singleDecIter()
{
    if (_timers.accept_event[ET_KEYBOARD_CONTROLS] &&
        _renderer->mandelbrotConstants.maxIter) {
        --_renderer->mandelbrotConstants.maxIter;
        _renderer->mandelbrotComputeDone = false;
        _timers.accept_event[ET_KEYBOARD_CONTROLS] = 0;
        _timers.updated[ET_KEYBOARD_CONTROLS] = 1;
    }
}

void
EventHandler::_multipleIncIter()
{
    if (_timers.accept_event[ET_KEYBOARD_CONTROLS]) {
        _renderer->mandelbrotConstants.maxIter += MULTI_ITER_INC_DEC_VALUE;
        _renderer->mandelbrotComputeDone = false;
        _timers.accept_event[ET_KEYBOARD_CONTROLS] = 0;
        _timers.updated[ET_KEYBOARD_CONTROLS] = 1;
    }
}

void
EventHandler::_multipleDecIter()
{
    if (_timers.accept_event[ET_KEYBOARD_CONTROLS]) {
        _renderer->mandelbrotConstants.maxIter =
          (_renderer->mandelbrotConstants.maxIter <= MULTI_ITER_INC_DEC_VALUE)
            ? 1
            : _renderer->mandelbrotConstants.maxIter - MULTI_ITER_INC_DEC_VALUE;
        _renderer->mandelbrotComputeDone = false;
        _timers.accept_event[ET_KEYBOARD_CONTROLS] = 0;
        _timers.updated[ET_KEYBOARD_CONTROLS] = 1;
    }
}

void
EventHandler::_resetIter()
{
    if (_timers.accept_event[ET_KEYBOARD_CONTROLS]) {
        _renderer->mandelbrotConstants.maxIter =
          mandelbrotPushConstants::DEFAULT_MAX_ITER;
        _renderer->mandelbrotComputeDone = false;
        _timers.accept_event[ET_KEYBOARD_CONTROLS] = 0;
        _timers.updated[ET_KEYBOARD_CONTROLS] = 1;
    }
}
