#include "XCB_Common.h"

PFN_vkCreateXcbSurfaceKHR createXCBSurface = NULL;

const char* PlatformVulkanSurfaceExtensionName = "VK_KHR_xcb_surface";

SolomonEnum PlatformUIVkCreateSurface(void* commonHandle, SolomonRect rect,
                                      PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkInstance instance,
                                      VkAllocationCallbacks* allocationCallbacks, VkSurfaceKHR* surface)
{
    // We double checked everything for us in the Solomon trampoline
    // So we can use the paramaters recieved to this function without null checking them, however we do have
    // to null pointer check our function pointer
    if (!createXCBSurface) {
        createXCBSurface = (PFN_vkCreateXcbSurfaceKHR)getInstanceProcAddr(instance, "vkCreateXcbSurfaceKHR");
        if (!createXCBSurface) {
            return SolomonEnumVkProcFail;
        }
    }

    // Now we can start creating an XCB surface create info
    VkXcbSurfaceCreateInfoKHR info;
    memset(&info, 0, sizeof(VkXcbSurfaceCreateInfoKHR));
    info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    info.connection = s_connection;
    info.window = ((SolomonWindowXCB*)commonHandle)->window_id;

    if (createXCBSurface(instance, &info, allocationCallbacks, surface) != VK_SUCCESS) {
        return SolomonEnumVkSurfaceFail;
    }

    return SolomonEnumSuccess;
}
