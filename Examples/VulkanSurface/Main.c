/**
 * Fair warning this will be a very messy Vulkan example
 * Almost certainly going to be terrible I just want to show you that Solomon can create a Vulkan surface for
 * you
 */

#include "Solomon.h"
#include "vulkan/vulkan.h"

#include <stdio.h>

VkInstance createInstance();

// Solomon entry point
int SolomonMain(int argc, char* argv[])
{
    VkInstance instance = createInstance();

    // We now need to start one Solomon window in order to have a surface context to create
    SolomonWindow window = SolomonWindowCreate(0, 0, 720, 360, "Vulkan Surface");
    SolomonWindowShow(window);

    // Now let Solomon create the surface for us
    // The arguments are as follows
    // window, Solomon window handle
    // NULL, Solomon rect handle we pass null to use the window as the rect
    // vkGetInstanceProcAddr, Function pointer provided by Vulkan
    // instance, Our Vulkan instance used to create the Surface
    // NULL, List of allocators, we're not using any
    // surface, pointer to the returned surface
    VkSurfaceKHR surface;
    if (SolomonUIVkCreateSurface(window, NULL, vkGetInstanceProcAddr, instance, NULL, &surface) !=
        SolomonEnumSuccess) {
        printf("Couldn't create a window via Solomon!!!\n");
        exit(-1);
    }
}
VkInstance createInstance()
{
    // Create the Vulkan instance create info. First thing we need to do is specify which instance
    // extensions are required. When targetting a windowing system in Vulkan you need to specify the
    // correct surface extension. Luckily Solomon can find that for us
    const char* instanceExtensions[2] = {"VK_KHR_surface", SolomonUIVkGetSurfaceName()};

    // We need an app info to tell Vulkan what our app is doing
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_VERSION_1_0;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pApplicationName = "Vulkan Surface";
    appInfo.pEngineName = "Solomon";
    appInfo.pNext = NULL;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.enabledExtensionCount = 2;
    instanceInfo.ppEnabledExtensionNames = instanceExtensions;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.flags = 0;
    instanceInfo.pNext = NULL;

    VkInstance instance;
    if (vkCreateInstance(&instanceInfo, NULL, &instance) != VK_SUCCESS) {
        printf("Could not create Vulkan instance successfully\n");
        exit(-1);
    }
    return instance;
}
