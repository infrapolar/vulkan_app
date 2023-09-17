#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <string>
std::unordered_map<std::string, VkPipeline> getPipelines(VkDevice device, VkExtent2D swapChainExtent);