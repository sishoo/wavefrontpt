

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define VOXEL_TYPE_HARD 0b00
#define VOXEL_TYPE_SOFT 0b11
#define VOXEL_TYPE_FLUCUATE1 0b10
#define VOXEL_TYPE_FLUCUATE2 0b01

#define wpt_SZPUSH_CONSTANTS sizeof(float[36])

typedef struct
{
        float x, y, z;
} vec3_t;

typedef struct
{
        float mass;
        vec3_t position, velocity, acceleration;
} point_mass_t;

typedef struct
{
        float k, rest_distance;
} spring_t;

typedef struct
{
        uint32_t npoint_masses, nsprings;
        point_mass_t *ppoint_masses;
        spring_t *psprings;
} entity_t;

#include "include/utils.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NFRAMES_IN_FLIGHT 2
#define wpt_VK_TIMEOUT 9999999

#define wpt_contextIMELINE_COMMANDS_COMPLETE_VALUE 2ULL
#define wpt_contextIMELINE_FRAME_PRESENT_VALUE 3ULL
#define wpt_SWAPCHAIN_IMAGE_FORMAT VK_FORMAT_R8G8B8A8_UNORM

#define wpt_SZWORKGROUP_X 16
#define wpt_SZWORKGROUP_Y 16
#define wpt_SZWORKGROUP_Z 1

#include "/Users/macfarrell/projects/ccpp/harpy/backend/hpyr_backend.h"

typedef struct
{
        VkFence fence;
        VkSemaphore time_sema, img_sema;
        VkCommandBuffer cmd_buf;
} frame_info;

typedef struct
{
        // makes pr == &pr->backend, dont move
        hpyr_backend backend;

        uint64_t nframe;

        VkPipelineLayout pipe_layout;
        VkPipeline raygen_pipe, tlbvh_pipe, blbvh_pipe;

        VkDescriptorSetLayout set_layout;
        VkDescriptorSet scene_desc;

        uint32_t npaths, cap_paths, szpaths;
        char *ppaths;

        uint32_t niters;

        VkDeviceMemory scene_mem;
        VkBuffer scene_buf;
        uint32_t idx_rays, idx_geometry, idx_draw, idx_ndraw, idx_object, idx_light;

        VkCommandPool cmd_pool;
        VkCommandBuffer cmd_buf;

        frame_info_t pframe_infos[NFRAMES_IN_FLIGHT];

        float dt;
        // TODO: maybe remove this and just generate on the fly
        float proj_mat[16], view_mat[16];
} wpt_context;

bool wpt_load_mesh(wpt_context *pr, char *ppath)
{
        if (szpath + pr->szpaths)
                pr->ppaths = realloc(pr->ppaths, pr->cap_paths);

        strncpy(pr->ppaths + sz_paths, ppath, szpath);
}

void wpt_prepare(wpt_context *pr, )
{

        /*
                allocate the scene buffer:
                        - geometry, light, drawcmds, objects

                map scene buffer

                assemble bvhs in place in the mapped scene buffer
        

        // sizeof scene buffer
        uint32_t szlights   = pr->nlights * sizeof(light_t);
        uint32_t szobects   = pr->nobjects * sizeof(object_t);
        uint32_t szdraw     = pr->nobjects * sizeof(VkDrawIndexedIndirectCommand);
        uint32_t szgeometry = 0;

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
                        (uint32_t[]) {pr->idx_compute_queue, pr->idx_transfer_queue}};
        VK_TRY(vkCreateBuffer(pr->ldevice, &buf_info, NULL, &pr->scene_buf));

        // alloc scene buffer
        VkMemoryAllocateInfo alloc_info = {
                .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize  =,
                .memoryTypeIndex = };
        VK_TRY(vkAllocateMemory(pr->ldevice, &alloc_info, NULL, &pr->scene_mem));
        VK_TRY(vkBindBufferMemory(pr->ldevice, pr->scene_buf, pr->scene_mem, 0));

        // map scene buf
        void *pmapped = NULL;
        VK_TRY(vkMapMemory(pr->ldevice, pr->scene_mem, 0, szscene_buf, 0));
        */

}

void wpt_init_common(wpt_context *pr)
{
        VkDescriptorSetLayoutBinding binding = {
                .binding         = 0,
                .descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags      = VK_SHADER_STAGE_ALL};

        VkDescriptorSetLayoutCreateInfo set_layout_info = {
                .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = 1,
                .pBindings    = &binding};

        VK_TRY(vkCreateDescriptorSetLayout(
                pr->ldevice, &set_layout_info, NULL, &pr->set_layout));

        VkPipelineLayoutCreateInfo pipe_layout_info = {
                .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount         = 1,
                .pSetLayouts            = &pr->set_layout,
                .pushConstantRangeCount = 1,
                .pPushConstantRanges    = &(VkPushConstantRange) {
                           .stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                      VK_SHADER_STAGE_FRAGMENT_BIT |
                                      VK_SHADER_STAGE_COMPUTE_BIT,
                           .offset = 0,
                           .size   = wpt_SZPUSH_CONSTANTS}};

        VK_TRY(vkCreatePipelineLayout(
                pr->ldevice, &pipe_layout_info, NULL, &pr->pipe_layout));
}

/*
void wpt_init_compute_pipes(wpt_context *pr)
{
        static uint32_t pphysics_spv[] = {
#include "shader/spv/physics.comp.spv"
        };

        VkShaderModule physics_module =
                init_shader_module(pphysics_spv, sizeof pphysics_spv);

        VkPipelineShaderStageCreateInfo physics_shader_info = {
                .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage  = VK_SHADER_STAGE_COMPUTE_BIT,
                .module = physics_module,
                .pname  = "main"};

        VkPipelineInfo pipe_info = {
                .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                .stage  = physics_shader_info,
                .layout = pr->pipe_layout};

        VK_TRY(vkCreateComputePipelines(
                pr->ldevice,
                VK_NULL_HANDLE,
                2,
                pipeline_infos,
                NULL,
                &pr->physics_pipe));
}
*/

void wpt_init_frame_infos(wpt_context *pr)
{
        VK_TRY(vkCreateCommandPool(
                pr->ldevice,
                &(VkCommandPoolCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                        .queueFamilyIndex = pr->idx_qfam},
                NULL,
                &pr->cmd_pool));

        for (uint32_t i = 0; i < NFRAMES_IN_FLIGHT; i++)
        {
                frame_info_t *pframe_info = &pr->pframe_infos[i];

                hpyr_backend_create_semaphore(pr, &pframe_info->img_sema, 0, 0);
                hpyr_backend_create_semaphore(
                        pr,
                        &pframe_info->time_sema,
                        wpt_contextIMELINE_COMMANDS_COMPLETE_VALUE,
                        1);

                hpyr_backend_create_command_buffers(
                        pr,
                        &pframe_info->cmd_buf,
                        1,
                        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                        HPYR_BACKEND_IDX_COMPUTE_QUEUE);
        }
}

void wpt_init(wpt_context *pr, char *pname, int width, int height, uint32_t niters)
{
        pr->nframe = 1;
        pr->niters = niters;

        hpyr_backend_init(pr, pname, width, height);
        wpt_init_common(pr);
        wpt_init_graphics_pipes(pr);
        // wpt_init_compute_pipes(pr);
        wpt_init_frame_infos(pr);
}

void wpt_draw()
{
        frame_info *pframe_info = pr->pframe_infos[nframe % NFRAMES_IN_FLIGHT];
        VkCommandBuffer cmd_buf = pframe_info->cmd_buf;
        VkImage img = pframe_info->img;

        hpyr_backend *pb = &pr->backend;

        // make sure to not loop around if CPU gets ahead
        VK_TRY(vkWaitForFences(pb->ldevice, 1, &fence, VK_TRUE, 1));
        VK_TRY(vkResetFences(pb->ldevice, 1, &fence));

        VkCommandBufferBeginInfo begin = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_TRY(vkBeginCommandBuffer(cmd_buf, &begin));

        // set dynamic state
        VkViewport vport = {
                .x        = 0,
                .y        = 0,
                .width    = pr->width,
                .height   = pr->height,
                .minDepth = 0.0f,
                .maxDepth = 1.0f};
        vkCmdSetViewport(cmd_buf, 0, 1, &vport);

        vkCmdSetScissor(cmd_buf, 0, 1, &(VkRect2D) {.extent = {pr->width, pr->height}});

        // transition img to general
        hpyr_backend_cmd_simple_transition(
                        pr, 
                        cmd_buf, 
                        img,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_ACCESS_NONE_KHR,
                        VK_ACCESS_TRANSFER_WRITE_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, 
                        VK_IMAGE_LAYOUT_GENERAL);

        // clear to black
        vkCmdClearColorImage(
                cmd_buf,
                img,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                &(VkClearColorValue) {},
                1,
                &whole_img);

        // primary ray gen
        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pr->raygen_pipe);
        vkCmdDispatch(cmd_buf, ceil(pr->w / 16), ceil(pr->h / 16), 1);

        // update the buf with the correct nrays for the compact pipe
        vkCmdUpdateBuffer(
                cmd_buf,
                pr->scene_buf,
                pr->idx_dispatch_cmd,
                sizeof(VkDispatchIndirectCommand),
                &(VkDispatchIndirectCommand) {ceil(pr->w / 16), ceil(pr->h / 16), 1});

        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pr->tlbvhquery_pipe);

        vkCmdDispatch(cmd_buf, ceil(pr->height / 16), ceil(pr->height / 16), 1);

        // TODO: memory barrier to protect the ray states from buffer write and pipeline

        uint32_t niters = pr->niters;
        while (niters--)
        {
                // compact
                vkCmdBindPipeline(
                        cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pr->compact_pipe);

                vkCmdDispatchIndirect(cmd_buf, pr->scene_buf, pr->idx_dispatch_cmd);

                // trace
                vkCmdBindPipeline(
                        cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pr->blbvhquery_pipe);

                vkCmdDispatchIndirect(cmd_buf, pr->scene_buf, pr->idx_dispatch_cmd);
        }

        // TODO: final color evaluation


        // transition img to present src
        hpyr_backend_cmd_simple_transition(
                        pr, 
                        cmd_buf, 
                        img,
                        ,  // TODO: fill in???
                        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        VK_ACCESS_NONE_KHR,
                        0,
                        VK_IMAGE_LAYOUT_GENERAL, 
                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_TRY(vkEndCommandBuffer(cmd_buf));

        pr->nframe++;
}

int main()
{
        srand(time(NULL));
        wpt_context wpt = {};
        wpt_init(&wpt, "HELLO BRO", 800, 600);

        uint32_t n = 2;
        while (1)
        {
                wpt_contextest2_draw(&wpt);
        }

        return 0;
}
