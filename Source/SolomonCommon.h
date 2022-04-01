/*************************************************************************************************************
 * Solomon internal common things that are shared across platforms
 *************************************************************************************************************/
#ifndef __SOLOMON_INTERNAL_COMMON_H__
#define __SOLOMON_INTERNAL_COMMON_H__ (1)
#include <stdbool.h>
#include "Solomon.h"

// Solomon Vulkan without prototypes
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

typedef struct SolomonInternalRect {
    int x;
    int y;
    int w;
    int h;
    void* extras;
} SolomonInternalRect;

typedef struct SolomonWindowCommon {
    SolomonInternalRect rect;
    char* title;
    bool windowInit;
    bool shouldContinue;
    SolomonKeyEventHandler keyHandler;
} SolomonWindowCommon;

/**
 * We parse off into the platform specific stuff here, platform specific code has the Platform instead of
 * Solomon prefix. These are called by our trampolines after they have done all the common stuff.
 *
 * If you want to port solomon to a new windowing system you need to implement all of the functions declared
 * below and also the following extras:
 *  -> A private Window handle
 *  -> size_t SolomonWindowSize() to return the size of the handle above
 */

/**
 * @brief Gets the internal size of the window in bytes
 * @returns Size of internal window handle
 */
size_t PlatformWindowSize();

/**
 * @brief Hands off the rest of the window creation to the platform dependant part
 * @param commonHandle void pointer to the opaque handle
 * @returns Solomon successcode
 */
SolomonEnum PlatformWindowCreate(void* commonHandle);

/**
 * @breif Attempt to make the window visible to the user
 * @param commonHandle void pointer to the opaque handle
 * @returns Solomon success code
 */
SolomonEnum PlatformWindowShow(void* commonHandle);

/**
 * @brief Evaluates the events that have happened since the last time this function was called, should be
 * called at least once a frame to check for exiting the window loop. Needs to handle if the user has
 * requested a close, if the operating system has requested a close, when a key has been pressed
 * @param window SolomonWindow handle for the window to process events for
 * @returns Success code
 */
SolomonEnum PlatformWindowEvaluateEvents(SolomonWindow window);

/**
 * Extern for the Vulkan surface name, must be defined in the platform specific source files
 */
extern const char* PlatformVulkanSurfaceExtensionName;

SolomonEnum PlatformUIVkCreateSurface(void* commonHandle, SolomonRect rect,
                                      PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkInstance instance,
                                      VkAllocationCallbacks* allocationCallbacks, VkSurfaceKHR* surface);

#endif  // !__SOLOMON_INTERNAL_COMMON_H__
