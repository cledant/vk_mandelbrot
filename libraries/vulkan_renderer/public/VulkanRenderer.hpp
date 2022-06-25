#ifndef VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP
#define VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP

#include "glm/glm.hpp"

#include <vector>
#include <array>
#include <string>
#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSync.hpp"
#include "ui/VulkanUi.hpp"
#include "renderPass/VulkanSceneRenderPass.hpp"

class VulkanRenderer final
{
  public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;
    VulkanRenderer(VulkanRenderer const &src) = delete;
    VulkanRenderer &operator=(VulkanRenderer const &rhs) = delete;
    VulkanRenderer(VulkanRenderer &&src) = delete;
    VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

    // Instance related
    void createInstance(std::string &&app_name,
                        std::string &&engine_name,
                        uint32_t app_version,
                        uint32_t engine_version,
                        std::vector<char const *> &&required_extensions);
    [[nodiscard]] VkInstance getVkInstance() const;
    void init(VkSurfaceKHR surface, uint32_t win_w, uint32_t win_h);
    void resize(uint32_t win_w, uint32_t win_h);
    void clear();

    // Info related
    [[nodiscard]] std::string const &getAppName() const;
    [[nodiscard]] uint32_t getAppVersion() const;
    [[nodiscard]] std::string const &getEngineName() const;
    [[nodiscard]] uint32_t getEngineVersion() const;

    // Skybox related
    void setSkyboxInfo(glm::mat4 const &skyboxInfo);

    // Particles related
    void toggleParticlesMvt();
    void setParticleGenerationType(VulkanParticleGenerationType type);
    void setParticlesNumber(uint32_t nbParticles);
    void setParticlesMaxSpeed(uint32_t maxSpeed);
    void setParticlesColor(glm::vec3 const &particlesColor);
    void setParticleGravityCenter(glm::vec3 const &particleGravityCenter);
    void setParticleMass(float mass);
    void setDeltaT(float deltaT);

    // Render related
    void draw(glm::mat4 const &view_proj_mat);

  private:
    std::string _appName;
    std::string _engineName;
    uint32_t _appVersion{};
    uint32_t _engineVersion{};

    VulkanInstance _vkInstance;
    VulkanSwapChain _swapChain;
    VulkanSync _sync;
    VulkanUi _ui;
    VulkanSceneRenderPass _sceneRenderPass;

    // Compute shader control
    bool _doParticleGeneration = true;
    bool _doParticleMvt = false;
    bool _updateComputeCmds = true;

    // Renderer global uniform
    VulkanBuffer _systemUniform{};

    // Cmd Buffers
    std::vector<VkCommandBuffer> _renderCommandBuffers;
    std::vector<VkCommandBuffer> _computeCommandBuffers;

    inline void recordRenderCmds();
    inline void recordComputeCmds(VulkanParticleComputeShaderType type,
                                  bool registerCmd);
    inline void selectComputeCase();
    inline void emitDrawCmds(uint32_t img_index,
                             glm::mat4 const &view_proj_mat);
};

#endif // VK_MANDELBROT_VULKAN_VULKANRENDERER_HPP
