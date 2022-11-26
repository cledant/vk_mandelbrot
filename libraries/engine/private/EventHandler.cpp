#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"

#include <functional>

void
EventHandler::setIOManager(IOManager *ioManager)
{
    _ioManager = ioManager;
    auto fbSize = _ioManager->getFramebufferSize();
    _screenRatio = static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y);
}

void
EventHandler::setVkRenderer(VulkanRenderer *renderer)
{
    _renderer = renderer;
}

void
EventHandler::setUi(Ui *ui)
{
    _ui = ui;
}

void
EventHandler::processEvents()
{
    assert(_ioManager);
    assert(_renderer);
    assert(_ui);

    auto ioEvents = _ioManager->getEvents();
    auto fbSize = _ioManager->getFramebufferSize();

    // Values init
    _keyboardMvt = glm::ivec2(0);
    _skipZoomHandling = false;

    // Checking Timers
    auto now = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        std::chrono::duration<double> time_diff = now - _timers.timeRef[i];
        _timers.timerDiff[i] = time_diff.count();
        _timers.acceptEvent[i] = (time_diff.count() > _timers.timerValues[i]);
    }

    initMultipliers(ioEvents);
    processIoEvents(ioEvents);
    processUiEvents(_ui->getUiEvents());
    zoomHandling(ioEvents, fbSize);
    keyboardMvtHandling();

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
            _timers.timeRef[i] = now;
        }
        _timers.updated[i] = 0;
    }
}

void
EventHandler::closeWinEvent()
{
    if (_timers.acceptEvent[ET_SYSTEM]) {
        _ioManager->triggerClose();

        _timers.acceptEvent[ET_SYSTEM] = 0;
        _timers.updated[ET_SYSTEM] = 1;
    }
}

void
EventHandler::toggleFullscreen()
{
    if (_timers.acceptEvent[ET_SYSTEM]) {
        _ioManager->toggleFullscreen();

        _timers.acceptEvent[ET_SYSTEM] = 0;
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
    if (_timers.acceptEvent[ET_RIGHT_MOUSE]) {
        _skipZoomHandling = true;
        _keyboardMvt = glm::ivec2(0);
        _renderer->mandelbrotConstants.offset =
          mandelbrotPushConstants::DEFAULT_OFFSET;
        _zoomVal = DEFAULT_ZOOM;
        _renderer->mandelbrotConstants.zoom =
          mandelbrotPushConstants::DEFAULT_ZOOM / _zoomVal;
        _renderer->mandelbrotConstants.zoomMultScreenRatio =
          _renderer->mandelbrotConstants.zoom * _screenRatio;
        _renderer->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_RIGHT_MOUSE] = 0;
        _timers.updated[ET_RIGHT_MOUSE] = 1;
    }
}

void
EventHandler::incIter()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _renderer->mandelbrotConstants.maxIter += _iterStepValue;
        _renderer->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::decIter()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _renderer->mandelbrotConstants.maxIter =
          (_renderer->mandelbrotConstants.maxIter <= _iterStepValue)
            ? 1
            : _renderer->mandelbrotConstants.maxIter - _iterStepValue;
        _renderer->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::resetIter()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _renderer->mandelbrotConstants.maxIter =
          mandelbrotPushConstants::DEFAULT_MAX_ITER;
        _renderer->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::initMultipliers(IOEvents const &ioEvents)
{
    _iterStepValue =
      (ioEvents.multiplier) ? ITER_WITH_MULTIPLIER : ITER_NO_MULTIPLIER;
    _zoomStepValue =
      (ioEvents.multiplier) ? ZOOM_WITH_MULTIPLIER : ZOOM_NO_MULTIPLIER;
    _keyboardMvtStepValue = (ioEvents.multiplier) ? KEYBOARD_MVT_WITH_MULTIPLIER
                                                  : KEYBOARD_MVT_NO_MULTIPLIER;
}

void
EventHandler::zoomHandling(IOEvents const &ioEvents, glm::vec2 const &fbSize)
{
    if (_skipZoomHandling) {
        return;
    }
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
}

void
EventHandler::keyboardMvtHandling()
{
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

void
EventHandler::processIoEvents(IOEvents const &ioEvents)
{
    static const std::array<void (EventHandler::*)(), IOET_NB>
      keyboardEvents = { &EventHandler::closeWinEvent,
                         &EventHandler::toggleFullscreen,
                         &EventHandler::up,
                         &EventHandler::down,
                         &EventHandler::right,
                         &EventHandler::left,
                         &EventHandler::resetZoomScreenCenter,
                         &EventHandler::incIter,
                         &EventHandler::decIter,
                         &EventHandler::resetIter };

    for (uint32_t i = 0; i < IOET_NB; ++i) {
        if (ioEvents.events[i]) {
            std::invoke(keyboardEvents[i], this);
        }
    }
}

void
EventHandler::processUiEvents(UiEvents const &uiEvents)
{
    (void)uiEvents;

    // TODO
}
