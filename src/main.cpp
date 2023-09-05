
#include <iostream>
#include <vector>
#include <cassert>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#ifndef NDEBUG
#define VALIDATION_LAYERS
#endif
#ifdef VALIDATION_LAYERS
#include "debugMessenger/debugMessenger.h"
#endif
#include "deviceCreation.h"

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
    createInfo.enabledLayerCount = 0;

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
    destroyer.add(createDebugUtilsMessenger(instance, debugCallback));
#endif
    VkDevice device = getDevice(instance);
    destroyer.setDevice(device);
    destroyer.add(device);


    while(!glfwWindowShouldClose(window))
        glfwPollEvents();
    destroyer.destroy();
    glfwTerminate();
    return 0;
}