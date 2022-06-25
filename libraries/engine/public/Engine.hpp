#ifndef VK_MANDELBROT_VULKAN_ENGINE_HPP
#define VK_MANDELBROT_VULKAN_ENGINE_HPP

#include "IOManager.hpp"
#include "EventHandler.hpp"
//#include "VulkanRenderer.hpp"

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
    IOManager _io_manager;
    // VulkanRenderer _vk_renderer;
    EventHandler _event_handler;
};

#endif // VK_MANDELBROT_VULKAN_ENGINE_HPP