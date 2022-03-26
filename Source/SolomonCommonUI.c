#include "SolomonCommon.h"

const char* SolomonUIVkGetSurfaceName()
{
    // Return the extern that will be defined for the platform
    return PlatformVulkanSurfaceExtensionName;
}

SolomonEnum SolomonUIVkCreateSurface(SolomonWindow window, SolomonRect rect, void* getInstanceProcAddr,
                                     void* instance, void** allocationCallbacks, void** surfaceKHR)
{
    // Do all the null pointer checls, vkAlllocationCallbacks can be null
    if (!window) return SolomonEnumSegFail;
    if (!getInstanceProcAddr) return SolomonEnumSegFail;
    if (!instance) return SolomonEnumSegFail;
    if (!surfaceKHR) return SolomonEnumSegFail;

    // Call into the platform specific variation, which also casts the void pointers into their respective
    // Vulkan objects
    return PlatformUIVkCreateSurface(window, rect, getInstanceProcAddr, instance, allocationCallbacks,
                                     surfaceKHR);
}
