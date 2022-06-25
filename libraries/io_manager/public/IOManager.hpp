#ifndef VK_MANDELBROT_VULKAN_IOMANAGER_HPP
#define VK_MANDELBROT_VULKAN_IOMANAGER_HPP

#include <array>
#include <vector>
#include <string>
#include <cstdint>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "IOManagerWindowCreationOption.hpp"
#include "IOEvents.hpp"

class IOManager final
{
  public:
    IOManager();
    ~IOManager();
    IOManager(IOManager const &src) = delete;
    IOManager &operator=(IOManager const &rhs) = delete;
    IOManager(IOManager &&src) = delete;
    IOManager &operator=(IOManager &&rhs) = delete;

    // Window related
    void createWindow(IOManagerWindowCreationOption &&opts);
    [[nodiscard]] GLFWwindow *getWindow() const;
    void deleteWindow();
    [[nodiscard]] bool wasResized();
    void toggleFullscreen();
    [[nodiscard]] bool shouldClose() const;
    void triggerClose() const;
    void toggleMouseExclusive();
    void toggleMouseVisibility();
    [[nodiscard]] bool isMouseExclusive() const;
    [[nodiscard]] float getWindowRatio() const;
    [[nodiscard]] glm::ivec2 getWindowSize() const;
    [[nodiscard]] glm::ivec2 getFramebufferSize() const;

    // Keyboard / Mouse Input related
    [[nodiscard]] IOEvents getEvents() const;
    void resetMouseScroll();

    // Vulkan related
    VkSurfaceKHR createVulkanSurface(VkInstance instance);
    [[nodiscard]] static std::vector<char const *>
    getRequiredInstanceExtension();

  private:
    // Constants
    static constexpr uint16_t const KEYS_BUFF_SIZE = 1024;
    static constexpr uint16_t const MOUSE_KEYS_BUFF_SIZE = 32;

    // Input
    std::array<uint8_t, KEYS_BUFF_SIZE> _keys{};
    std::array<uint8_t, MOUSE_KEYS_BUFF_SIZE> _mouseButton{};
    glm::vec2 _mousePosition{};
    float _mouseScroll{};

    // Window related
    GLFWwindow *_win{};
    bool _fullscreen{};
    bool _resized{};
    glm::ivec2 _winSize{};
    glm::ivec2 _framebufferSize{};

    bool _mouseExclusive{};
    bool _cursorHiddenOnWindow{};

    // Callbacks
    inline void _initCallbacks();

    // Mouse
    inline void _applyMouseVisibility() const;
};

#endif // VK_MANDELBROT_VULKAN_IOMANAGER_HPP
