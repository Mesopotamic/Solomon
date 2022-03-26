/**
 * Fair warning this will be a very messy Vulkan example
 * Almost certainly going to be terrible I just want to show you that Solomon can create a Vulkan surface for
 * you
 */

#include "Solomon.h"
#include "vulkan/vulkan.h"

#include <stdbool.h>
#include <stdio.h>

VkInstance instance;
VkSurfaceKHR surface;
VkPhysicalDevice physicalDevice;
VkDevice device;
VkQueue graphicsQueue;
VkQueue presentQueue;

void createInstance();
void postSurfaceVulkanInit();

// Solomon entry point
int SolomonMain(int argc, char* argv[])
{
    createInstance();

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
    if (SolomonUIVkCreateSurface(window, NULL, vkGetInstanceProcAddr, instance, NULL, &surface) !=
        SolomonEnumSuccess) {
        printf("Couldn't create a window via Solomon!!!\n");
        exit(-1);
    }

    postSurfaceVulkanInit();
}
void createInstance()
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

    if (vkCreateInstance(&instanceInfo, NULL, &instance) != VK_SUCCESS) {
        printf("Could not create Vulkan instance successfully\n");
        exit(-1);
    }
}

/**
 * Just do the rest of the stuff for Vulkan as lazily as possible
 */

void createPhysicalDevice()
{
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

    if (physicalDeviceCount == 0) {
        printf("No Vulkan physical devices detected!\n");
        exit(-1);
    }

    VkPhysicalDevice* physicalDevices = malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
    if (!physicalDevices) exit(-1);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    // Keep just the first one
    if (physicalDeviceCount > 1) {
        VkPhysicalDevice* temp = realloc(physicalDevices, sizeof(VkPhysicalDevice));
        if (!temp) exit(-1);
        physicalDevices = temp;
    }
    physicalDevice = *physicalDevices;
}

void createLogicalDevice()
{
    // We're looking for two queues, one for graphics and one for present
    VkDeviceQueueCreateInfo queues[2];
    bool graphics = false;
    bool present = false;

    // Get all the queue properties
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties* queueFamilyProps = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    if (!queueFamilyProps) exit(-1);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProps);

    // Loop through all of the queue family props looking for our families
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (!graphics && queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics = true;
            queues[0].queueFamilyIndex = i;
        }

        if (!present) {
            VkBool32 presentIndex = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentIndex);
            if (presentIndex == VK_TRUE) {
                present = true;
                queues[1].queueFamilyIndex = i;
            }
        }

        if (graphics && present) break;
    }

    // Ensure we only create one queue per unique queue index
    int queueCount = 2;
    if (queues[0].queueFamilyIndex == queues[1].queueFamilyIndex) queueCount = 1;
    float queueProprity = 1.0;

    // Fill in the rest of the queue details
    for (uint32_t i = 0; i < queueCount; i++) {
        queues[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues[i].queueCount = 1;
        queues[i].pQueuePriorities = &queueProprity;
        queues[i].flags = 0;
        queues[i].pNext = NULL;
    }

    VkDeviceCreateInfo deviceInfo;
    memset(&deviceInfo, 0, sizeof(VkDeviceCreateInfo));
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = queues;
    deviceInfo.queueCreateInfoCount = queueCount;

    if (vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device) != VK_SUCCESS) {
        printf("Couldn't create Logical device!\n");
        exit(-1);
    }

    vkGetDeviceQueue(device, queues[0].queueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queues[1].queueFamilyIndex, 0, &presentQueue);
}

void postSurfaceVulkanInit()
{
    createPhysicalDevice();
    createLogicalDevice();
}
