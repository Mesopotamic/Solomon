/**
 * Fair warning this will be a very messy Vulkan example
 * Almost certainly going to be terrible I just want to show you that Solomon can create a Vulkan surface for
 * you
 */

#include "Solomon.h"
#include "vulkan/vulkan.h"

#include <stdbool.h>
#include <stdio.h>

// Base Vulkan structures
VkInstance instance;
VkPhysicalDevice physicalDevice;
VkDevice device;
VkDebugUtilsMessengerEXT debugMessenger;

// Queues
uint32_t gpQueueFamilyIndexes[2];
VkQueue graphicsQueue;
VkQueue presentQueue;

// Surface
VkSurfaceKHR surface;
VkSurfaceFormatKHR swapchainFormat;
uint32_t swapLength;
VkExtent2D swapExtent;
VkSwapchainKHR swapchain;
VkImage* swapImages;
VkImageView* swapImageViews;
VkFramebuffer* framebuffers;

// Graphics pipeline
VkShaderModule vertShader;
VkShaderModule fragShader;
VkPipelineLayout graphicsLayout;
VkPipeline graphicsPipe;
VkRenderPass renderpass;

// Drawing
VkCommandPool cmdPool;
VkCommandBuffer* cmdBuffers;
VkSemaphore imageAvailble;
VkSemaphore imageFinished;
VkFence inFlight;

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

    // Enter into the Solomon windowing loop
    while (SolomonWindowShouldContinue(window)) {
        SolomonWindowEvaluateEvents(window);

        // Wait for the the next fence
        vkWaitForFences(device, 1, &inFlight, VK_TRUE, -1);
        vkResetFences(device, 1, &inFlight);

        // Get which index in the swapchain has become availble
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, -1, imageAvailble, VK_NULL_HANDLE, &imageIndex);

        // Build a submit info
        VkSubmitInfo submit;
        memset(&submit, 0, sizeof(VkSubmitInfo));
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmdBuffers[imageIndex];

        submit.pWaitSemaphores = &imageAvailble;
        submit.waitSemaphoreCount = 1;
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask = &waitStage;

        submit.pSignalSemaphores = &imageFinished;
        submit.signalSemaphoreCount = 1;

        if (vkQueueSubmit(graphicsQueue, 1, &submit, inFlight) != VK_SUCCESS) {
            printf("Could not submit the command buffer");
            exit(-1);
        }

        // Now present the finished frame
        VkPresentInfoKHR present;
        memset(&present, 0, sizeof(VkPresentInfoKHR));
        present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.pWaitSemaphores = &imageFinished;
        present.waitSemaphoreCount = 1;
        present.pSwapchains = &swapchain;
        present.swapchainCount = 1;
        present.pImageIndices = &imageIndex;

        if (vkQueuePresentKHR(presentQueue, &present) != VK_SUCCESS) {
            printf("Failed to present");
        }
    }

    // Finished the windowing loop, so now we should clean up
    vkDeviceWaitIdle(device);
    for (uint32_t i = 0; i < swapLength; i++) {
        vkDestroyFramebuffer(device, framebuffers[i], NULL);
        vkDestroyImageView(device, swapImageViews[i], NULL);
    }
    vkDestroySemaphore(device, imageAvailble, NULL);
    vkDestroySemaphore(device, imageFinished, NULL);
    vkDestroyFence(device, inFlight, NULL);
    vkDestroyCommandPool(device, cmdPool, NULL);
    vkDestroyPipeline(device, graphicsPipe, NULL);
    vkDestroyShaderModule(device, fragShader, NULL);
    vkDestroyShaderModule(device, vertShader, NULL);
    vkDestroyPipelineLayout(device, graphicsLayout, NULL);
    vkDestroyRenderPass(device, renderpass, NULL);
    vkDestroyDevice(device, NULL);

#ifndef NDEBUG
    PFN_vkDestroyDebugUtilsMessengerEXT destroy =
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    destroy(instance, debugMessenger, NULL);
#endif
    vkDestroyInstance(instance, NULL);

    free(swapImages);
    free(swapImageViews);
    free(framebuffers);
    free(cmdBuffers);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) return VK_FALSE;
    printf("validation layer: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

void createInstance()
{
    // Create the Vulkan instance create info. First thing we need to do is specify which instance
    // extensions are required. When targetting a windowing system in Vulkan you need to specify the
    // correct surface extension. Luckily Solomon can find that for us
    const char* instanceExtensions[3] = {"VK_KHR_surface", SolomonUIVkGetSurfaceName(),
                                         VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    const char* instanceLayers[1] = {"VK_LAYER_KHRONOS_validation"};

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
    instanceInfo.ppEnabledLayerNames = instanceLayers;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.flags = 0;
    instanceInfo.pNext = NULL;

    // Enable the validation layers in debug mode
#ifndef NDEBUG
    instanceInfo.enabledExtensionCount = 3;
    instanceInfo.enabledLayerCount = 1;
#endif  // !DEBUG

    if (vkCreateInstance(&instanceInfo, NULL, &instance) != VK_SUCCESS) {
        printf("Could not create Vulkan instance successfully\n");
        exit(-1);
    }

    VkDebugUtilsMessengerCreateInfoEXT debug;
    memset(&debug, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
    debug.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug.pfnUserCallback = debugCallback;
    debug.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

#ifndef NDEBUG
    PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT =
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    createDebugUtilsMessengerEXT(instance, &debug, NULL, &debugMessenger);
#endif  // !DEBUG
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

        // Store queue family indexes
        gpQueueFamilyIndexes[i] = queues[i].queueFamilyIndex;
    }

    VkDeviceCreateInfo deviceInfo;
    memset(&deviceInfo, 0, sizeof(VkDeviceCreateInfo));
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = queues;
    deviceInfo.queueCreateInfoCount = queueCount;
    const char* deviceExtensions[1] = {"VK_KHR_swapchain"};
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceInfo.enabledExtensionCount = 1;

    if (vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device) != VK_SUCCESS) {
        printf("Couldn't create Logical device!\n");
        exit(-1);
    }

    vkGetDeviceQueue(device, queues[0].queueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queues[1].queueFamilyIndex, 0, &presentQueue);
}

void createSwapchain()
{
    // Find the first supported image format
    uint32_t formatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, NULL);
    VkSurfaceFormatKHR* formats = malloc(formatsCount * sizeof(VkSurfaceFormatKHR));
    if (!formats) exit(-1);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, formats);
    swapchainFormat = formats[0];
    free(formats);

    // Get how many images we need in the swapchain
    VkSurfaceCapabilitiesKHR capablities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capablities);
    swapLength = capablities.minImageCount;

    VkSwapchainCreateInfoKHR info;
    memset(&info, 0, sizeof(VkSwapchainCreateInfoKHR));
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;

    // Attach the format
    info.imageFormat = swapchainFormat.format;
    info.imageColorSpace = swapchainFormat.colorSpace;

    // Guarenteed present mode
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Bound the extent
    swapExtent.height = 360;
    swapExtent.width = 720;
    if (swapExtent.height > capablities.maxImageExtent.height)
        swapExtent.height = capablities.maxImageExtent.height;
    if (swapExtent.width > capablities.maxImageExtent.width)
        swapExtent.width = capablities.maxImageExtent.width;
    info.imageExtent = swapExtent;
    info.minImageCount = swapLength;

    // If the graphics and present queue are different queues then we have to tell the swap images that they
    // need to be shared between queues
    if (gpQueueFamilyIndexes[0] == gpQueueFamilyIndexes[1]) {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = gpQueueFamilyIndexes;
    }

    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;
    info.preTransform = capablities.currentTransform;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (vkCreateSwapchainKHR(device, &info, NULL, &swapchain) != VK_SUCCESS) {
        printf("Failed to create a swapchain");
        exit(-1);
    }

    // Retrieve those images
    vkGetSwapchainImagesKHR(device, swapchain, &swapLength, NULL);
    swapImages = malloc(swapLength * sizeof(VkImage));
    if (!swapImages) exit(-1);
    vkGetSwapchainImagesKHR(device, swapchain, &swapLength, swapImages);

    // Produce an image view for those images
    VkImageViewCreateInfo view;
    memset(&view, 0, sizeof(VkImageViewCreateInfo));
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = swapchainFormat.format;
    view.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel = 0;
    view.subresourceRange.levelCount = 1;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount = 1;

    swapImageViews = malloc(swapLength * sizeof(VkImageView));
    if (!swapImageViews) exit(-1);

    for (uint32_t i = 0; i < swapLength; i++) {
        view.image = swapImages[i];
        if (vkCreateImageView(device, &view, NULL, &swapImageViews[i]) != VK_SUCCESS) {
            printf("Couldn't create image view %i", i);
            exit(-1);
        }
    }
}

void createRenderPass()
{
    // Onscreen attachment description
    VkAttachmentDescription attach;
    memset(&attach, 0, sizeof(VkAttachmentDescription));
    attach.format = swapchainFormat.format;
    attach.samples = VK_SAMPLE_COUNT_1_BIT;
    attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Subpass

    VkAttachmentReference ref;
    ref.attachment = 0;
    ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription desc;
    memset(&desc, 0, sizeof(VkSubpassDescription));
    desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    desc.pColorAttachments = &ref;
    desc.colorAttachmentCount = 1;

    VkSubpassDependency dep;
    memset(&dep, 0, sizeof(VkSubpassDependency));
    dep.srcSubpass = VK_SUBPASS_EXTERNAL;
    dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.srcAccessMask = 0;

    dep.dstSubpass = 0;
    dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info;
    memset(&info, 0, sizeof(VkRenderPassCreateInfo));
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pSubpasses = &desc;
    info.subpassCount = 1;
    info.pAttachments = &attach;
    info.attachmentCount = 1;
    info.pDependencies = &dep;
    info.dependencyCount = 1;

    if (vkCreateRenderPass(device, &info, NULL, &renderpass) != VK_SUCCESS) {
        printf("Couldn't create a renderpass");
        exit(-1);
    }

    // Create framebuffers for the renderpass
    // And place those image views into a framebuffer
    framebuffers = malloc(swapLength * sizeof(VkFramebuffer));
    if (!framebuffers) exit(-1);

    VkFramebufferCreateInfo fb;
    memset(&fb, 0, sizeof(VkFramebufferCreateInfo));
    fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb.attachmentCount = 1;
    fb.renderPass = renderpass;
    fb.width = swapExtent.width;
    fb.height = swapExtent.height;
    fb.layers = 1;

    for (uint32_t i = 0; i < swapLength; i++) {
        fb.pAttachments = &swapImageViews[i];
        if (vkCreateFramebuffer(device, &fb, NULL, &framebuffers[i]) != VK_SUCCESS) {
            printf("Couldn't create framebuffer %i", i);
            exit(-1);
        }
    }
}

void createShaders()
{
    VkShaderModuleCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = NULL;
    info.flags = 0;

    FILE* file = fopen("Shader.vert.spv", "rb");
    if (!file) exit(-1);
    fseek(file, 0, SEEK_END);
    info.codeSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint32_t* data = malloc(info.codeSize * sizeof(uint32_t));
    if (!data) exit(-1);
    fread(data, sizeof(uint32_t), info.codeSize, file);

    info.pCode = data;
    if (vkCreateShaderModule(device, &info, NULL, &vertShader) != VK_SUCCESS) {
        printf("Failed to create shader module\n");
        exit(-1);
    }

    // Do the exact same thing for the fragment shader
    free(data);
    fclose(file);

    file = fopen("Shader.frag.spv", "rb");
    if (!file) exit(-1);
    fseek(file, 0, SEEK_END);
    info.codeSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    data = malloc(info.codeSize * sizeof(uint32_t));
    if (!data) exit(-1);
    fread(data, sizeof(uint32_t), info.codeSize, file);
    info.pCode = data;

    if (vkCreateShaderModule(device, &info, NULL, &fragShader) != VK_SUCCESS) {
        printf("Failed to create shader module\n");
        exit(-1);
    }
    free(data);
    fclose(file);
}

void createGraphicsPipeline()
{
    // This one is gonna suck I'm sorry

    // How to interprate the input data
    VkPipelineVertexInputStateCreateInfo vertex;
    memset(&vertex, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
    vertex.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo input;
    memset(&input, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
    input.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkRect2D scissor;
    scissor.extent = swapExtent;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    VkViewport viewport;
    viewport.x = 0.0;
    viewport.y = 0.0;
    viewport.height = swapExtent.height;
    viewport.width = swapExtent.width;
    viewport.minDepth = 0.0;
    viewport.maxDepth = 1.0;

    VkPipelineViewportStateCreateInfo viewportState;
    memset(&viewportState, 0, sizeof(VkPipelineViewportStateCreateInfo));
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pScissors = &scissor;
    viewportState.scissorCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.viewportCount = 1;

    // Rasteriser
    VkPipelineRasterizationStateCreateInfo raster;
    memset(&raster, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.lineWidth = 1.0;
    raster.cullMode = VK_CULL_MODE_BACK_BIT;
    raster.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multi;
    memset(&multi, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
    multi.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multi.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Colour blending
    VkPipelineColorBlendAttachmentState attachState;
    memset(&attachState, 0, sizeof(VkPipelineColorBlendAttachmentState));
    attachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                 VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blend;
    memset(&blend, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
    blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.pAttachments = &attachState;
    blend.attachmentCount = 1;

    VkPipelineLayoutCreateInfo pipeLayout;
    memset(&pipeLayout, 0, sizeof(VkPipelineLayoutCreateInfo));
    pipeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    if (vkCreatePipelineLayout(device, &pipeLayout, NULL, &graphicsLayout) != VK_SUCCESS) {
        printf("Couldn't create a graphics pipeline layout");
        exit(-1);
    }

    // Shader stages
    VkPipelineShaderStageCreateInfo stages[2];
    memset(stages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));
    for (uint32_t i = 0; i < 2; i++) {
        stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[i].pName = "main";
    }
    stages[0].module = vertShader;
    stages[1].module = fragShader;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo info;
    memset(&info, 0, sizeof(VkGraphicsPipelineCreateInfo));
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pStages = stages;
    info.stageCount = 2;
    info.pVertexInputState = &vertex;
    info.pInputAssemblyState = &input;
    info.pRasterizationState = &raster;
    info.pViewportState = &viewportState;
    info.pMultisampleState = &multi;
    info.pColorBlendState = &blend;
    info.layout = graphicsLayout;
    info.renderPass = renderpass;

    if (vkCreateGraphicsPipelines(device, NULL, 1, &info, NULL, &graphicsPipe) != VK_SUCCESS) {
        printf("Failed to create graphics pipeline");
        exit(-1);
    }
}

void createCmdBuffers()
{
    VkCommandPoolCreateInfo pool;
    memset(&pool, 0, sizeof(VkCommandPoolCreateInfo));
    pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool.queueFamilyIndex = gpQueueFamilyIndexes[0];

    if (vkCreateCommandPool(device, &pool, NULL, &cmdPool) != VK_SUCCESS) {
        printf("Failed to create a command pool");
        exit(-1);
    }

    VkCommandBufferAllocateInfo alloc;
    alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.commandBufferCount = swapLength;
    alloc.commandPool = cmdPool;
    alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.pNext = NULL;

    cmdBuffers = malloc(swapLength * sizeof(VkCommandBuffer));
    if (!cmdBuffers) exit(-1);
    if (vkAllocateCommandBuffers(device, &alloc, cmdBuffers) != VK_SUCCESS) {
        printf("Failed to allocate command buffers");
        exit(-1);
    }

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    VkCommandBufferBeginInfo begin;
    memset(&begin, 0, sizeof(VkCommandBufferBeginInfo));
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo rp;
    memset(&rp, 0, sizeof(VkRenderPassBeginInfo));
    rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp.renderPass = renderpass;
    rp.renderArea.extent = swapExtent;
    rp.pClearValues = &clearColor;
    rp.clearValueCount = 1;

    for (uint32_t i = 0; i < swapLength; i++) {
        if (vkBeginCommandBuffer(cmdBuffers[i], &begin) != VK_SUCCESS) {
            printf("Couldn't start recording buffer %i", i);
            exit(-1);
        }

        rp.framebuffer = framebuffers[i];
        vkCmdBeginRenderPass(cmdBuffers[i], &rp, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipe);

        vkCmdDraw(cmdBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(cmdBuffers[i]);

        if (vkEndCommandBuffer(cmdBuffers[i]) != VK_SUCCESS) {
            printf("Failed to end the command buffer");
            exit(-1);
        }
    }
}

void createSyncObjects()
{
    VkSemaphoreCreateInfo sem;
    sem.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    sem.flags = 0;
    sem.pNext = NULL;

    VkFenceCreateInfo fence;
    fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fence.pNext = NULL;

    if (vkCreateSemaphore(device, &sem, NULL, &imageAvailble) != VK_SUCCESS ||
        vkCreateSemaphore(device, &sem, NULL, &imageFinished) != VK_SUCCESS ||
        vkCreateFence(device, &fence, NULL, &inFlight) != VK_SUCCESS) {
        printf("Failed to make sync objects");
        exit(-1);
    }
}

void postSurfaceVulkanInit()
{
    createPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createRenderPass();
    createShaders();
    createGraphicsPipeline();
    createCmdBuffers();
    createSyncObjects();
}
