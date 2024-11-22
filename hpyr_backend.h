#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <assert.h>
#include <stdbool.h>

#warning this stuff is hardcoded for windows pc
#define HPYR_BACKEND_IDX_GRAPHICS_QUEUE 0
#define HPYR_BACKEND_IDX_COMPUTE_QUEUE 1
#define HPYR_BACKEND_IDX_TRANSFER_QUEUE 2
#define HPYR_BACKEND_SWAPCHAIN_IMAGE_FORMAT VK_FORMAT_R8G8B8A8_UINT

#define VK_TRY(try)                                                                     \
        do                                                                              \
        {                                                                               \
                VkResult res_IOWHFOSJNFOISDNFOSDNFLNSDFLNSDLFNSDLFNSDLFN =              \
                        try;                                                            \
                if (res_IOWHFOSJNFOISDNFOSDNFLNSDFLNSDLFNSDLFNSDLFN !=                  \
                    VK_SUCCESS)                                                         \
                {                                                                       \
                        fprintf(stderr,                                                 \
                                #try " failed with %s.\n",                              \
                                string_VkResult(                                        \
                                        res_IOWHFOSJNFOISDNFOSDNFLNSDFLNSDLFNSDLFNSDLFN \
                                ));                                                     \
                }                                                                       \
        } while (0);

typedef struct
{
        VkInstance instance;
        VkPhysicalDevice pdevice;
        VkDevice ldevice;
        VkCommandPool cmd_pool;

        uint32_t pidx_qfam[3];
        VkQueue pqueues[3];
        VkCommandPool ppools[3];

        VkSwapchainKHR swapchain;
        uint32_t nswapchain_imgs;
        VkImage *pswapchain_imgs;
        VkSurfaceKHR surface;

        int w, h; // window width, height
        SDL_Window *pwin;
} hpyr_backend;


// TODO: make this
// void hpyr_backend_create_command_pool();

void hpyr_backend_init(hpyr_backend *pb, char *pname, int w, int h)
{
        assert(pb && pname);

        pb->w = w;
        pb->h = h;

        // SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
                fprintf(stderr, "Cant init SDL.\n");
                abort();
        }

        pb->pwin = SDL_CreateWindow(
                pname,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                w,
                h,
                SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
        );
        if (!pb->pwin)
        {
                fprintf(stderr, "Cant init SDL window.\n");
                abort();
        }

        // Instance
        VkInstanceCreateInfo instance_info = {
                .sType             = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames =
                        (char *[]){"VK_LAYER_KHRONOS_validation"},
                .enabledExtensionCount = 2,
                .ppEnabledExtensionNames =
                        (char *[]){"VK_KHR_surface", "VK_KHR_win32_surface"}
        };
        VK_TRY(vkCreateInstance(&instance_info, NULL, &pb->instance));

        // pdevice
        uint32_t npdevices = 0;
        VK_TRY(vkEnumeratePhysicalDevices(pb->instance, &npdevices, NULL));
        VkPhysicalDevice *ppdevices =
                malloc(sizeof(VkPhysicalDevice) * npdevices);
        VK_TRY(vkEnumeratePhysicalDevices(pb->instance, &npdevices, ppdevices));
        pb->pdevice = ppdevices[0];

        // ldevice
        VkPhysicalDeviceTimelineSemaphoreFeatures timeline_feat = {
                .sType =
                        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
                .timelineSemaphore = VK_TRUE
        };

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dyn_rendering_feat = {
                .sType =
                        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .pNext            = &timeline_feat,
                .dynamicRendering = VK_TRUE
        };

        VkDeviceCreateInfo device_info = {
                .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext                   = &dyn_rendering_feat,
                .queueCreateInfoCount    = 1,
                .pQueueCreateInfos       = &(VkDeviceQueueCreateInfo
                ){.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .queueFamilyIndex = 0,
                        .queueCount       = 1,
                        .pQueuePriorities = (float[]){1.0f}},
                .enabledExtensionCount   = 5,
                .ppEnabledExtensionNames = (char *[]
                ){"VK_KHR_swapchain",
                  "VK_KHR_dynamic_rendering",
                  "VK_KHR_depth_stencil_resolve",
                  "VK_KHR_create_renderpass2",
                  "VK_KHR_timeline_semaphore"}
        };
        VK_TRY(vkCreateDevice(pb->pdevice, &device_info, NULL, &pb->ldevice));

        // queues
        // TODO: fix this to actually get the queues for windows pc
        vkGetDeviceQueue(
                pb->ldevice, 0, 0, &pb->pqueues[HPYR_BACKEND_IDX_GRAPHICS_QUEUE]
        );
        vkGetDeviceQueue(
                pb->ldevice, 0, 0, &pb->pqueues[HPYR_BACKEND_IDX_COMPUTE_QUEUE]
        );
        vkGetDeviceQueue(
                pb->ldevice, 0, 0, &pb->pqueues[HPYR_BACKEND_IDX_TRANSFER_QUEUE]
        );

        // swapchain
        SDL_Vulkan_CreateSurface(pb->pwin, pb->instance, &pb->surface);

        // uint32_t nsurface_formats;
        // vkGetPhysicalDeviceSurfaceFormatsKHR(
        //         pb->pdevice, pb->surface, &nsurface_formats, NULL);
        // VkSurfaceFormatKHR *psurface_formats =
        //         malloc(sizeof(VkSurfaceFormatKHR) * nsurface_formats);
        // vkGetPhysicalDeviceSurfaceFormatsKHR(
        //         pb->pdevice, pb->surface, &nsurface_formats,
        //         psurface_formats);

        VK_TRY(vkCreateSwapchainKHR(
                pb->ldevice,
                &(VkSwapchainCreateInfoKHR
                ){.sType         = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                  .surface       = pb->surface,
                  .minImageCount = 2,
                  .imageFormat   = HPYR_BACKEND_SWAPCHAIN_IMAGE_FORMAT,
                  .imageColorSpace       = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
                  .imageExtent           = (VkExtent2D){w, h},
                  .imageArrayLayers      = 1,
                  .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                  .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
                  .queueFamilyIndexCount = 1,
                  .pQueueFamilyIndices   = (uint32_t[]){0},
                  .preTransform   = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                  .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                  .presentMode    = VK_PRESENT_MODE_MAILBOX_KHR,
                  .clipped        = VK_TRUE},
                NULL,
                &pb->swapchain
        ));

        // get images
        VK_TRY(vkGetSwapchainImagesKHR(
                pb->ldevice, pb->swapchain, &pb->nswapchain_imgs, NULL
        ));
        pb->pswapchain_imgs = malloc(sizeof(VkImage) * pb->nswapchain_imgs);
        VK_TRY(vkGetSwapchainImagesKHR(
                pb->ldevice,
                pb->swapchain,
                &pb->nswapchain_imgs,
                pb->pswapchain_imgs
        ));

        // TODO: verify if this works
        // Command Pool
        // it looks dumb but works because queues dont need to be destroyed
        for (uint32_t i = 0; i < 3; i++)
        {
                uint32_t idx = i;

                for (uint32_t j = i + 1; j < 3; j++)
                {
                        // one of the queues is a multi purpose queue
                        // ex: graphics and compute
                        if (pb->pidx_qfam[i] == pb->pidx_qfam[j])
                                idx = j;
                }

                VkCommandPoolCreateInfo pool_info = {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        .flags =
                                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                        .queueFamilyIndex = pb->pidx_qfam[idx]
                };

                VkCommandPool pool = VK_NULL_HANDLE;
                VK_TRY(vkCreateCommandPool(pb->ldevice, &pool_info, NULL, &pool));
        }
}

VkShaderModule hpyr_backend_create_shader_module(
        hpyr_backend *pb, uint32_t *pcode, uint32_t sz
)
{
        assert(pb && pcode);

        VkShaderModule module = VK_NULL_HANDLE;
        VK_TRY(vkCreateShaderModule(
                pb->ldevice,
                &(VkShaderModuleCreateInfo
                ){.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                  .codeSize = sz,
                  .pCode    = pcode},
                NULL,
                &module
        ));

        return module;
}

void hpyr_backend_create_semaphore(
        hpyr_backend *pb, VkSemaphore *psema, uint64_t val, bool is_bin
)
{
        assert(pb && psema);

        VkSemaphoreCreateInfo info = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        if (is_bin)
        {
                VkSemaphoreTypeCreateInfo type_info = {
                        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                        .initialValue  = val
                };

                info.pNext = &type_info;
        }

        VK_TRY(vkCreateSemaphore(pb->ldevice, &info, NULL, psema));
}

void hpyr_backend_create_command_buffers(
        hpyr_backend *pb,
        VkCommandBuffer *pcmd,
        uint32_t ncmd,
        VkCommandBufferLevel lvl,
        uint32_t idx_queue
)
{
        assert(pb && pcmd);

        VkCommandBufferAllocateInfo info = {
                .sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = pb->ppools[idx_queue],
                .level       = lvl,
                .commandBufferCount = ncmd
        };
        VK_TRY(vkAllocateCommandBuffers(pb->ldevice, &info, pcmd));
}

void hpyr_backend_cmd_simple_transition(
        hpyr_backend *pb,
        VkCommandBuffer cmd_buf,
        VkImage img,
        VkPipelineStageFlags src_stage,
        VkPipelineStageFlags dst_stage,
        VkAccessFlagBits src_access,
        VkAccessFlagBits dst_access,
        VkImageLayout old,
        VkImageLayout new
)
{
        assert(pb && VK_NULL_HANDLE != cmd_buf && VK_NULL_HANDLE != img);

        // TODO: maybe you need to add nmiplevels??
        VkImageSubresourceRange range = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
        };

        VkImageMemoryBarrier bar = {
                .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask       = src_access,
                .dstAccessMask       = dst_access,
                .oldLayout           = old,
                .newLayout           = new,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image               = img,
                .subresourceRange    = range
        };

        vkCmdPipelineBarrier(
                cmd_buf,
                src_stage,
                dst_stage,
                VK_DEPENDENCY_BY_REGION_BIT,
                0,
                NULL,
                0,
                NULL,
                1,
                &bar
        );
}

void hpyr_backend_cmd_image_barrier(
        hpyr_backend *pb,
        VkCommandBuffer cmd_buf,
        VkImage img,
        VkPipelineStageFlags src_stage,
        VkPipelineStageFlags dst_stage,
        VkDependencyFlags deps,
        VkAccessFlagBits src_access,
        VkAccessFlagBits dst_access,
        VkImageLayout old,
        VkImageLayout new,
        uint32_t src_qfam,
        uint32_t dst_qfam,
        VkImageSubresourceRange range
)
{
        assert(pb && VK_NULL_HANDLE != cmd_buf && VK_NULL_HANDLE != img);

        VkImageMemoryBarrier bar = {
                .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask       = src_access,
                .dstAccessMask       = dst_access,
                .oldLayout           = old,
                .newLayout           = new,
                .srcQueueFamilyIndex = src_qfam,
                .dstQueueFamilyIndex = dst_qfam,
                .image               = img,
                .subresourceRange    = range
        };

        vkCmdPipelineBarrier(
                cmd_buf, src_stage, dst_stage, deps, 0, NULL, 0, NULL, 1, &bar
        );
}

void hpyr_backend_cmd_buffer_barrier(
        hpyr_backend *pb,
        VkCommandBuffer cmd_buf,
        VkBuffer buf,
        VkPipelineStageFlags src_stage,
        VkPipelineStageFlags dst_stage,
        VkDependencyFlags deps,
        VkAccessFlagBits src_access,
        VkAccessFlagBits dst_access,
        uint32_t src_qfam,
        uint32_t dst_qfam,
        VkDeviceSize offset,
        VkDeviceSize sz
)
{

        VkBufferMemoryBarrier bar = {
                .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .srcAccessMask       = src_access,
                .dstAccessMask       = dst_access,
                .srcQueueFamilyIndex = src_qfam,
                .dstQueueFamilyIndex = dst_qfam,
                .buffer              = buf,
                .offset              = offset,
                .size                = sz
        };

        vkCmdPipelineBarrier(
                cmd_buf, src_stage, dst_stage, deps, 0, NULL, 1, &bar, 0, NULL
        );
}

uint32_t
hpyr_backend_find_idx_memory_type(hpyr_backend *pb, VkMemoryRequirements *preq)
{
}

VkImage hpyr_backend_next_image(
        hpyr_backend *pb, uint64_t time, VkSemaphore sema, VkFence fence
)
{
        uint32_t idx = 0;
        VK_TRY(vkAcquireNextImageKHR(
                pb->ldevice, pb->swapchain, time, sema, fence, &idx
        ));
        return pb->pswapchain_imgs[idx];
}

void hpyr_backend_simple_submit(
        hpyr_backend *pb,
        VkCommandBuffer cmd_buf,
        uint32_t idx_queue,
        uint32_t nwait_semas,
        VkSemaphore *pwait_semas,
        VkPipelineStageFlags *pstages,
        uint32_t nsig_semas,
        VkSemaphore *psig_semas,
        VkFence fence
)
{
        VkSubmitInfo info = {
                .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount   = nwait_semas,
                .pWaitSemaphores      = pwait_semas,
                .pWaitDstStageMask    = pstages,
                .commandBufferCount   = 1,
                .pCommandBuffers      = &cmd_buf,
                .signalSemaphoreCount = nsig_semas,
                .pSignalSemaphores    = psig_semas
        };

        VK_TRY(vkQueueSubmit(pb->pqueues[idx_queue], 1, &info, fence));
}

void hpyr_backend_simple_present(
        hpyr_backend *pb,
        uint32_t idx_queue,
        uint32_t nwait_sema,
        VkSemaphore *pwait_sema,
        VkImage img
)
{
        // There are only like 3 images usually so this is fine
        int idx_img = -1;
        for (uint32_t i = 0; i < pb->nswapchain_imgs; i++)
        {
                if (pb->pswapchain_imgs[i] == img)
                {
                        idx_img = i;
                        break;
                }
        }

        // its probably better to just crash
        // if (idx < 0)
        //         return;

        VkPresentInfoKHR info = {
                .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = nwait_sema,
                .pWaitSemaphores    = pwait_sema,
                .swapchainCount     = 1,
                .pSwapchains        = &pb->swapchain,
                .pImageIndices      = &idx_img
        };

        VK_TRY(vkQueuePresentKHR(pb->pqueues[idx_queue], &info));
}

void hpyr_backend_create_compute_pipe(
        hpyr_backend *pb,
        VkPipeline *ppipe,
        VkPipelineLayout layout,
        VkShaderModule mod
)
{
        VkPipelineShaderStageCreateInfo shader_info = {
                .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage  = VK_SHADER_STAGE_COMPUTE_BIT,
                .module = mod,
                .pName  = "main"
        };

        VkComputePipelineCreateInfo pipe_info = {
                .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                .stage  = shader_info,
                .layout = layout
        };

        VK_TRY(vkCreateComputePipelines(
                pb->ldevice, VK_NULL_HANDLE, 1, &pipe_info, NULL, ppipe
        ));
}

void hpyr_backend_create_descriptor_set_layout(hpyr_backend *pb, VkDescriptorSetLayout *playout, VkDescriptorSetLayoutCreateInfo *pinfo)
{
        VK_TRY(vkCreateDescriptorSetLayout(
                pb->ldevice, pinfo, NULL, playout
        ));
}

void hpyr_backend_create_pipeline_layout(hpyr_backend *pb, VkPipelineLayout *playout, VkPipelineLayoutCreateInfo *pinfo)
{
           VK_TRY(vkCreatePipelineLayout(
                pb->ldevice, pinfo, NULL, playout
        ));
}

void hpyr_backend_cmd_clear_whole_image(hpyr_backend *pb, VkCommandBuffer cmd_buf, VkImage img, VkImageLayout layout, VkClearColorValue *pcolor)
{

        VkImageSubresourceRange range = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = VK_REMAINING_MIP_LEVELS,
                .layerCount = VK_REMAINING_ARRAY_LAYERS
        };

        vkCmdClearColorImage(
                cmd_buf,
                img,
                layout,
                pcolor,
                1,
                &range
        );
}       