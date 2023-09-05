#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
struct ObjectDestroyer{
    typedef void (*PFN_destroy)(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object);
    VkAllocationCallbacks *pAllocator;
    std::vector<uint64_t> objects;
    std::vector<PFN_destroy> destroyFuncs;
    void setInstance(VkInstance instance);
    void setDevice(VkDevice device);
    ObjectDestroyer(VkInstance instance, VkDevice device, VkAllocationCallbacks* pAllocator);
    ~ObjectDestroyer();
    template<typename T>
    void add(T object){
        objects.push_back(reinterpret_cast<uint64_t>(object));
        destroyFuncs.push_back(destroyObject<T>);
    }
    void destroy();
    private:
    bool destroyInstance, destroyDevice;
    VkInstance instance;
    VkDevice device;
    template<typename T>
    static void destroyObject(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object);
};