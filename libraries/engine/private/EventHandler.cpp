#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"
#include "fmt/core.h"
#include "fmt/chrono.h"

#include <chrono>
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

    screenshotHandling();

    // Values init
    _keyboardMvt = glm::ivec2(0);
    _skipZoomHandling = false;
    _recreateSwapchain = false;

    // Checking Timers
    auto now = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        std::chrono::duration<double> time_diff = now - _timers.timeRef[i];
        _timers.timerDiff[i] = time_diff.count();
        _timers.acceptEvent[i] = (time_diff.count() > _timers.timerValues[i]);
    }

    auto ioEvents = _ioManager->getEvents();
    initMultipliers(ioEvents);
    processIoEvents(ioEvents);
    processUiEvents(_ui->getUiEvents());
    recreateSwapchain();
    zoomHandling(ioEvents, _ioManager->getFramebufferSize());
    keyboardMvtHandling();

    // Setting timers origin
    for (uint32_t i = 0; i < ET_NB_EVENT_TIMER_TYPES; ++i) {
        if (_timers.updated[i]) {
            _timers.timeRef[i] = now;
        }
        _timers.updated[i] = 0;
    }
}

// IO Events handling functions
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
        _recreateSwapchain = true;

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
EventHandler::displayControlMenu()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _ui->displayControlMenu = !_ui->displayControlMenu;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::displayUi()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _ui->displayUi = !_ui->displayUi;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::displayInfo()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _ui->showInfoPosition = !_ui->showInfoPosition;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::displayFps()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _ui->showInfoFps = !_ui->showInfoFps;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::displayHelp()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _ui->helpBox.isOpen = !_ui->helpBox.isOpen;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::takeScreenshot()
{
    if (_timers.acceptEvent[ET_SCREENSHOT]) {
        _renderer->saveNextFrame = true;
        _saveScreenshotTofile = true;

        _timers.acceptEvent[ET_SCREENSHOT] = 0;
        _timers.updated[ET_SCREENSHOT] = 1;
    }
}

// Ui Events handling functions
void
EventHandler::uiCloseWinEvent()
{
    _ioManager->triggerClose();
}

void
EventHandler::uiToggleFullscreen()
{
    _ioManager->toggleFullscreen();
    _recreateSwapchain = true;
}

void
EventHandler::uiToggleVsync()
{
    _vsync = !_vsync;
    _recreateSwapchain = true;
}

void
EventHandler::uiSaveFractalToFile()
{
    _renderer->saveNextFrame = true;
    _saveScreenshotTofile = true;
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

void
EventHandler::recreateSwapchain()
{
    if (_recreateSwapchain || _ioManager->wasResized()) {
        auto fbSize = _ioManager->getFramebufferSize();

        _renderer->resize(fbSize.x, fbSize.y, _vsync);
        _screenRatio =
          static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y);
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
                         &EventHandler::resetIter,
                         &EventHandler::displayControlMenu,
                         &EventHandler::displayUi,
                         &EventHandler::displayInfo,
                         &EventHandler::displayFps,
                         &EventHandler::displayHelp,
                         &EventHandler::takeScreenshot };

    for (uint32_t i = 0; i < IOET_NB; ++i) {
        if (ioEvents.events[i]) {
            std::invoke(keyboardEvents[i], this);
        }
    }
}

void
EventHandler::processUiEvents(UiEvents const &uiEvents)
{
    static const std::array<void (EventHandler::*)(), UET_TOTAL_NB>
      keyboardEvents = { &EventHandler::uiCloseWinEvent,
                         &EventHandler::uiToggleFullscreen,
                         &EventHandler::uiToggleVsync,
                         &EventHandler::uiSaveFractalToFile };

    for (uint32_t i = 0; i < UET_TOTAL_NB; ++i) {
        if (uiEvents.events[i]) {
            std::invoke(keyboardEvents[i], this);
        }
    }
}

void
EventHandler::screenshotHandling()
{
    // TODO: use ret to display success or error on UI
    if (_saveScreenshotTofile) {
        auto screenshot = _renderer->generateScreenshot();
        auto filepath = generateScrenshotName(".");
        _screenshotsResults.emplace_back(
          std::async(std::launch::async,
                     &EventHandler::saveScreenshotHelper,
                     std::move(screenshot),
                     std::move(filepath)));
        _saveScreenshotTofile = false;
    }

    // Looping on promises
    for (auto it = _screenshotsResults.begin();
         it != _screenshotsResults.end();) {
        if (it->wait_for(std::chrono::nanoseconds(0)) ==
            std::future_status::ready) {
            auto result = it->get();

            if (result) {
                fmt::print("Saved !\n");
            } else {
                fmt::print("Failed !\n");
            }
            std::swap(*it, _screenshotsResults.back());
            _screenshotsResults.pop_back();
        } else {
            ++it;
        }
    }
}

std::string
EventHandler::generateScrenshotName(std::string const &folderpath)
{
    using namespace std::chrono;

    auto now = system_clock::now();
    auto nowEpoch = now.time_since_epoch();
    nowEpoch -= duration_cast<seconds>(nowEpoch);

    return (fmt::format("{}/mandelbrot_{:%Y-%m-%d:%H:%M:%S}:{:03}.png",
                        folderpath,
                        now,
                        duration_cast<milliseconds>(nowEpoch).count()));
}

bool
EventHandler::saveScreenshotHelper(VulkanScreenshot &&screenshot,
                                   std::string &&filepath)
{
    return (screenshot.saveScreenshotToFile(filepath));
}
