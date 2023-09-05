#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
struct ObjectDestroyer{
    typedef void (*PFN_destroy)(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object);
    VkInstance instance;
    VkDevice device;
    VkAllocationCallbacks *pAlloc;
    std::vector<uint64_t> objects;
    std::vector<PFN_destroy> destroyFuncs;
    
    ObjectDestroyer(VkInstance instance, VkDevice device, VkAllocationCallbacks* pAlloc);
    template<typename T>
    void add(T object){
        objects.push_back(reinterpret_cast<uint64_t>(object));
        destroyFuncs.push_back(destroyObject<T>);
    }
    void destroy();
    private:
    template<typename T>
    static void destroyObject(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object);
};