#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../objectDestroyer/objectDestroyer.h"
VkDebugUtilsMessengerEXT createDebugUtilsMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback, const VkAllocationCallbacks* pAllocator = nullptr);
