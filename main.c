#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hpyr_backend.h"

#define NFRAMES_IN_FLIGHT 2
#define NPIPES
#define SZPUSH_CONSTANTS 256 // TODO: dont just define it as the max size

typedef struct
{
        VkFence fence;
        VkSemaphore cmd_sema, img_sema;
        VkCommandBuffer cmd_buf;
} frame_info;

typedef struct
{
        // makes pr == &pr->backend, dont move
        hpyr_backend backend;

        uint64_t nframe, niters;

        // pipes
        VkPipelineLayout pipe_layout;
        VkPipeline raygen_pipe, tlbvh_pipe,
                blbvh_pipe; // TODO: remove this and replace with array
        VkPipeline ppipes[NPIPES];

        // descriptors
        VkDescriptorSetLayout set_layout;
        VkDescriptorSet scene_desc;

        uint32_t npaths, cap_paths, szpaths;
        char *ppaths;

        // scene buffer
        VkDeviceMemory scene_mem;
        VkBuffer scene_buf;
        uint32_t idx_rays, idx_geometry, idx_draw, idx_ndraw, idx_object,
                idx_light, idx_dispatch_cmd;

        VkCommandPool cmd_pool;
        VkCommandBuffer cmd_buf;

        frame_info pframe_infos[NFRAMES_IN_FLIGHT];


        uint32_t w, h;

        float dt;
        // TODO: maybe remove this and just generate on the fly
        float proj_mat[16], view_mat[16];
} wpt_context;

void wpt_prepare(wpt_context *pr)
{

        /*
                allocate the scene buffer:
                        - geometry, light, drawcmds, objects

                map scene buffer

                assemble bvhs in place in the mapped scene buffer


        // sizeof scene buffer
        uint32_t szlights   = pr->nlights * sizeof(light_t);
        uint32_t szobects   = pr->nobjects * sizeof(object_t);
        uint32_t szdraw     = pr->nobjects *
        sizeof(VkDrawIndexedIndirectCommand); uint32_t szgeometry = 0;

        for (uint32_t i = 0; i < pr->npaths; i++)
        {
        }

        uint32_t szscene_buf = szlights + szobjects + szdraw + szgeometry;

        char *ppaths       = pr->ppaths;
        uint32_t sztemp    = 100000;
        mesh_header *ptemp = malloc(100000);
        for (uint32_t i = 0; i < pr->nmeshes; i++)
        {
                FILE *pfile = fopen(pr->ppaths, "rb");
                fseek(pfile, 0, SEEK_END);
                uint64_t sz = ftell(pfile);
                rewind(pfile);
                if (sztemp < sz)
                        ptemp = realloc(ptemp, sz);
                fwrite(ptemp, sz, 1, pfile);

                while (*ppaths++ != '\0')
                        ;

                bvh_t bvh = {};
                blbvh_init(&bvh, 2 * nindices - 1, 8, );
        }
        free(ptemp);

        // build tlbvh
        for (uint32_t i = 0; i < pr->nmeshes; i++)
        {
        }

        // scene buffer handle
        VkBufferCreateInfo buf_info = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size  =,
                .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                         VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
                .sharingMode           = VK_SHARING_MODE_CONCURRENT,
                .queueFamilyIndexCount = 2,
                .pQueueFamilyIndices =
                        (uint32_t[]) {pr->idx_compute_queue,
        pr->idx_transfer_queue}}; VK_TRY(vkCreateBuffer(pr->ldevice, &buf_info,
        NULL, &pr->scene_buf));

        // alloc scene buffer
        VkMemoryAllocateInfo alloc_info = {
                .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize  =,
                .memoryTypeIndex = };
        VK_TRY(vkAllocateMemory(pr->ldevice, &alloc_info, NULL,
        &pr->scene_mem)); VK_TRY(vkBindBufferMemory(pr->ldevice, pr->scene_buf,
        pr->scene_mem, 0));

        // map scene buf
        void *pmapped = NULL;
        VK_TRY(vkMapMemory(pr->ldevice, pr->scene_mem, 0, szscene_buf, 0));
        */
}

void wpt_init_common(wpt_context *pr)
{
        // descriptor set layout
        VkDescriptorSetLayoutBinding binding = {
                .binding         = 0,
                .descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_ALL
        };

        VkDescriptorSetLayoutCreateInfo set_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = 1,
                .pBindings    = &binding
        };

        hpyr_backend_create_descriptor_set_layout(
                (hpyr_backend *) pr, &pr->set_layout, &set_info
        );

        // pipeline layout
        VkPipelineLayoutCreateInfo pipe_info = {
                .sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount = 1,
                .pSetLayouts    = &pr->set_layout,
                .pushConstantRangeCount = 1,
                .pPushConstantRanges    = &(VkPushConstantRange
                ){.stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                   VK_SHADER_STAGE_FRAGMENT_BIT |
                                   VK_SHADER_STAGE_COMPUTE_BIT,
                     .offset = 0,
                     .size   = SZPUSH_CONSTANTS}
        };

        hpyr_backend_create_pipeline_layout((hpyr_backend *) pr, &pr->pipe_layout, &pipe_info);
}

void wpt_init_compute_pipes(wpt_context *pr)
{
        // TODO: populate this
        char *ppaths[] = {
                "",

        };

        for (uint32_t i = 0; i < sizeof ppaths; i++)
        {
                FILE *pfile = fopen(ppaths[i], "rb");
                if (!pfile)
                {
                        fprintf(stderr, "Cant open: %s\n", ppaths[i]);
                        abort();
                }

                uint64_t sz = fseek(pfile, 0, SEEK_END);
                rewind(pfile);

                uint32_t *pcode = malloc(sz); // whatever
                fwrite(pcode, sz, 1, pfile);

                VkShaderModule mod =
                        hpyr_backend_create_shader_module((hpyr_backend *) pr, pcode, sz);

                hpyr_backend_create_compute_pipe(
                        pr, &pr->ppipes[i], pr->pipe_layout, mod
                );

                free(pcode);
        }
}

void wpt_init_frame_infos(wpt_context *pr)
{
        for (uint32_t i = 0; i < NFRAMES_IN_FLIGHT; i++)
        {
                frame_info *pframe_info = &pr->pframe_infos[i];

                hpyr_backend_create_semaphore(pr, &pframe_info->img_sema, 0, 0);
                hpyr_backend_create_semaphore(pr, &pframe_info->cmd_sema, 0, 0);

                hpyr_backend_create_command_buffers(
                        (hpyr_backend *) pr,
                        &pframe_info->cmd_buf,
                        1,
                        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                        HPYR_BACKEND_IDX_COMPUTE_QUEUE
                );
        }
}

void wpt_init(
        wpt_context *pr, char *pname, int width, int height, uint32_t niters)
{
        pr->nframe = 1; // TODO: fix this crap and the starting frames stuff 
        pr->niters = niters;

        hpyr_backend_init(pr, pname, width, height);

        pr->w = pr->backend.w;
        pr->h = pr->backend.h;

        wpt_init_common(pr);
        wpt_init_graphics_pipes(pr);
        // wpt_init_compute_pipes(pr);
        wpt_init_frame_infos(pr);
}

void wpt_draw(wpt_context *pr)
{
        frame_info *pframe_info = &pr->pframe_infos[pr->nframe % NFRAMES_IN_FLIGHT];
        VkCommandBuffer cmd_buf = pframe_info->cmd_buf;
        VkSemaphore img_sema    = pframe_info->img_sema;
        VkSemaphore cmd_sema    = pframe_info->cmd_sema;
        VkFence fence           = pframe_info->fence;

        hpyr_backend *pb = &pr->backend;

        // make sure to not loop around if CPU gets ahead
        VK_TRY(vkWaitForFences(pb->ldevice, 1, &fence, VK_TRUE, 1));
        VK_TRY(vkResetFences(pb->ldevice, 1, &fence));

        VkCommandBufferBeginInfo begin = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_TRY(vkBeginCommandBuffer(cmd_buf, &begin));

        // get image, 1ms timeout
        VkImage img =
                hpyr_backend_next_image(pr, 1000000, img_sema, VK_NULL_HANDLE);

        // set dynamic state
        VkViewport vport = {
                .x        = 0,
                .y        = 0,
                .width    = pr->w,
                .height   = pr->h,
                .minDepth = 0.0f,
                .maxDepth = 1.0f
        };
        vkCmdSetViewport(cmd_buf, 0, 1, &vport);

        vkCmdSetScissor(
                cmd_buf, 0, 1, &(VkRect2D){.extent = {pr->w, pr->h}}
        );

        // transition img to general
        hpyr_backend_cmd_simple_transition(
                (hpyr_backend *) pr,
                cmd_buf,
                img,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_ACCESS_NONE_KHR,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_GENERAL
        );

        // clear to black
        hpyr_backend_cmd_clear_whole_image(pr, cmd_buf, img, VK_IMAGE_LAYOUT_GENERAL, &(VkClearColorValue){});

        // primary ray gen
        vkCmdBindPipeline(
                cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pr->raygen_pipe
        );
        vkCmdDispatch(cmd_buf, ceil(pr->w / 16), ceil(pr->h / 16), 1);

        // update the buf with the correct nrays for the compact pipe
        vkCmdUpdateBuffer(
                cmd_buf,
                pr->scene_buf,
                pr->idx_dispatch_cmd,
                sizeof(VkDispatchIndirectCommand),
                &(VkDispatchIndirectCommand
                ){ceil(pr->w / 16), ceil(pr->h / 16), 1}
        );

        // top level bvh query
        vkCmdBindPipeline(
                cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pr->tlbvh_pipe
        );

        vkCmdDispatch(cmd_buf, ceil(pr->w / 16), ceil(pr->h / 16), 1);

        // ray buffer barrier
        hpyr_backend_cmd_buffer_barrier(
                pr,
                cmd_buf,
                pr->scene_buf,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_DEPENDENCY_BY_REGION_BIT,
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                pr->ray_offset,
                pr->szrays
        );

        uint32_t niters = pr->niters;
        while (niters--)
        {
                // compact
                vkCmdBindPipeline(
                        cmd_buf,
                        VK_PIPELINE_BIND_POINT_COMPUTE,
                        pr->compact_pipe
                );

                vkCmdDispatchIndirect(
                        cmd_buf, pr->scene_buf, pr->idx_dispatch_cmd
                );

                // ray buffer barrier
                hpyr_backend_cmd_buffer_barrier(
                        pr,
                        cmd_buf,
                        pr->scene_buf,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_DEPENDENCY_BY_REGION_BIT,
                        VK_ACCESS_SHADER_WRITE_BIT,
                        VK_ACCESS_SHADER_READ_BIT,
                        VK_QUEUE_FAMILY_IGNORED,
                        VK_QUEUE_FAMILY_IGNORED,
                        pr->ray_offset,
                        pr->szrays
                );

                // bottom level bvh query
                vkCmdBindPipeline(
                        cmd_buf,
                        VK_PIPELINE_BIND_POINT_COMPUTE,
                        pr->blbvh_pipe
                );

                vkCmdDispatchIndirect(
                        cmd_buf, pr->scene_buf, pr->idx_dispatch_cmd
                );

                // ray buffer barrier
                hpyr_backend_cmd_buffer_barrier(
                        pr,
                        cmd_buf,
                        pr->scene_buf,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_DEPENDENCY_BY_REGION_BIT,
                        VK_ACCESS_SHADER_WRITE_BIT,
                        VK_ACCESS_SHADER_READ_BIT,
                        VK_QUEUE_FAMILY_IGNORED,
                        VK_QUEUE_FAMILY_IGNORED,
                        pr->ray_offset,
                        pr->szrays
                );
        }

        // transition img to present src
        hpyr_backend_cmd_simple_transition(
                (hpyr_backend *) pr,
                cmd_buf,
                img,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                VK_ACCESS_NONE_KHR,
                0,
                VK_IMAGE_LAYOUT_GENERAL,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        );

        VK_TRY(vkEndCommandBuffer(cmd_buf));

        // submit and present
        hpyr_backend_simple_submit(
                (hpyr_backend *) pr,
                cmd_buf, 
                HPYR_BACKEND_IDX_COMPUTE_QUEUE,
                1,
                &img_sema,
                (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT},
                1,
                &cmd_sema,
                VK_NULL_HANDLE
        );

        hpyr_backend_simple_present(
                pb, HPYR_BACKEND_IDX_COMPUTE_QUEUE, 1, &cmd_sema, img
        );

        pr->nframe++;
}

int main()
{
        srand(time(NULL));
        wpt_context wpt = {};
        wpt_init(&wpt, "HELLO BRO", 800, 600, 10);

       
        return 0;
}
