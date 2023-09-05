#pragma once
#include <vulkan/vulkan.h>
VkPhysicalDevice getPhysicalDevice(VkInstance instance, VkPhysicalDevice (*choosePhysicalDevice)(VkPhysicalDevice* devices, uint32_t size));
VkDevice getLogicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);