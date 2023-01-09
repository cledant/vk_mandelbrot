#ifndef VK_MANDELBROT_ENGINE_HPP
#define VK_MANDELBROT_ENGINE_HPP

#include "IOManager.hpp"
#include "EventHandler.hpp"
#include "VulkanInstance.hpp"
#include "VulkanRenderer.hpp"
#include "Ui.hpp"

class Engine final
{
  public:
    Engine() = default;
    ~Engine() = default;
    Engine(Engine const &src) = delete;
    Engine &operator=(Engine const &rhs) = delete;
    Engine(Engine &&src) = delete;
    Engine &operator=(Engine &&rhs) = delete;

    void init();
    void run();

  private:
    IOManager _ioManager;
    VulkanInstance _vkInstance;
    VulkanRenderer _vkRenderer;
    Ui _ui;
    EventHandler _eventHandler;
};

#endif // VK_MANDELBROT_ENGINE_HPP