#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
VkPhysicalDevice getPhysicalDevice(VkInstance instance, VkPhysicalDevice (*choosePhysicalDevice)(VkPhysicalDevice* devices, uint32_t size));
VkDevice getLogicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);