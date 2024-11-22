#pragma once

#include "vec3.h"

typedef struct 
{       
        vec3_t pos, dir;
        int hit_id; // id of the BVH that it hit
} ray_state_t;