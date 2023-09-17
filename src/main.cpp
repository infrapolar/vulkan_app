
#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#ifndef NDEBUG
#define VALIDATION_LAYERS
#endif
#ifdef VALIDATION_LAYERS
#include "debugMessenger/debugMessenger.h"
#endif
#include "deviceCreation.h"
#include "devices/devices.h"
#include "pipelineGenerator.h"
using namespace std;
static int width = 600, height = 600;
vector<const char*> getExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    auto extensions = vector<const char*>(glfwExtensions, glfwExtensions+glfwExtensionCount);
#ifdef VALIDATION_LAYERS
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    return extensions;
}
vector<const char*> getValidationLayers(){
    return vector<const char*> {"VK_LAYER_KHRONOS_validation"};
}


VkInstance createInstance(const char** extensions, uint32_t extensionsCount, const char** validationLayers = nullptr, uint32_t validationLayersCount = 0){
    VkInstance instance;
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionsCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.enabledLayerCount = validationLayersCount;
    createInfo.ppEnabledLayerNames = validationLayers;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    assert(result == VK_SUCCESS && "VkInstance doesn't created");
    return instance;

}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
int main(){
#ifndef NDEBUG
    cout << "DEBUG";
#endif
    cout << endl;
    bool succes = glfwInit();
    assert(succes && "GLFW doesn`t init");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(width, height, "test", NULL, NULL);
    assert(window != nullptr);
    glfwMakeContextCurrent(window);
    auto extensions = getExtensions();
#ifdef VALIDATION_LAYERS
    auto validationLayers = getValidationLayers();
    VkInstance instance = createInstance(extensions.data(), extensions.size(), validationLayers.data(), validationLayers.size());
#else
    VkInstance instance = createInstance(extensions.data(), extensions.size());
#endif

    ObjectDestroyer destroyer(instance, NULL, nullptr);
    destroyer.add(instance);
    destroyer.add(window);
#ifdef VALIDATION_LAYERS
    auto messenger = createDebugUtilsMessenger(instance, debugCallback);
    destroyer.add(messenger);
#endif
    //TODO: Write chooseDevice function
    VkPhysicalDevice physDevice = getPhysicalDevice(instance, nullptr);
    //TODO: Write normal getDevice in devices/devices.cpp
    VkDevice device = getDevice(physDevice);
    destroyer.setDevice(device);
    destroyer.add(device);
    //TODO: Add present Queue
    VkQueue graphicalQueue;
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance, window, nullptr, &surface);
    VkSwapchainKHR ourSwapChain;
    VkSurfaceFormatKHR ourFormat;
    VkExtent2D ourExtent{};
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &capabilities);
        {
            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &format_count, nullptr);
            vector<VkSurfaceFormatKHR> formats(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &format_count, formats.data());
            ourFormat = formats[0];
            for (const auto& availableFormat : formats) {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    ourFormat = availableFormat;
                    break;
                }
            }
        }
        VkSwapchainCreateInfoKHR createInfo{};
        ourExtent.height = height; //not quaranted
        ourExtent.width = width;
        createInfo.minImageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount == capabilities.minImageCount)
            createInfo.minImageCount--;
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.imageExtent = ourExtent;
        createInfo.imageFormat = ourFormat.format;
        createInfo.imageColorSpace = ourFormat.colorSpace;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;//quaranted, but MAILBOX better

        vkCreateSwapchainKHR(device, &createInfo, nullptr, &ourSwapChain);
        
    }
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, ourSwapChain, &imageCount, nullptr);
    std::vector<VkImage> swapChainImages(imageCount);
    vkGetSwapchainImagesKHR(device, ourSwapChain, &imageCount, swapChainImages.data());
    std::vector<VkImageView> imageViews(imageCount);
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.format = ourFormat.format;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        for (uint32_t i = 0; i < imageCount; i++)
        {
            createInfo.image = swapChainImages[i];
            vkCreateImageView(device, &createInfo, nullptr, imageViews.data()+i);
            destroyer.add(imageViews[i]);
        }
    }
    destroyer.add(ourSwapChain);
    destroyer.add(surface);
    getPipelines(device, ourExtent);
    while(!glfwWindowShouldClose(window))
        glfwPollEvents();
    destroyer.destroy();
    glfwTerminate();
    return 0;
}