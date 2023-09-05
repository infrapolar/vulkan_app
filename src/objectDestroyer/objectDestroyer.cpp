#include "objectDestroyer.h"
#include <unordered_map>
#include <cassert>

ObjectDestroyer::ObjectDestroyer(VkInstance _instance, VkDevice _device, VkAllocationCallbacks* _pAllocator){
    instance = _instance;
    device = _device;
    pAllocator = _pAllocator;
}
ObjectDestroyer::~ObjectDestroyer(){
    destroy();
}

void ObjectDestroyer::destroy(){
    for (size_t i = 0; i < objects.size(); i++)
    {
        destroyFuncs[i](instance, device, pAllocator, objects[i]);
    }
    objects.clear();
    destroyFuncs.clear();
    if(destroyDevice){
        destroyDevice = false;
        vkDestroyDevice(device, pAllocator);
    }
    if(destroyInstance){
        destroyInstance = false;
        vkDestroyInstance(instance, pAllocator);
    }
}
void ObjectDestroyer::setDevice(VkDevice _device){
    assert(device == NULL || (objects.empty() && !destroyDevice));
    device = _device;
}
void ObjectDestroyer::setInstance(VkInstance _instance){
    assert(instance == NULL || (objects.empty() && !destroyInstance));
    instance = _instance;
}
template<> void ObjectDestroyer::destroyObject<VkDebugUtilsMessengerEXT>(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object){
    static_assert(sizeof(uint64_t) >= sizeof(VkDebugUtilsMessengerEXT));
    auto func = ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    assert(func != nullptr && "vkDestroyDebugUtilsMessengerEXT");
    func(instance, (VkDebugUtilsMessengerEXT) object, pAllocator);
}
template<> void ObjectDestroyer::destroyObject<VkDevice>(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object){
    static_assert(sizeof(uint64_t) >= sizeof(VkDevice));
    vkDestroyDevice((VkDevice)object, pAllocator);
}
template<> void ObjectDestroyer::destroyObject<VkInstance>(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object){
    static_assert(sizeof(uint64_t) >= sizeof(VkInstance));
    vkDestroyInstance((VkInstance)object, pAllocator);
}
template<> void ObjectDestroyer::destroyObject<GLFWwindow*>(VkInstance instance, VkDevice device, VkAllocationCallbacks *pAllocator, uint64_t object){
    static_assert(sizeof(uint64_t) >= sizeof(GLFWwindow*));
    glfwDestroyWindow((GLFWwindow*)object);
}

template<>
void ObjectDestroyer::add<VkInstance>(VkInstance object){
    if(instance == object){
        destroyInstance = true;
        return;
    }
    objects.push_back(reinterpret_cast<uint64_t>(object));
    destroyFuncs.push_back(destroyObject<VkInstance>);
}
template<>
void ObjectDestroyer::add<VkDevice>(VkDevice object){
    if(device == object){
        destroyDevice = true;
        return;
    }
    objects.push_back(reinterpret_cast<uint64_t>(object));
    destroyFuncs.push_back(destroyObject<VkDevice>);

}