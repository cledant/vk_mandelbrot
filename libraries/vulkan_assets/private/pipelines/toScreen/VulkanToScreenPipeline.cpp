#include "pipelines/toScreen/VulkanToScreenPipeline.hpp"

#include <stdexcept>

#include "utils/VulkanShader.hpp"
#include "utils/VulkanMemory.hpp"
#include "utils/VulkanDescriptorUtils.hpp"

void
VulkanToScreenPipeline::init(VulkanInstance const &vkInstance,
                             uint32_t currentSwapChainNbImg,
                             uint32_t swapChainImgW,
                             uint32_t swapChainImgH,
                             VkRenderPass renderPass,
                             VkDescriptorImageInfo const &toDisplayImageInfo)
{
    _devices = vkInstance.devices;
    _cmdPools = vkInstance.cmdPools;
    _queues = vkInstance.queues;

    _toDisplayImageInfo = toDisplayImageInfo;
    _pipelineData.init(_devices, _cmdPools, _queues);
    _pipelineDescription.init(_devices);
    createDescriptorPool(currentSwapChainNbImg);
    createGfxPipeline(swapChainImgW, swapChainImgH, renderPass);
    createDescriptorSets(_pipelineData, currentSwapChainNbImg);
}

void
VulkanToScreenPipeline::resize(uint32_t currentSwapChainNbImg,
                               uint32_t swapChainImgW,
                               uint32_t swapChainImgH,
                               VkRenderPass renderPass,
                               VkDescriptorImageInfo const &toDisplayImageInfo)
{
    vkDestroyDescriptorPool(_devices.device, _descriptorPool, nullptr);
    vkDestroyPipeline(_devices.device, _gfxPipeline, nullptr);
    _pipelineDescription.clear();
    _pipelineData.clear();

    _toDisplayImageInfo = toDisplayImageInfo;
    _pipelineData.init(_devices, _cmdPools, _queues);
    _pipelineDescription.init(_devices);
    createDescriptorPool(currentSwapChainNbImg);
    createGfxPipeline(swapChainImgW, swapChainImgH, renderPass);
    createDescriptorSets(_pipelineData, currentSwapChainNbImg);
}

void
VulkanToScreenPipeline::clear()
{
    vkDestroyDescriptorPool(_devices.device, _descriptorPool, nullptr);
    vkDestroyPipeline(_devices.device, _gfxPipeline, nullptr);
    _devices = VulkanDevices{};
    _cmdPools = VulkanCommandPools{};
    _queues = VulkanQueues{};
    _toDisplayImageInfo = {};
    _pipelineDescription.clear();
    _gfxPipeline = nullptr;
    _pipelineData.clear();
    _descriptorSets.clear();
    _descriptorPool = nullptr;
}

void
VulkanToScreenPipeline::generateCommands(VkCommandBuffer cmdBuffer,
                                         size_t descriptorSetIndex)
{
    // Vertex related values
    VkBuffer vertex_buffer[] = { _pipelineData.data.buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _gfxPipeline);
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertex_buffer, offsets);
    vkCmdBindIndexBuffer(cmdBuffer,
                         _pipelineData.data.buffer,
                         _pipelineData.indicesOffset,
                         VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _pipelineDescription.pipelineLayout,
                            0,
                            1,
                            &_descriptorSets[descriptorSetIndex],
                            0,
                            nullptr);
    vkCmdDrawIndexed(cmdBuffer, _pipelineData.indicesDrawNb, 1, 0, 0, 0);
}

void
VulkanToScreenPipeline::createGfxPipeline(uint32_t swapChainImgW,
                                          uint32_t swapChainImgH,
                                          VkRenderPass renderPass)
{
    // Shaders
    auto vert_shader = loadShader(
      _devices.device, "resources/shaders/toScreen/toScreen.vert.spv");
    auto frag_shader = loadShader(
      _devices.device, "resources/shaders/toScreen/toScreen.frag.spv");

    VkPipelineShaderStageCreateInfo vert_shader_info{};
    vert_shader_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_info.module = vert_shader;
    vert_shader_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_info{};
    frag_shader_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_info.module = frag_shader;
    frag_shader_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stage_info[] = { vert_shader_info,
                                                            frag_shader_info };

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    auto binding_description =
      VulkanToScreenPipelineDescription::inputBindingDescription;
    auto attribute_description =
      VulkanToScreenPipelineDescription::inputAttributeDescription;
    vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount =
      binding_description.size();
    vertex_input_info.pVertexBindingDescriptions = binding_description.data();
    vertex_input_info.vertexAttributeDescriptionCount =
      attribute_description.size();
    vertex_input_info.pVertexAttributeDescriptions =
      attribute_description.data();

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(swapChainImgH);
    viewport.height = -static_cast<float>(swapChainImgH);
    viewport.width = static_cast<float>(swapChainImgW);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = { swapChainImgW, swapChainImgH };

    VkPipelineViewportStateCreateInfo viewport_state_info{};
    viewport_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = &scissor;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = &viewport;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer_info{};
    rasterizer_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_info.depthBiasClamp = VK_FALSE;
    rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer_info.lineWidth = 1.0f;
    rasterizer_info.cullMode = VK_CULL_MODE_NONE;
    rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer_info.depthBiasEnable = VK_FALSE;
    rasterizer_info.depthBiasConstantFactor = 0.0f;
    rasterizer_info.depthBiasClamp = 0.0f;
    rasterizer_info.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling_info{};
    multisampling_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_info.sampleShadingEnable = VK_FALSE;
    multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_info.minSampleShading = 1.0f;
    multisampling_info.pSampleMask = nullptr;
    multisampling_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_info.alphaToOneEnable = VK_FALSE;

    // Color Blending
    VkPipelineColorBlendAttachmentState color_blending_attachment{};
    color_blending_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blending_attachment.blendEnable = VK_FALSE;
    color_blending_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blending_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blending_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blending_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blending_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blending_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending_info{};
    color_blending_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending_info.logicOpEnable = VK_FALSE;
    color_blending_info.attachmentCount = 1;
    color_blending_info.pAttachments = &color_blending_attachment;
    color_blending_info.logicOp = VK_LOGIC_OP_COPY;
    color_blending_info.blendConstants[0] = 0.0f;
    color_blending_info.blendConstants[1] = 0.0f;
    color_blending_info.blendConstants[2] = 0.0f;
    color_blending_info.blendConstants[3] = 0.0f;

    // Depth
    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.minDepthBounds = 0.0f;
    depth_stencil.maxDepthBounds = 1.0f;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = {};
    depth_stencil.back = {};

    // Gfx pipeline creation
    VkGraphicsPipelineCreateInfo gfx_pipeline_info{};
    gfx_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gfx_pipeline_info.stageCount = 2;
    gfx_pipeline_info.pStages = shader_stage_info;
    gfx_pipeline_info.pVertexInputState = &vertex_input_info;
    gfx_pipeline_info.pInputAssemblyState = &input_assembly_info;
    gfx_pipeline_info.pViewportState = &viewport_state_info;
    gfx_pipeline_info.pRasterizationState = &rasterizer_info;
    gfx_pipeline_info.pMultisampleState = &multisampling_info;
    gfx_pipeline_info.pDepthStencilState = &depth_stencil;
    gfx_pipeline_info.pColorBlendState = &color_blending_info;
    gfx_pipeline_info.pDynamicState = nullptr;
    gfx_pipeline_info.layout = _pipelineDescription.pipelineLayout;
    gfx_pipeline_info.renderPass = renderPass;
    gfx_pipeline_info.subpass = 0;
    gfx_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    gfx_pipeline_info.basePipelineIndex = -1;
    if (vkCreateGraphicsPipelines(_devices.device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &gfx_pipeline_info,
                                  nullptr,
                                  &_gfxPipeline) != VK_SUCCESS) {
        throw std::runtime_error(
          "VulkanSurfaceDisplayPipeline: Failed to create graphic pipeline");
    }

    vkDestroyShaderModule(_devices.device, vert_shader, nullptr);
    vkDestroyShaderModule(_devices.device, frag_shader, nullptr);
}

void
VulkanToScreenPipeline::createDescriptorSets(
  VulkanToScreenPipelineData &pipelineData,
  uint32_t descriptorCount)
{
    static_cast<void>(pipelineData);
    allocateDescriptorSets(_devices,
                           _descriptorPool,
                           _pipelineDescription.descriptorSetLayout,
                           descriptorCount,
                           _descriptorSets);

    for (size_t i = 0; i < descriptorCount; ++i) {
        std::array<VkWriteDescriptorSet, 1> descriptor_write{};

        // Texture
        VkDescriptorImageInfo img_info{};
        img_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        img_info.imageView = _toDisplayImageInfo.imageView;
        img_info.sampler = _toDisplayImageInfo.sampler;
        descriptor_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[0].dstSet = _descriptorSets[i];
        descriptor_write[0].dstBinding = 0;
        descriptor_write[0].dstArrayElement = 0;
        descriptor_write[0].descriptorType =
          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write[0].descriptorCount = 1;
        descriptor_write[0].pBufferInfo = nullptr;
        descriptor_write[0].pImageInfo = &img_info;
        descriptor_write[0].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_devices.device,
                               descriptor_write.size(),
                               descriptor_write.data(),
                               0,
                               nullptr);
    }
}

void
VulkanToScreenPipeline::createDescriptorPool(uint32_t descriptorCount)
{
    std::array<VkDescriptorPoolSize, 1> const poolSize{
        { { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount } }
    };

    _descriptorPool =
      ::createDescriptorPool(_devices, poolSize, descriptorCount);
}