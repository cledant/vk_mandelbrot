#include "GfxAssets.hpp"

#include "VulkanRenderer.hpp"

void
GfxAssets::init(VulkanInstance const &vkInstance,
                uint32_t winW,
                uint32_t winH,
                std::vector<VulkanTexture> const &swapChainImageViews,
                VkFormat swapChainImageFormat,
                float scale)
{
    _devices = vkInstance.devices;
    _depthFormat = vkInstance.depthFormat;

    // Textures
    int32_t rendererW = winW * scale;
    int32_t rendererH = winH * scale;
    imageMandelbrot.init(
      vkInstance, VK_FORMAT_R8G8B8A8_UNORM, _depthFormat, rendererW, rendererH);
    capturedFrame.allocate(_devices, rendererW, rendererH, 4, false);

    // Render Passes + pipelines
    toScreenRenderPass.init(
      vkInstance, swapChainImageViews, swapChainImageFormat, winW, winH);
    toScreen.init(vkInstance,
                  swapChainImageViews.size(),
                  winW,
                  winH,
                  toScreenRenderPass.renderPass,
                  imageMandelbrot.descriptorImage);

    mandelbrotFirstRenderPass.init(vkInstance,
                                   VK_FORMAT_R8G8B8A8_UNORM,
                                   _depthFormat,
                                   imageMandelbrot.colorTex.textureImgView,
                                   imageMandelbrot.depthTex.textureImgView,
                                   rendererW,
                                   rendererH);
    mandelbrotFirst.init(
      vkInstance, mandelbrotFirstRenderPass.renderPass, rendererW, rendererH);

    mandelbrotMultipleRenderPass.init(vkInstance,
                                      VK_FORMAT_R8G8B8A8_UNORM,
                                      _depthFormat,
                                      imageMandelbrot.colorTex.textureImgView,
                                      imageMandelbrot.depthTex.textureImgView,
                                      rendererW,
                                      rendererH);
    mandelbrotMultiple.init(vkInstance,
                            mandelbrotMultipleRenderPass.renderPass,
                            rendererW,
                            rendererH);

    uiRenderPass.init(
      vkInstance, swapChainImageViews, swapChainImageFormat, winW, winH);
    ui.init(vkInstance, uiRenderPass.renderPass, swapChainImageViews.size());
}

void
GfxAssets::resize(uint32_t winW,
                  uint32_t winH,
                  std::vector<VulkanTexture> const &swapChainImageViews,
                  VkFormat swapChainImageFormat,
                  float scale)
{
    vkDeviceWaitIdle(_devices.device);
    if (winW <= 0 || winH <= 0 || scale <= 0.0f) {
        return;
    }

    // Textures
    int32_t rendererW = winW * scale;
    int32_t rendererH = winH * scale;
    imageMandelbrot.resize(
      VK_FORMAT_R8G8B8A8_UNORM, _depthFormat, rendererW, rendererH);
    capturedFrame.clear();
    capturedFrame.allocate(_devices, rendererW, rendererH, 4, false);

    // Render passes + pipelines
    toScreenRenderPass.resize(
      swapChainImageViews, swapChainImageFormat, winW, winH);
    toScreen.resize(swapChainImageViews.size(),
                    winW,
                    winH,
                    toScreenRenderPass.renderPass,
                    imageMandelbrot.descriptorImage);

    mandelbrotFirstRenderPass.resize(VK_FORMAT_R8G8B8A8_UNORM,
                                     _depthFormat,
                                     imageMandelbrot.colorTex.textureImgView,
                                     imageMandelbrot.depthTex.textureImgView,
                                     rendererW,
                                     rendererH);
    mandelbrotFirst.resize(
      mandelbrotFirstRenderPass.renderPass, rendererW, rendererH);

    mandelbrotMultipleRenderPass.resize(VK_FORMAT_R8G8B8A8_UNORM,
                                        _depthFormat,
                                        imageMandelbrot.colorTex.textureImgView,
                                        imageMandelbrot.depthTex.textureImgView,
                                        rendererW,
                                        rendererH);
    mandelbrotMultiple.resize(
      mandelbrotMultipleRenderPass.renderPass, rendererW, rendererH);

    uiRenderPass.resize(swapChainImageViews, swapChainImageFormat, winW, winH);
    ui.resize(uiRenderPass.renderPass, swapChainImageViews.size());

    mandelbrotComputeDone = false;
}

void
GfxAssets::clear()
{
    vkDeviceWaitIdle(_devices.device);
    ui.clear();
    uiRenderPass.clear();
    mandelbrotFirst.clear();
    mandelbrotFirstRenderPass.clear();
    mandelbrotMultiple.clear();
    mandelbrotMultipleRenderPass.clear();
    toScreen.clear();
    toScreenRenderPass.clear();
    capturedFrame.clear();
    imageMandelbrot.clear();
    mandelbrotComputeDone = false;
    mandelbrotConstants = {};
    _devices = {};
    _depthFormat = {};
}

void
GfxAssets::recordDrawCmds(VkCommandBuffer cmdBuffer,
                          uint32_t imgIndex,
                          VkClearColorValue const &cmdClearColor,
                          VkClearDepthStencilValue const &cmdClearDepth)
{
    vkResetCommandBuffer(cmdBuffer, 0);

    VkCommandBufferBeginInfo cb_begin_info{};
    cb_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cb_begin_info.flags = 0;
    cb_begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(cmdBuffer, &cb_begin_info) != VK_SUCCESS) {
        throw std::runtime_error("VulkanRenderer: Failed to begin "
                                 "recording render command buffer");
    }

    if (!mandelbrotComputeDone) {
        // Update push constant values
        mandelbrotConstants.fbW = imageMandelbrot.colorTex.width;
        mandelbrotConstants.fbH = imageMandelbrot.colorTex.height;
        recordMandelbrotFirstRenderCmd(cmdBuffer, cmdClearColor, cmdClearDepth);
        recordMandelbrotMultipleRenderCmd(
          cmdBuffer, cmdClearColor, cmdClearDepth);
        mandelbrotComputeDone = true;
    }
    recordToScreenRenderCmd(cmdBuffer, imgIndex, cmdClearColor, cmdClearDepth);
    recordUiRenderCmd(cmdBuffer, imgIndex, cmdClearColor, cmdClearDepth);

    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanRenderer: Failed to record render command Buffer");
    }
}

// Sub-functions for recordDrawCmds
void
GfxAssets::recordMandelbrotFirstRenderCmd(
  VkCommandBuffer cmdBuffer,
  VkClearColorValue const &cmdClearColor,
  VkClearDepthStencilValue const &cmdClearDepth)
{
    int32_t realChunkWidth = CHUNK_WIDTH;
    if (imageMandelbrot.colorTex.height < CHUNK_WIDTH) {
        realChunkWidth = imageMandelbrot.colorTex.width;
    }
    int32_t realChunkHeight = CHUNK_HEIGHT;
    if (imageMandelbrot.colorTex.height < CHUNK_HEIGHT) {
        realChunkHeight = imageMandelbrot.colorTex.height;
    }

    // Begin Mandelbrot first renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = cmdClearDepth;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = mandelbrotFirstRenderPass.renderPass;
    rp_begin_info.framebuffer = mandelbrotFirstRenderPass.framebuffer;
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = {
        static_cast<uint32_t>(realChunkWidth),
        static_cast<uint32_t>(realChunkHeight),
    };
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    mandelbrotFirst.generateCommands(cmdBuffer, mandelbrotConstants);
    vkCmdEndRenderPass(cmdBuffer);
}

void
GfxAssets::recordMandelbrotMultipleRenderCmd(
  VkCommandBuffer cmdBuffer,
  VkClearColorValue const &cmdClearColor,
  VkClearDepthStencilValue const &cmdClearDepth)
{
    // Begin Mandelbrot renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = cmdClearDepth;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = mandelbrotMultipleRenderPass.renderPass;
    rp_begin_info.framebuffer = mandelbrotMultipleRenderPass.framebuffer;
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    int32_t i = 1;
    int32_t j = 0;
    while (j * CHUNK_HEIGHT < imageMandelbrot.colorTex.height) {
        int32_t realChunkHeight = CHUNK_HEIGHT;
        if ((imageMandelbrot.colorTex.height - (j * CHUNK_HEIGHT)) <
            CHUNK_HEIGHT) {
            realChunkHeight =
              imageMandelbrot.colorTex.height - (j * CHUNK_HEIGHT);
        }

        while (i * CHUNK_WIDTH < imageMandelbrot.colorTex.width) {
            int32_t realChunkWidth = CHUNK_WIDTH;
            if ((imageMandelbrot.colorTex.width - (i * CHUNK_WIDTH)) <
                CHUNK_WIDTH) {
                realChunkWidth =
                  imageMandelbrot.colorTex.width - (i * CHUNK_WIDTH);
            }

            rp_begin_info.renderArea.extent = {
                static_cast<uint32_t>(realChunkWidth),
                static_cast<uint32_t>(realChunkHeight),
            };
            rp_begin_info.renderArea.offset = { i * CHUNK_WIDTH,
                                                j * CHUNK_HEIGHT };
            vkCmdBeginRenderPass(
              cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            mandelbrotMultiple.generateCommands(cmdBuffer, mandelbrotConstants);
            vkCmdEndRenderPass(cmdBuffer);
            ++i;
        }
        i = 0;
        ++j;
    }
}

void
GfxAssets::recordUiRenderCmd(VkCommandBuffer cmdBuffer,
                             uint32_t imgIndex,
                             VkClearColorValue const &cmdClearColor,
                             VkClearDepthStencilValue const &cmdClearDepth)
{
    // Begin Ui renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = cmdClearDepth;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = uiRenderPass.renderPass;
    rp_begin_info.framebuffer = uiRenderPass.framebuffers[imgIndex];
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = uiRenderPass.renderPassExtent;
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    ui.generateCommands(cmdBuffer);
    vkCmdEndRenderPass(cmdBuffer);
}

void
GfxAssets::recordToScreenRenderCmd(
  VkCommandBuffer cmdBuffer,
  uint32_t imgIndex,
  VkClearColorValue const &cmdClearColor,
  VkClearDepthStencilValue const &cmdClearDepth)
{
    // Begin onscreen renderpass
    std::array<VkClearValue, 2> clear_vals{};
    clear_vals[0].color = cmdClearColor;
    clear_vals[1].depthStencil = cmdClearDepth;
    VkRenderPassBeginInfo rp_begin_info{};
    rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass = toScreenRenderPass.renderPass;
    rp_begin_info.framebuffer = toScreenRenderPass.framebuffers[imgIndex];
    rp_begin_info.renderArea.offset = { 0, 0 };
    rp_begin_info.renderArea.extent = toScreenRenderPass.renderPassExtent;
    rp_begin_info.clearValueCount = clear_vals.size();
    rp_begin_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    toScreen.generateCommands(cmdBuffer, imgIndex);
    vkCmdEndRenderPass(cmdBuffer);
}