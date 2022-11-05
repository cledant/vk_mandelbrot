#ifndef VK_MANDELBROT_ENGINE_HPP
#define VK_MANDELBROT_ENGINE_HPP

#include "IOManager.hpp"
#include "EventHandler.hpp"
#include "VulkanRenderer.hpp"

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
    VulkanRenderer _vkRenderer;
    EventHandler _eventHandler;
};

#endif // VK_MANDELBROT_ENGINE_HPP