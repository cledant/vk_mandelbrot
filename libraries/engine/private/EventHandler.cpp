#include "EventHandler.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/gtc/matrix_transform.hpp"
#include "fmt/core.h"
#include "fmt/chrono.h"

#include "utils/VulkanTextureUtils.hpp"

#include <chrono>
#include <functional>

void
EventHandler::setIOManager(IOManager *ioManager)
{
    assert(ioManager);

    _ioManager = ioManager;
    auto fbSize = _ioManager->getFramebufferSize();
    _screenRatio =
      static_cast<double>(fbSize.x) / static_cast<double>(fbSize.y);
}

void
EventHandler::setVkRenderer(VulkanRenderer *renderer)
{
    _renderer = renderer;
}

void
EventHandler::setSwapChain(VulkanSwapChain *swapChain)
{
    _swapChain = swapChain;
}

void
EventHandler::setUi(Ui *ui)
{
    _ui = ui;
}

void
EventHandler::setGfxAssets(GfxAssets *gfxAssets)
{
    _gfxAssets = gfxAssets;
}

void
EventHandler::processEvents()
{
    assert(_ioManager);
    assert(_renderer);
    assert(_swapChain);
    assert(_ui);
    assert(_gfxAssets);

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
    setUiInfoValues();

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
        _ui->fullscreen = !_ui->fullscreen;
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
        _gfxAssets->mandelbrotConstants.offsetX =
          mandelbrotPushConstants::DEFAULT_OFFSET_X;
        _gfxAssets->mandelbrotConstants.offsetY =
          mandelbrotPushConstants::DEFAULT_OFFSET_Y;
        _zoomVal = DEFAULT_ZOOM;
        _gfxAssets->mandelbrotConstants.zoom =
          mandelbrotPushConstants::DEFAULT_ZOOM / _zoomVal;
        _gfxAssets->mandelbrotConstants.zoomMultScreenRatio =
          _gfxAssets->mandelbrotConstants.zoom * _screenRatio;
        _gfxAssets->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_RIGHT_MOUSE] = 0;
        _timers.updated[ET_RIGHT_MOUSE] = 1;
    }
}

void
EventHandler::incIter()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _gfxAssets->mandelbrotConstants.maxIter += _iterStepValue;
        _gfxAssets->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::decIter()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _gfxAssets->mandelbrotConstants.maxIter =
          (_gfxAssets->mandelbrotConstants.maxIter <= _iterStepValue)
            ? 1
            : _gfxAssets->mandelbrotConstants.maxIter - _iterStepValue;
        _gfxAssets->mandelbrotComputeDone = false;

        _timers.acceptEvent[ET_CONFIG] = 0;
        _timers.updated[ET_CONFIG] = 1;
    }
}

void
EventHandler::resetIter()
{
    if (_timers.acceptEvent[ET_CONFIG]) {
        _gfxAssets->mandelbrotConstants.maxIter =
          mandelbrotPushConstants::DEFAULT_MAX_ITER;
        _gfxAssets->mandelbrotComputeDone = false;

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
    _recreateSwapchain = true;
}

void
EventHandler::uiSaveFractalToFile()
{
    _renderer->saveNextFrame = true;
    _saveScreenshotTofile = true;
}

void
EventHandler::uiRendererScale()
{
    _recreateSwapchain = true;
}

void
EventHandler::uiNbIteration()
{
    _gfxAssets->mandelbrotConstants.maxIter = _ui->iterationInput.parsedInput;
    _gfxAssets->mandelbrotComputeDone = false;
}

void
EventHandler::initMultipliers(IOEvents const &ioEvents)
{
    _iterStepValue =
      (ioEvents.multiplier) ? ITER_WITH_MULTIPLIER : ITER_NO_MULTIPLIER;
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

    if (ioEvents.mouseScroll != 0.0) {
        if (ioEvents.mouseScroll > 0.0) {
            _zoomVal *= ZOOM_MULTIPLIER;
            addOffsetZoomIn = true;
        } else {
            _zoomVal /= ZOOM_MULTIPLIER;
            addOffsetZoomOut = true;
        }

        if (_zoomVal < 1.0) {
            _zoomVal = 1.0;
            addOffsetZoomOut = false;
        }

        _gfxAssets->mandelbrotComputeDone = false;
        _ioManager->resetMouseScroll();
    }
    if (addOffsetZoomOut) {
        _gfxAssets->mandelbrotConstants.offsetX -= computeMouseOffset(
          ioEvents.mousePosition.x,
          fbSize.x,
          _gfxAssets->mandelbrotConstants.zoomMultScreenRatio);
        _gfxAssets->mandelbrotConstants.offsetY -=
          computeMouseOffset(ioEvents.mousePosition.y,
                             fbSize.y,
                             _gfxAssets->mandelbrotConstants.zoom);
    }
    _gfxAssets->mandelbrotConstants.zoom =
      mandelbrotPushConstants::DEFAULT_ZOOM / _zoomVal;
    _gfxAssets->mandelbrotConstants.zoomMultScreenRatio =
      _gfxAssets->mandelbrotConstants.zoom * _screenRatio;
    if (addOffsetZoomIn) {
        _gfxAssets->mandelbrotConstants.offsetX += computeMouseOffset(
          ioEvents.mousePosition.x,
          fbSize.x,
          _gfxAssets->mandelbrotConstants.zoomMultScreenRatio);
        _gfxAssets->mandelbrotConstants.offsetY +=
          computeMouseOffset(ioEvents.mousePosition.y,
                             fbSize.y,
                             _gfxAssets->mandelbrotConstants.zoom);
    }
}

void
EventHandler::keyboardMvtHandling()
{
    if (_keyboardMvt.x) {
        _gfxAssets->mandelbrotConstants.offsetX +=
          _keyboardMvt.x * (_keyboardMvtStepValue / _zoomVal);
        _gfxAssets->mandelbrotComputeDone = false;
    }
    if (_keyboardMvt.y) {
        _gfxAssets->mandelbrotConstants.offsetY -=
          _keyboardMvt.y * (_keyboardMvtStepValue / _zoomVal);
        _gfxAssets->mandelbrotComputeDone = false;
    }
}

void
EventHandler::recreateSwapchain()
{
    if (_recreateSwapchain || _ioManager->wasResized()) {
        auto fbSize = _ioManager->getFramebufferSize();

        _swapChain->resize(fbSize.x, fbSize.y, _ui->vsync);
        _renderer->resize();
        _gfxAssets->resize(_swapChain->swapChainExtent.width,
                           _swapChain->swapChainExtent.height,
                           _swapChain->swapChainImageViews,
                           _swapChain->swapChainImageFormat,
                           UiMenuBarRendererScaleSelection::INDEX_VALUES
                             [_ui->rendererScaleSelection.activeIndex]);
        _screenRatio =
          static_cast<double>(fbSize.x) / static_cast<double>(fbSize.y);
    }
}

double
EventHandler::computeMouseOffset(double mousePos, double fbSize, double zoom)
{
    return (zoom * ((mousePos / fbSize) - 0.5));
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
      keyboardEvents = {
          &EventHandler::uiCloseWinEvent, &EventHandler::uiToggleFullscreen,
          &EventHandler::uiToggleVsync,   &EventHandler::uiSaveFractalToFile,
          &EventHandler::uiRendererScale, &EventHandler::uiNbIteration
      };

    for (uint32_t i = 0; i < UET_TOTAL_NB; ++i) {
        if (uiEvents.events[i]) {
            std::invoke(keyboardEvents[i], this);
        }
    }
}

void
EventHandler::setUiInfoValues()
{
    _ui->infoOverview.maxIteration = _gfxAssets->mandelbrotConstants.maxIter;
    _ui->infoOverview.zoom = _gfxAssets->mandelbrotConstants.zoom;
    _ui->infoOverview.cameraPos = { _gfxAssets->mandelbrotConstants.offsetX,
                                    _gfxAssets->mandelbrotConstants.offsetY };
    _ui->infoOverview.rendererW = _gfxAssets->mandelbrotConstants.fbW;
    _ui->infoOverview.rendererH = _gfxAssets->mandelbrotConstants.fbH;
}

void
EventHandler::screenshotHandling()
{
    if (_saveScreenshotTofile) {
        auto screenshot = _gfxAssets->capturedFrame.generateScreenshot();
        auto filepath = generateScrenshotName(".");

        _screenshotsResults.emplace_back(
          std::async(std::launch::async,
                     &EventHandler::saveScreenshotHelper,
                     std::move(screenshot),
                     std::move(filepath)));
        _saveScreenshotTofile = false;
    }

    // Handling in progress tasks
    for (auto it = _screenshotsResults.begin();
         it != _screenshotsResults.end();) {
        if (it->wait_for(std::chrono::nanoseconds(0)) ==
            std::future_status::ready) {

            auto now = std::chrono::steady_clock::now();
            ++_ui->notifications.numberOfNotifications[it->get()];
            _ui->notifications.lastNotificationTime = now;
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
