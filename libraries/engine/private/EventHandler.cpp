#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"

#include <functional>

void
EventHandler::setIOManager(IOManager *io_manager)
{
    _ioManager = io_manager;
    auto fbSize = _ioManager->getFramebufferSize();
    _screenRatio = static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y);
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

    auto fbSize = _ioManager->getFramebufferSize();

    // Resetting movement tracking
    _keyboardMvt = glm::ivec2(0);

    // Multiplier value handling
    _iterStepValue =
      (ioEvents.multiplier) ? ITER_WITH_MULTIPLIER : ITER_NO_MULTIPLIER;
    _zoomStepValue =
      (ioEvents.multiplier) ? ZOOM_WITH_MULTIPLIER : ZOOM_NO_MULTIPLIER;
    _keyboardMvtStepValue = (ioEvents.multiplier) ? KEYBOARD_MVT_WITH_MULTIPLIER
                                                  : KEYBOARD_MVT_NO_MULTIPLIER;

    // Zoom handling
    bool addOffsetZoomIn{};
    bool addOffsetZoomOut{};
    if (ioEvents.mouseScroll != 0.0f) {
        if (ioEvents.mouseScroll > 0.0f) {
            _zoomVal *= _zoomStepValue;
            addOffsetZoomIn = true;
        } else {
            _zoomVal /= _zoomStepValue;
            addOffsetZoomOut = true;
        }

        if (_zoomVal < 1.0f) {
            _zoomVal = 1.0f;
            addOffsetZoomOut = false;
        }

        _renderer->mandelbrotComputeDone = false;
        _ioManager->resetMouseScroll();
    }
    if (addOffsetZoomOut) {
        _renderer->mandelbrotConstants.offset -=
          computeMouseOffset(ioEvents.mousePosition, fbSize);
    }
    _renderer->mandelbrotConstants.zoom =
      mandelbrotPushConstants::DEFAULT_ZOOM / _zoomVal;
    _renderer->mandelbrotConstants.zoomMultScreenRatio =
      _renderer->mandelbrotConstants.zoom * _screenRatio;
    if (addOffsetZoomIn) {
        _renderer->mandelbrotConstants.offset +=
          computeMouseOffset(ioEvents.mousePosition, fbSize);
    }

    static const std::array<void (EventHandler::*)(), IOET_NB>
      keyboard_events = { &EventHandler::closeWinEvent,
                          &EventHandler::toggleFullscreen,
                          &EventHandler::up,
                          &EventHandler::down,
                          &EventHandler::right,
                          &EventHandler::left,
                          &EventHandler::resetZoomScreenCenter,
                          &EventHandler::incIter,
                          &EventHandler::decIter,
                          &EventHandler::resetIter };

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

    // Keyboard Mvt handling
    if (_keyboardMvt.x) {
        _renderer->mandelbrotConstants.offset.x +=
          _keyboardMvt.x * (_keyboardMvtStepValue / _zoomVal);
        _renderer->mandelbrotComputeDone = false;
    }
    if (_keyboardMvt.y) {
        _renderer->mandelbrotConstants.offset.y -=
          _keyboardMvt.y * (_keyboardMvtStepValue / _zoomVal);
        _renderer->mandelbrotComputeDone = false;
    }

    // Resized window case
    if (_ioManager->wasResized()) {
        fbSize = _ioManager->getFramebufferSize();
        _renderer->resize(fbSize.x, fbSize.y);
        _screenRatio =
          static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y);
        _renderer->mandelbrotComputeDone = false;
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
EventHandler::closeWinEvent()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ioManager->triggerClose();

        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::toggleFullscreen()
{
    if (_timers.accept_event[ET_SYSTEM]) {
        _ioManager->toggleFullscreen();

        _timers.accept_event[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::up()
{
    _keyboardMvt.y += 1;
}

void
EventHandler::down()
{
    _keyboardMvt.y -= 1;
}

void
EventHandler::right()
{
    _keyboardMvt.x += 1;
}

void
EventHandler::left()
{
    _keyboardMvt.x -= 1;
}

void
EventHandler::resetZoomScreenCenter()
{
    if (_timers.accept_event[ET_RIGHT_MOUSE]) {
        _keyboardMvt = glm::ivec2(0.0);
        _renderer->mandelbrotConstants.offset =
          mandelbrotPushConstants::DEFAULT_OFFSET;
        _zoomVal = DEFAULT_ZOOM;
        _renderer->mandelbrotConstants.zoom =
          mandelbrotPushConstants::DEFAULT_ZOOM / _zoomVal;
        _renderer->mandelbrotConstants.zoomMultScreenRatio =
          _renderer->mandelbrotConstants.zoom * _screenRatio;
        _renderer->mandelbrotComputeDone = false;

        _timers.accept_event[ET_RIGHT_MOUSE] = 0;
        _timers.updated[ET_RIGHT_MOUSE] = 1;
    }
}

void
EventHandler::incIter()
{
    if (_timers.accept_event[ET_CONFIG]) {
        _renderer->mandelbrotConstants.maxIter += _iterStepValue;
        _renderer->mandelbrotComputeDone = false;

        _timers.accept_event[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::decIter()
{
    if (_timers.accept_event[ET_CONFIG]) {
        _renderer->mandelbrotConstants.maxIter =
          (_renderer->mandelbrotConstants.maxIter <= _iterStepValue)
            ? 1
            : _renderer->mandelbrotConstants.maxIter - _iterStepValue;
        _renderer->mandelbrotComputeDone = false;

        _timers.accept_event[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::resetIter()
{
    if (_timers.accept_event[ET_CONFIG]) {
        _renderer->mandelbrotConstants.maxIter =
          mandelbrotPushConstants::DEFAULT_MAX_ITER;
        _renderer->mandelbrotComputeDone = false;

        _timers.accept_event[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

glm::vec2
EventHandler::computeMouseOffset(glm::vec2 const &mousePos,
                                  glm::ivec2 const &fbSize)
{
    glm::vec2 mouseOffsetPosition{};

    mouseOffsetPosition.x =
      _renderer->mandelbrotConstants.zoomMultScreenRatio *
      ((mousePos.x / static_cast<float>(fbSize.x)) - 0.5f);
    mouseOffsetPosition.y =
      _renderer->mandelbrotConstants.zoom *
      ((mousePos.y / static_cast<float>(fbSize.y)) - 0.5f);
    return (mouseOffsetPosition);
}
