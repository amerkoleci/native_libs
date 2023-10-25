// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_gpu_internal.h"

#if defined(ALIMER_VULKAN)
#include <vulkan/vulkan.h>

#ifdef _WIN32
#   include <windows.h>
#else
#   include <dlfcn.h>
#endif

#define VMA_STATS_STRING_ENABLED 0
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
ALIMER_DISABLE_WARNINGS()
#include "third_party/vk_mem_alloc.h"
//#include "third_party/spirv_reflect.h"
ALIMER_ENABLE_WARNINGS()

#include <stdio.h>
#include <string.h>
#include <mutex>
#include <deque>
#include <vector>

// Functions that don't require an instance
#define GPU_FOREACH_ANONYMOUS(X)\
    X(vkEnumerateInstanceLayerProperties)\
    X(vkEnumerateInstanceExtensionProperties)\
    X(vkCreateInstance)\
    X(vkEnumerateInstanceVersion)

// Functions that require an instance but don't require a device
#define GPU_FOREACH_INSTANCE(X)\
    X(vkDestroyInstance)\
    X(vkCreateDebugUtilsMessengerEXT)\
    X(vkDestroyDebugUtilsMessengerEXT)\
    X(vkDestroySurfaceKHR)\
    X(vkEnumeratePhysicalDevices)\
    X(vkGetPhysicalDeviceProperties)\
    X(vkGetPhysicalDeviceMemoryProperties)\
    X(vkGetPhysicalDeviceProperties2)\
    X(vkGetPhysicalDeviceFeatures2)\
    X(vkGetPhysicalDeviceMemoryProperties2)\
    X(vkGetPhysicalDeviceFormatProperties)\
    X(vkGetPhysicalDeviceQueueFamilyProperties2)\
    X(vkGetPhysicalDeviceSurfaceSupportKHR)\
    X(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)\
    X(vkGetPhysicalDeviceSurfaceFormatsKHR)\
    X(vkGetPhysicalDeviceSurfacePresentModesKHR)\
    X(vkEnumerateDeviceExtensionProperties)\
    X(vkCreateDevice) \
    X(vkGetDeviceProcAddr)\
    X(vkCmdBeginDebugUtilsLabelEXT)\
    X(vkCmdEndDebugUtilsLabelEXT)\
    X(vkCmdInsertDebugUtilsLabelEXT)

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define GPU_FOREACH_INSTANCE_PLATFORM(X)\
    X(vkCreateAndroidSurfaceKHR)
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
#define GPU_FOREACH_INSTANCE_PLATFORM(X)\
    X(vkCreateWin32SurfaceKHR)\
    X(vkGetPhysicalDeviceWin32PresentationSupportKHR)
#elif defined(VK_USE_PLATFORM_METAL_EXT)
#define GPU_FOREACH_INSTANCE_PLATFORM(X)\
    X(vkCreateMetalSurfaceEXT)
#elif defined(VK_USE_PLATFORM_XCB_KHR)
#define GPU_FOREACH_INSTANCE_PLATFORM(X)\
    X(vkCreateXcbSurfaceKHR)\
    X(vkGetPhysicalDeviceXcbPresentationSupportKHR)
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
#define GPU_FOREACH_INSTANCE_PLATFORM(X)\
    X(vkCreateXlibSurfaceKHR)\
    X(vkGetPhysicalDeviceXlibPresentationSupportKHR)
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
#define GPU_FOREACH_INSTANCE_PLATFORM(X)\
    X(vkCreateWaylandSurfaceKHR)\
    X(vkGetPhysicalDeviceWaylandPresentationSupportKHR)
#endif

// Functions that require a device
#define GPU_FOREACH_DEVICE(X)\
    X(vkDestroyDevice)\
    X(vkGetDeviceQueue)\
    X(vkSetDebugUtilsObjectNameEXT)\
    X(vkDeviceWaitIdle)\
    X(vkQueueSubmit)\
    X(vkQueuePresentKHR)\
    X(vkCreateSwapchainKHR)\
    X(vkDestroySwapchainKHR)\
    X(vkGetSwapchainImagesKHR)\
    X(vkAcquireNextImageKHR)\
    X(vkCreateCommandPool)\
    X(vkDestroyCommandPool)\
    X(vkResetCommandPool)\
    X(vkAllocateCommandBuffers)\
    X(vkBeginCommandBuffer)\
    X(vkEndCommandBuffer)\
    X(vkCreateFence)\
    X(vkDestroyFence)\
    X(vkResetFences)\
    X(vkGetFenceStatus)\
    X(vkWaitForFences)\
    X(vkCreateSemaphore)\
    X(vkDestroySemaphore)\
    X(vkCmdPipelineBarrier)\
    X(vkCreateQueryPool)\
    X(vkDestroyQueryPool)\
    X(vkCmdResetQueryPool)\
    X(vkCmdBeginQuery)\
    X(vkCmdEndQuery)\
    X(vkCmdWriteTimestamp)\
    X(vkCmdCopyQueryPoolResults)\
    X(vkGetQueryPoolResults)\
    X(vkCreateBuffer)\
    X(vkDestroyBuffer)\
    X(vkGetBufferMemoryRequirements)\
    X(vkBindBufferMemory)\
    X(vkCreateImage)\
    X(vkDestroyImage)\
    X(vkGetImageMemoryRequirements)\
    X(vkBindImageMemory)\
    X(vkCmdCopyBuffer)\
    X(vkCmdCopyImage)\
    X(vkCmdBlitImage)\
    X(vkCmdCopyBufferToImage)\
    X(vkCmdCopyImageToBuffer)\
    X(vkCmdFillBuffer)\
    X(vkCmdClearColorImage)\
    X(vkCmdClearDepthStencilImage)\
    X(vkAllocateMemory)\
    X(vkFreeMemory)\
    X(vkMapMemory)\
    X(vkUnmapMemory)\
    X(vkFlushMappedMemoryRanges)\
    X(vkInvalidateMappedMemoryRanges)\
    X(vkCreateSampler)\
    X(vkDestroySampler)\
    X(vkCreateRenderPass2KHR)\
    X(vkDestroyRenderPass)\
    X(vkCmdBeginRenderPass2KHR)\
    X(vkCmdEndRenderPass2KHR)\
    X(vkCreateImageView)\
    X(vkDestroyImageView)\
    X(vkCreateFramebuffer)\
    X(vkDestroyFramebuffer)\
    X(vkCreateShaderModule)\
    X(vkDestroyShaderModule)\
    X(vkCreateDescriptorSetLayout)\
    X(vkDestroyDescriptorSetLayout)\
    X(vkCreatePipelineLayout)\
    X(vkDestroyPipelineLayout)\
    X(vkCreateDescriptorPool)\
    X(vkDestroyDescriptorPool)\
    X(vkAllocateDescriptorSets)\
    X(vkResetDescriptorPool)\
    X(vkUpdateDescriptorSets)\
    X(vkCreatePipelineCache)\
    X(vkDestroyPipelineCache)\
    X(vkGetPipelineCacheData)\
    X(vkCreateGraphicsPipelines)\
    X(vkCreateComputePipelines)\
    X(vkDestroyPipeline)\
    X(vkCmdSetViewport)\
    X(vkCmdSetScissor)\
    X(vkCmdPushConstants)\
    X(vkCmdBindPipeline)\
    X(vkCmdBindDescriptorSets)\
    X(vkCmdBindVertexBuffers)\
    X(vkCmdBindIndexBuffer)\
    X(vkCmdDraw)\
    X(vkCmdDrawIndexed)\
    X(vkCmdDrawIndirect)\
    X(vkCmdDrawIndexedIndirect)\
    X(vkCmdDispatch)\
    X(vkCmdDispatchIndirect)\
    X(vkGetBufferMemoryRequirements2)\
    X(vkGetImageMemoryRequirements2)\
    X(vkBindBufferMemory2)\
    X(vkBindImageMemory2)

#define GPU_FOREACH_DEVICE_SYNCHRONIZATION2(X)\
    X(vkQueueSubmit2)\

#define GPU_DECLARE(fn) static PFN_##fn fn;
// Used to load/declare Vulkan functions without lots of clutter
#define GPU_LOAD_ANONYMOUS(fn) fn = (PFN_##fn) vkGetInstanceProcAddr(NULL, #fn);
#define GPU_LOAD_INSTANCE(fn) fn = (PFN_##fn) vkGetInstanceProcAddr(state.instance, #fn);
#define GPU_LOAD_DEVICE(fn) fn = (PFN_##fn) vkGetDeviceProcAddr(state.device, #fn);

GPU_DECLARE(vkGetInstanceProcAddr)
GPU_FOREACH_ANONYMOUS(GPU_DECLARE)
GPU_FOREACH_INSTANCE(GPU_DECLARE)
GPU_FOREACH_INSTANCE_PLATFORM(GPU_DECLARE)
GPU_FOREACH_DEVICE(GPU_DECLARE)
GPU_FOREACH_DEVICE_SYNCHRONIZATION2(GPU_DECLARE)

namespace
{
    static constexpr uint32_t kVulkanBindingShiftBuffer = 0u;
    static constexpr uint32_t kVulkanBindingShiftSRV = 1000u;
    static constexpr uint32_t kVulkanBindingShiftUAV = 2000u;
    static constexpr uint32_t kVulkanBindingShiftSampler = 3000u;

    //static_assert(sizeof(ClearColorValue) == sizeof(VkClearColorValue), "ClearColorValue mismatch");
    //static_assert(offsetof(ClearColorValue, float32) == offsetof(VkClearColorValue, float32), "ClearColorValue mismatch");
    //static_assert(offsetof(ClearColorValue, int32) == offsetof(VkClearColorValue, int32), "ClearColorValue mismatch");
    //static_assert(offsetof(ClearColorValue, uint32) == offsetof(VkClearColorValue, uint32), "ClearColorValue mismatch");

    static_assert(sizeof(GPUViewport) == sizeof(VkViewport), "Viewport mismatch");
    static_assert(offsetof(GPUViewport, x) == offsetof(VkViewport, x), "Layout mismatch");
    static_assert(offsetof(GPUViewport, y) == offsetof(VkViewport, y), "Layout mismatch");
    static_assert(offsetof(GPUViewport, width) == offsetof(VkViewport, width), "Layout mismatch");
    static_assert(offsetof(GPUViewport, height) == offsetof(VkViewport, height), "Layout mismatch");
    static_assert(offsetof(GPUViewport, minDepth) == offsetof(VkViewport, minDepth), "Layout mismatch");
    static_assert(offsetof(GPUViewport, maxDepth) == offsetof(VkViewport, maxDepth), "Layout mismatch");

    //static_assert(sizeof(Rect) == sizeof(VkRect2D), "Rect mismatch");
    //static_assert(offsetof(Rect, x) == offsetof(VkRect2D, offset.x), "Layout mismatch");
    //static_assert(offsetof(Rect, y) == offsetof(VkRect2D, offset.y), "Layout mismatch");
    //static_assert(offsetof(Rect, width) == offsetof(VkRect2D, extent.width), "Layout mismatch");
    //static_assert(offsetof(Rect, height) == offsetof(VkRect2D, extent.height), "Layout mismatch");

    static_assert(sizeof(GPUDispatchIndirectCommand) == sizeof(VkDispatchIndirectCommand), "DispatchIndirectCommand mismatch");
    static_assert(offsetof(GPUDispatchIndirectCommand, x) == offsetof(VkDispatchIndirectCommand, x), "Layout mismatch");
    static_assert(offsetof(GPUDispatchIndirectCommand, y) == offsetof(VkDispatchIndirectCommand, y), "Layout mismatch");
    static_assert(offsetof(GPUDispatchIndirectCommand, z) == offsetof(VkDispatchIndirectCommand, z), "Layout mismatch");

    static_assert(sizeof(GPUDrawIndirectCommand) == sizeof(VkDrawIndirectCommand), "DrawIndirectCommand mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, vertexCount) == offsetof(VkDrawIndirectCommand, vertexCount), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, instanceCount) == offsetof(VkDrawIndirectCommand, instanceCount), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, firstVertex) == offsetof(VkDrawIndirectCommand, firstVertex), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndirectCommand, firstInstance) == offsetof(VkDrawIndirectCommand, firstInstance), "Layout mismatch");

    static_assert(sizeof(GPUDrawIndexedIndirectCommand) == sizeof(VkDrawIndexedIndirectCommand), "DrawIndexedIndirectCommand mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, indexCount) == offsetof(VkDrawIndexedIndirectCommand, indexCount), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, instanceCount) == offsetof(VkDrawIndexedIndirectCommand, instanceCount), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, firstIndex) == offsetof(VkDrawIndexedIndirectCommand, firstIndex), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, baseVertex) == offsetof(VkDrawIndexedIndirectCommand, vertexOffset), "Layout mismatch");
    static_assert(offsetof(GPUDrawIndexedIndirectCommand, firstInstance) == offsetof(VkDrawIndexedIndirectCommand, firstInstance), "Layout mismatch");

    inline const char* VkResultToString(VkResult result)
    {
        switch (result)
        {
#define STR(r)   \
	case VK_##r: \
		return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
#undef STR
            default:
                return "UNKNOWN_ERROR";
        }
    }



    bool ValidateLayers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available)
    {
        for (auto layer : required)
        {
            bool found = false;
            for (auto& available_layer : available)
            {
                if (strcmp(available_layer.layerName, layer) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                AlimerLogWarn("Validation Layer '%s' not found", layer);
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> GetOptimalValidationLayers(const std::vector<VkLayerProperties>& supported_instance_layers)
    {
        std::vector<std::vector<const char*>> validation_layer_priority_list =
        {
            // The preferred validation layer is "VK_LAYER_KHRONOS_validation"
            {"VK_LAYER_KHRONOS_validation"},

            // Otherwise we fallback to using the LunarG meta layer
            {"VK_LAYER_LUNARG_standard_validation"},

            // Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
            {
                "VK_LAYER_GOOGLE_threading",
                "VK_LAYER_LUNARG_parameter_validation",
                "VK_LAYER_LUNARG_object_tracker",
                "VK_LAYER_LUNARG_core_validation",
                "VK_LAYER_GOOGLE_unique_objects",
            },

            // Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
            {"VK_LAYER_LUNARG_core_validation"}
        };

        for (auto& validation_layers : validation_layer_priority_list)
        {
            if (ValidateLayers(validation_layers, supported_instance_layers))
            {
                return validation_layers;
            }

            AlimerLogWarn("Couldn't enable validation layers (see log for error) - falling back");
        }

        // Else return nothing
        return {};
    }
}

#define VK_CHECK(x) \
	do \
	{ \
		VkResult err = x; \
		if (err) \
		{ \
			AlimerLogError("Detected Vulkan error: %s", VkResultToString(err)); \
            ALIMER_UNREACHABLE(); \
		} \
	} while (0)
#define VK_LOG_ERROR(result, message) AlimerLogError("Vulkan: %s, error: %s", message, VkResultToString(result));

static struct {
    GPUConfig config;
#ifdef _WIN32
    HMODULE library;
#else
    void* library;
#endif

    uint32_t apiVersion;
    bool debugUtils;
    bool portability;
    bool headless;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
} state;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*/* pUserData*/)
{
    //ALIMER_UNUSED(pUserData);

    const char* messageTypeStr = "General";

    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        messageTypeStr = "Validation";
    else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        messageTypeStr = "Performance";

    // Log debug messge
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        AlimerLogWarn("Vulkan - %s: %s", messageTypeStr, pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        AlimerLogError("Vulkan - %s: %s", messageTypeStr, pCallbackData->pMessage);
    }

    return VK_FALSE;
}


static bool Vulkan_IsSupported(void)
{
    static bool available_initialized = false;
    static bool available = false;

    if (available_initialized) {
        return available;
    }

    available_initialized = true;

    // Load
#ifdef _WIN32
    state.library = LoadLibraryA("vulkan-1.dll");
    if (!state.library)
    {
        //Alimer_LogError("Failed to load vulkan library");
        return false;
    }
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(state.library, "vkGetInstanceProcAddr");
#elif __APPLE__
    state.library = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!state.library)
        state.library = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!state.library)
        state.library = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!state.library)
    {
        //Alimer_LogError("Failed to load vulkan library");
        return false;
    }
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(state.library, "vkGetInstanceProcAddr");
#else
    state.library = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (!state.library)
        state.library = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!state.library)
    {
        //Alimer_LogError("Failed to load vulkan library");
        return false;
    }
#endif
    GPU_FOREACH_ANONYMOUS(GPU_LOAD_ANONYMOUS);

    if (vkEnumerateInstanceVersion(&state.apiVersion) != VK_SUCCESS)
    {
        return false;
    }

    if (state.apiVersion < VK_API_VERSION_1_1)
    {
        return false;
    }

    available = true;
    return true;
}

static bool Vulkan_Init(const GPUConfig* config)
{
    state.config = *config;

    // Enumerate available layers and extensions
    {
        uint32_t instanceLayerCount;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
        std::vector<VkLayerProperties> availableInstanceLayers(instanceLayerCount);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, availableInstanceLayers.data()));

        uint32_t extensionCount = 0;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
        std::vector<VkExtensionProperties> availableInstanceExtensions(extensionCount);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableInstanceExtensions.data()));

        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;

        for (auto& availableExtension : availableInstanceExtensions)
        {
            if (strcmp(availableExtension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
            {
                state.debugUtils = true;
                instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            else if (strcmp(availableExtension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
            {
                state.portability = true;
                instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            }
            else if (strcmp(availableExtension.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0)
            {
                instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            }
            else if (strcmp(availableExtension.extensionName, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME) == 0)
            {
                instanceExtensions.push_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
            }
            else if (strcmp(availableExtension.extensionName, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME) == 0)
            {
                instanceExtensions.push_back(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME);
            }
        }

        instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

        // Enable surface extensions depending on os
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
        instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
        instanceExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
        instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
        instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_HEADLESS_EXT)
        instanceExtensions.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
#else
#       pragma error Platform not supported
#endif

        if (config->validationMode != GPUValidationMode_Disabled)
        {
            // Determine the optimal validation layers to enable that are necessary for useful debugging
            std::vector<const char*> optimalValidationLyers = GetOptimalValidationLayers(availableInstanceLayers);
            instanceLayers.insert(instanceLayers.end(), optimalValidationLyers.begin(), optimalValidationLyers.end());
        }


        bool validationFeatures = false;
        if (config->validationMode == GPUValidationMode_GPU)
        {
            uint32_t layerInstanceExtensionCount;
            VK_CHECK(vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layerInstanceExtensionCount, nullptr));
            std::vector<VkExtensionProperties> availableLayerInstanceExtensions(layerInstanceExtensionCount);
            VK_CHECK(vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layerInstanceExtensionCount, availableLayerInstanceExtensions.data()));

            for (auto& availableExtension : availableLayerInstanceExtensions)
            {
                if (strcmp(availableExtension.extensionName, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) == 0)
                {
                    validationFeatures = true;
                    instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
                }
            }
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = config->applicationName;
        appInfo.pEngineName = config->engineName;
        appInfo.engineVersion = VK_MAKE_VERSION(ALIMER_GPU_VERSION_MAJOR, ALIMER_GPU_VERSION_MINOR, ALIMER_GPU_VERSION_PATCH);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
        instanceInfo.ppEnabledLayerNames = instanceLayers.data();
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};

        if (config->validationMode != GPUValidationMode_Disabled && state.debugUtils)
        {
            debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugUtilsCreateInfo.pNext = nullptr;
            debugUtilsCreateInfo.flags = 0;

            debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

            if (config->validationMode == GPUValidationMode_Verbose)
            {
                debugUtilsCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
                debugUtilsCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
            }

            debugUtilsCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;
            instanceInfo.pNext = &debugUtilsCreateInfo;
        }

        VkValidationFeaturesEXT validationFeaturesInfo = {};

        if (config->validationMode == GPUValidationMode_GPU && validationFeatures)
        {
            static const VkValidationFeatureEnableEXT enable_features[2] = {
                VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
                VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
            };

            validationFeaturesInfo.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            validationFeaturesInfo.enabledValidationFeatureCount = 2;
            validationFeaturesInfo.pEnabledValidationFeatures = enable_features;
            validationFeaturesInfo.pNext = instanceInfo.pNext;
            instanceInfo.pNext = &validationFeaturesInfo;
        }

        if (state.portability)
        {
            instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &state.instance);
        if (result != VK_SUCCESS)
        {
            VK_LOG_ERROR(result, "Instance creation failed");
            return false;
        }

        GPU_FOREACH_INSTANCE(GPU_LOAD_INSTANCE);
        GPU_FOREACH_INSTANCE_PLATFORM(GPU_LOAD_INSTANCE);

        if (config->validationMode != GPUValidationMode_Disabled && state.debugUtils)
        {
            result = vkCreateDebugUtilsMessengerEXT(state.instance, &debugUtilsCreateInfo, nullptr, &state.debugUtilsMessenger);
            if (result != VK_SUCCESS)
            {
                VK_LOG_ERROR(result, "Could not create debug utils messenger");
            }
        }

#ifdef _DEBUG
        AlimerLogInfo("Created VkInstance with version: %d.%d.%d",
            VK_VERSION_MAJOR(appInfo.apiVersion),
            VK_VERSION_MINOR(appInfo.apiVersion),
            VK_VERSION_PATCH(appInfo.apiVersion)
        );

        if (instanceInfo.enabledLayerCount)
        {
            AlimerLogInfo("Enabled %d Validation Layers:", instanceInfo.enabledLayerCount);

            for (uint32_t i = 0; i < instanceInfo.enabledLayerCount; ++i)
            {
                AlimerLogInfo("	\t%s", instanceInfo.ppEnabledLayerNames[i]);
            }
        }

        AlimerLogInfo("Enabled %d Instance Extensions:", instanceInfo.enabledExtensionCount);
        for (uint32_t i = 0; i < instanceInfo.enabledExtensionCount; ++i)
        {
            AlimerLogInfo("	\t%s", instanceInfo.ppEnabledExtensionNames[i]);
        }
#endif
    }

    return true;
}

static void Vulkan_Shutdown()
{
    if (state.debugUtilsMessenger != VK_NULL_HANDLE)
        vkDestroyDebugUtilsMessengerEXT(state.instance, state.debugUtilsMessenger, nullptr);

    if (state.instance != VK_NULL_HANDLE)
        vkDestroyInstance(state.instance, nullptr);

    memset(&state, 0, sizeof(state));
}


static bool Vulkan_InitSurface(GPUSurface surface)
{
    // Create surface first
    VkResult result = VK_SUCCESS;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.window = surface->windowHandle;

    result = vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = (HINSTANCE)surface->win32.hinstance;
    surfaceCreateInfo.hwnd = (HWND)surface->win32.hwnd;

    result = vkCreateWin32SurfaceKHR(state.instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    VkMetalSurfaceCreateInfoEXT surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    surfaceCreateInfo.pLayer = (const CAMetalLayer*)surface->apple.metalLayer;

    result = vkCreateMetalSurfaceEXT(state.instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    //surfaceCreateInfo.connection = xlibXcb.GetXCBConnection(static_cast<Display*>(surface->GetXDisplay()));
    surfaceCreateInfo.window = (xcb_window_t)surface->windowHandle;

    result = vkCreateXcbSurfaceKHR(state.instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    //surfaceCreateInfo.dpy = static_cast<Display*>(surface->GetXDisplay());
    surfaceCreateInfo.window = surface->windowHandle;

    result = vkCreateXlibSurfaceKHR(state.instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    //surfaceCreateInfo.display = display;
    surfaceCreateInfo.surface = surface->windowHandle;

    result = vkCreateWaylandSurfaceKHR(state.instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
#elif defined(VK_USE_PLATFORM_HEADLESS_EXT)
    VkHeadlessSurfaceCreateInfoEXT surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT;

    result = vkCreateHeadlessSurfaceEXT(state.instance, &surfaceCreateInfo, nullptr, &surface->vk.handle);
#endif

    if (result != VK_SUCCESS)
    {
        VK_LOG_ERROR(result, "Failed to create surface");
        return false;
    }

    return true;
}

static GPUDevice Vulkan_CreateDevice(/*const VGPUDeviceDescriptor* info*/)
{
    return nullptr;
}

GPUDriver Vulkan_Driver = {
    GPUBackendType_Vulkan,
    Vulkan_IsSupported,
    Vulkan_Init,
    Vulkan_Shutdown,
    Vulkan_InitSurface,
    Vulkan_CreateDevice
};

#endif /* ALIMER_VULKAN */
