#pragma once
#define GLFW_INCLUDE_VULKAN
#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <typeinfo>
#include <typeindex>
struct ObjectDestroyer{
    VkInstance instance;
    VkDevice device;
    VkAllocationCallbacks *pAlloc;
    std::vector<uint64_t> objects;
    std::vector<std::type_index> objectTypes;
    
    ObjectDestroyer(VkInstance instance, VkDevice device, VkAllocationCallbacks* pAlloc);
    template<typename T>
    void add(T object){
        objects.push_back(reinterpret_cast<uint64_t>(object));
        objectTypes.push_back(std::type_index(typeid(T)));
    }
    void destroy();

};