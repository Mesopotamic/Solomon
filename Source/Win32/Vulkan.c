#include "SolomonWin32.h"

const char* PlatformVulkanSurfaceExtensionName = "VK_KHR_win32_surface";

PFN_vkCreateWin32SurfaceKHR vkCreateWin32Surface = NULL;

SolomonEnum PlatformUIVkCreateSurface(void* commonHandle, SolomonRect rect,
                                      PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkInstance instance,
                                      VkAllocationCallbacks* allocationCallbacks, VkSurfaceKHR* surface)
{
    if (!vkCreateWin32Surface) {
        vkCreateWin32Surface =
          (PFN_vkCreateWin32SurfaceKHR)getInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");

        if (!vkCreateWin32Surface) {
            return SolomonEnumVkProcFail;
        }
    }

    // We got this far lets construct a surface create info for the win32 create info
    VkWin32SurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = s_hinstance;
    surfaceInfo.hwnd = ((SolomonWindowWin32*)commonHandle)->hwnd;
    surfaceInfo.flags = 0;
    surfaceInfo.pNext = NULL;

    if (vkCreateWin32Surface(instance, &surfaceInfo, allocationCallbacks, surface) != VK_SUCCESS) {
        return SolomonEnumVkSurfaceFail;
    }

    return SolomonEnumSuccess;
}
