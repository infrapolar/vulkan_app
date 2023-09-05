#include "devices.h"
#include <cassert>
#include <cstdlib>
typedef VkPhysicalDevice (*PFN_choosePhysicalDevice)(VkPhysicalDevice* devices, uint32_t size);
VkPhysicalDevice getPhysicalDevice(VkInstance instance, PFN_choosePhysicalDevice choosePhysicalDevice){
    uint32_t deviceNum;
    vkEnumeratePhysicalDevices(instance, &deviceNum, nullptr);
    assert(deviceNum != 0);
    VkPhysicalDevice device, *devices;
    devices = (VkPhysicalDevice*)std::malloc(sizeof(VkPhysicalDevice) * deviceNum);
    vkEnumeratePhysicalDevices(instance, &deviceNum, devices);
    if (choosePhysicalDevice == nullptr)
        device = devices[0];
    else
        device = choosePhysicalDevice(devices, deviceNum);
    std::free(devices);
    return device;
}