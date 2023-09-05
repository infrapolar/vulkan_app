#include "objectDestroyer.h"
#include <unordered_map>
#include <cassert>

ObjectDestroyer::ObjectDestroyer(VkInstance _instance, VkDevice _device, VkAllocationCallbacks* _pAlloc){
    instance = _instance;
    device = _device;
    pAlloc = _pAlloc;
}

void ObjectDestroyer::destroy(){
    for (size_t i = 0; i < objects.size(); i++)
    {
        destroyFuncs[i](instance, device, pAlloc, objects[i]);
    }
    objects.clear();
    destroyFuncs.clear();
    
}

template<> void ObjectDestroyer::destroyObject<VkDebugUtilsMessengerEXT>(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object){
    static_assert(sizeof(uint64_t) >= sizeof(VkDebugUtilsMessengerEXT));
    auto func = ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    assert(func != nullptr && "vkDestroyDebugUtilsMessengerEXT");
    func(instance, (VkDebugUtilsMessengerEXT) object, pAllocator);
}