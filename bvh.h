#pragma once

#include "vec3.h"

typedef struct 
{
        vec3_t x, y;
} aabb_t;

typedef struct 
{
        uint32_t child, object_id;
        aabb_t aabb;
} bvh_node_t;

typedef struct
{       
        uint32_t nnodes, cap_nodes, szmeshlet;
        bvh_node_t *pnodes; 
} bvh_t;

static bool bvh_node_is_leaf(bvh_node_t *pnode)
{       
        return !pnode->child;
}

static void bvh_init(bvh_t *pbvh)
{       
        if (!pbvh)
                return;

        ++nprealloc_nodes;
        pbvh->nnodes = nprealloc_nodes;
        pbvh->pnodes = malloc(sizeof(bvh_node_t) * nprealloc_nodes);
}

uint32_t bvh_add_node(bvh_t *pbvh, bvh_node_t node)
{

}       


void bvh_node_update_bounds(bvh_node_t *pnode, uint32_t meshlet_id)
{
        for (uint32_t i = 0; i < bvh_node_t->sz_meshlet)
        {

        }
}


























void bvh_destroy(bvh_t *pbvh)
{
        if (!pbvh)      
                return;

        free(pbvh->pnodes);
}

void bvh_copy(bvh_t *pbvh, bvh_node_t *pdest)
{
        if (!pbvh || !pdest)
                return;
                
        memcpy(pdest, pbvh->pnodes, pbvh->nnodes * sizeof(bvh_node_t));
}

void bvh_clear(bvh_t *pbvh)
{
        if (!pbvh)
                return;

        memset(pbvh->pnodes, 0, pbvh->cap_nodes * sizeof(bvh_node_t));
        memset(pbvh, 0, sizeof(bvh_t));
}       

void blbvh_init(bvh_t *pbvh, uint32_t nprealloc_nodes, uint32_t sz_meshlet, uint32_t nindices, uint32_t *pindices, vertex_t *pvertices)
{
        if (!pbvh || !pindices || !pvertices)
                return;

        bvh_init(pbvh, nprealloc_nodes);

        pbvh->pnodes[0]
}

void tlbvh_init(bvh_t *pbvh, uint32_t nprealloc_nodes)
{
        if (!pbvh)
                return;

        bvh_init(pbvh, nprealloc_nodes);


}





void blbvh_copy_to_gpu(bvh_t *pbvh, void *pdest)
{
        
}       