#include "IOManager.hpp"

#include <stdexcept>

#define THIS_WIN_PTR static_cast<IOManager *>(glfwGetWindowUserPointer(win))

IOManager::IOManager()
{
    if (!glfwInit()) {
        throw std::runtime_error("Glfw : failed to init");
    }
}

IOManager::~IOManager()
{
    glfwTerminate();
}

// Window related
void
IOManager::createWindow(IOManagerWindowCreationOption &&opts)
{
    if (!_win) {
        if (!glfwVulkanSupported()) {
            throw std::runtime_error("Glfw : Vulkan not supported !");
        }
        _winSize = opts.size;
        _mouseExclusive = opts.mouseExclusive;
        _cursorHiddenOnWindow = opts.cursorHiddenOnWindow;
        glfwWindowHint(GLFW_RESIZABLE, opts.resizable);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        _win = glfwCreateWindow(
          _winSize.x, _winSize.y, opts.winName.c_str(), nullptr, nullptr);
        if (!_win) {
            throw std::runtime_error("Glfw : failed to create window");
        }
        glfwSetWindowPos(_win, 100, 100);
        glfwGetWindowSize(_win, &_winSize.x, &_winSize.y);
        glfwGetFramebufferSize(_win, &_framebufferSize.x, &_framebufferSize.y);
        glfwSetWindowUserPointer(_win, this);
        initCallbacks();
        if (opts.fullscreen) {
            toggleFullscreen();
        }
        applyMouseVisibility();
    }
}

GLFWwindow *
IOManager::getWindow() const
{
    return (_win);
}

void
IOManager::deleteWindow()
{
    if (!_win) {
        glfwDestroyWindow(_win);
        _win = nullptr;
    }
}

bool
IOManager::wasResized()
{
    auto tmp = _resized;
    _resized = false;
    return (tmp);
}

void
IOManager::toggleFullscreen()
{
    _fullscreen = !_fullscreen;

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        throw std::runtime_error("Glfw : No primary monitor");
    }

    GLFWvidmode const *mode = glfwGetVideoMode(monitor);
    if (!mode) {
        throw std::runtime_error("Glfw : failed to fetch monitor mode");
    }

    if (_fullscreen) {
        glfwSetWindowMonitor(
          _win, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(_win,
                             nullptr,
                             100,
                             100,
                             DEFAULT_WIN_SIZE.x,
                             DEFAULT_WIN_SIZE.y,
                             GLFW_DONT_CARE);
    }
}

bool
IOManager::shouldClose() const
{
    return (glfwWindowShouldClose(_win));
}

void
IOManager::triggerClose() const
{
    glfwSetWindowShouldClose(_win, 1);
}

void
IOManager::toggleMouseExclusive()
{
    _mouseExclusive = !_mouseExclusive;
    applyMouseVisibility();
}

void
IOManager::toggleMouseVisibility()
{
    _cursorHiddenOnWindow = !_cursorHiddenOnWindow;
    applyMouseVisibility();
}

bool
IOManager::isMouseExclusive() const
{
    return (_mouseExclusive);
}

float
IOManager::getWindowRatio() const
{
    return (static_cast<float>(_winSize.x) / static_cast<float>(_winSize.y));
}

glm::ivec2
IOManager::getWindowSize() const
{
    return (_winSize);
}

glm::ivec2
IOManager::getFramebufferSize() const
{
    return (_framebufferSize);
}

// Keyboard / Mouse Input related
IOEvents
IOManager::getEvents() const
{
    IOEvents io{};

    glfwPollEvents();
    io.events[IOET_QUIT] = _keys[GLFW_KEY_F12];
    io.events[IOET_FULLSCREEN] = _keys[GLFW_KEY_F11];
    io.events[IOET_UP] = _keys[GLFW_KEY_W];
    io.events[IOET_DOWN] = _keys[GLFW_KEY_S];
    io.events[IOET_RIGHT] = _keys[GLFW_KEY_D];
    io.events[IOET_LEFT] = _keys[GLFW_KEY_A];
    io.events[IOET_RESET_ZOOM_SCREEN_CENTER] =
      _mouseButton[GLFW_MOUSE_BUTTON_RIGHT];
    io.events[IOET_INC_ITER] = _keys[GLFW_KEY_EQUAL];
    io.events[IOET_DEC_ITER] = _keys[GLFW_KEY_MINUS];
    io.events[IOET_RESET_ITER] = _keys[GLFW_KEY_P];
    io.multiplier = _keys[GLFW_KEY_LEFT_SHIFT];
    io.mousePosition = _mousePosition;
    io.mouseScroll = _mouseScroll;
    return (io);
}

void
IOManager::resetMouseScroll()
{
    _mouseScroll = 0.0f;
}

// Vulkan related
VkSurfaceKHR
IOManager::createVulkanSurface(VkInstance instance)
{
    VkSurfaceKHR vk_surface{};
    if (glfwCreateWindowSurface(instance, _win, nullptr, &vk_surface) !=
        VK_SUCCESS) {
        throw std::runtime_error("IOManager: Failed to create window surface");
    }
    return (vk_surface);
}

std::vector<const char *>
IOManager::getRequiredInstanceExtension()
{
    uint32_t nb_glfw_extension = 0;
    char const **glfw_extensions =
      glfwGetRequiredInstanceExtensions(&nb_glfw_extension);
    std::vector<char const *> extensions(glfw_extensions,
                                         glfw_extensions + nb_glfw_extension);
    return (extensions);
}

// Callbacks
void
IOManager::initCallbacks()
{
    // Keyboard input
    auto keyboard_callback =
      [](GLFWwindow *win, int key, int scancode, int action, int mods) {
          static_cast<void>(scancode);
          static_cast<void>(mods);
          if (key >= 0 && key < KEYS_BUFF_SIZE) {
              if (action == GLFW_PRESS) {
                  THIS_WIN_PTR->_keys[key] = 1;
              } else if (action == GLFW_RELEASE) {
                  THIS_WIN_PTR->_keys[key] = 0;
              }
          }
      };
    glfwSetKeyCallback(_win, keyboard_callback);

    // Mouse position
    auto cursor_position_callback =
      [](GLFWwindow *win, double xpos, double ypos) {
          THIS_WIN_PTR->_mousePosition = glm::vec2(xpos, ypos);
      };
    glfwSetCursorPosCallback(_win, cursor_position_callback);

    // Mouse button input
    auto mouse_button_callback =
      [](GLFWwindow *win, int button, int action, int mods) {
          static_cast<void>(mods);
          if (button >= 0 && button < MOUSE_KEYS_BUFF_SIZE) {
              if (action == GLFW_PRESS)
                  THIS_WIN_PTR->_mouseButton[button] = GLFW_PRESS;
              else if (action == GLFW_RELEASE)
                  THIS_WIN_PTR->_mouseButton[button] = GLFW_RELEASE;
          }
      };
    glfwSetMouseButtonCallback(_win, mouse_button_callback);

    // Mouse Scroll
    auto mouse_scroll_callback =
      [](GLFWwindow *win, double xoffset, double yoffset) {
          static_cast<void>(win);
          THIS_WIN_PTR->_mouseScroll += xoffset;
          THIS_WIN_PTR->_mouseScroll += yoffset;
      };
    glfwSetScrollCallback(_win, mouse_scroll_callback);

    // Close
    auto close_callback = [](GLFWwindow *win) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
    };
    glfwSetWindowCloseCallback(_win, close_callback);

    // Window
    auto window_size_callback = [](GLFWwindow *win, int w, int h) {
        auto prev_size = THIS_WIN_PTR->_winSize;

        THIS_WIN_PTR->_winSize = glm::ivec2(w, h);
        if (prev_size != THIS_WIN_PTR->_winSize) {
            THIS_WIN_PTR->_resized = true;
        }
    };
    glfwSetWindowSizeCallback(_win, window_size_callback);

    // Framebuffer
    auto framebuffer_size_callback = [](GLFWwindow *win, int w, int h) {
        THIS_WIN_PTR->_framebufferSize = glm::ivec2(w, h);
    };
    glfwSetFramebufferSizeCallback(_win, framebuffer_size_callback);
}

void
IOManager::applyMouseVisibility() const
{
    if (_mouseExclusive) {
        glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else if (_cursorHiddenOnWindow) {
        glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
        glfwSetInputMode(_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}