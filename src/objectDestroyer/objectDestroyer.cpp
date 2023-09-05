#include "objectDestroyer.h"
#include <unordered_map>
#include <cassert>
typedef void (*PFN_destroy)(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object);
static std::unordered_map<std::type_index, PFN_destroy> destroyFuncs;
static bool mapInitialized = false;


void initializeMap(){
    static_assert(sizeof(uint64_t) >= sizeof(VkDebugUtilsMessengerEXT));
    destroyFuncs[std::type_index(typeid(VkDebugUtilsMessengerEXT))] = 
        [](VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object){
            auto func = ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
            assert(func != nullptr && "vkDestroyDebugUtilsMessengerEXT");
            func(instance, (VkDebugUtilsMessengerEXT) object, pAllocator);
        };
}

ObjectDestroyer::ObjectDestroyer(VkInstance _instance, VkDevice _device, VkAllocationCallbacks* _pAlloc){
    instance = _instance;
    device = _device;
    pAlloc = _pAlloc;
    if (!mapInitialized)
        initializeMap();
    mapInitialized = true;
}
void ObjectDestroyer::destroy(){
    for (size_t i = 0; i < objects.size(); i++)
    {
        destroyFuncs[objectTypes[i]](instance, device, pAlloc, objects[i]);
    }
    objects.clear();
    objectTypes.clear();
    
}