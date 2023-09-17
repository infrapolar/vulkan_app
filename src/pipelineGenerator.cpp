#include "pipelineGenerator.h"
#include <cstdlib>
#include <string_view>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
using mapped_region = boost::interprocess::mapped_region;
struct File : public std::string_view{
    File(mapped_region&& file) : std::string_view((char*)file.get_address(), file.get_size()){
        handler = std::move(file);
    }
    private:
    mapped_region handler;
};
static File mapFile(const char* filename) {
    const boost::interprocess::mode_t mode = boost::interprocess::read_write;
    boost::interprocess::file_mapping fm(filename, mode);
    return File(mapped_region(fm, mode));
}
std::unordered_map<std::string, VkPipeline> getPipelines(VkDevice device, VkExtent2D swapChainExtent){
    std::unordered_map<std::string, VkPipeline> pipelines(1);
    {
        VkShaderModule mod[2];
        VkPipelineShaderStageCreateInfo shaderInfo[2];
        {
            shaderInfo[0] = VkPipelineShaderStageCreateInfo{};
            shaderInfo[1] = VkPipelineShaderStageCreateInfo{};
            auto fragSrc = mapFile("shader.frag.spv");
            auto vertSrc = mapFile("shader.vert.spv");
            VkShaderModuleCreateInfo fragCI{}, vertCI{};
            fragCI.codeSize = fragSrc.size();
            fragCI.pCode = (uint32_t*)fragSrc.data();
            fragCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            vkCreateShaderModule(device, &fragCI, nullptr, mod);
            shaderInfo[0].pName = "main";
            shaderInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo[0].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderInfo[0].module = mod[0];
            vertCI.codeSize = vertSrc.size();
            vertCI.pCode = (uint32_t*)vertSrc.data();
            vertCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            vkCreateShaderModule(device, &vertCI, nullptr, mod+1);
            shaderInfo[1].pName = "main";
            shaderInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderInfo[1].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderInfo[1].module = mod[1];
        }
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        VkPipelineLayout pipelineLayout;
        {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // Optional
            pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
        }

        vkDestr
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyShaderModule(device, mod[0], nullptr);
        vkDestroyShaderModule(device, mod[1], nullptr);
    }
    VkGraphicsPipelineCreateInfo f{};
    return pipelines;
}