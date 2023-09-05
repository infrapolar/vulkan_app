#pragma once
#include <vulkan/vulkan.h>
#include "../objectDestroyer/objectDestroyer.h"
VkDebugUtilsMessengerEXT createDebugUtilsMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback, const VkAllocationCallbacks* pAllocator = nullptr);
