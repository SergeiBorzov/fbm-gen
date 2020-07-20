#include "fbm.h"
#include "raymarch.h"

typedef struct Parameters {
    float2 resolution;
} Parameters;

__kernel void main(write_only image2d_t image, Camera camera, Parameters parameters) {
    int2 pixel_coord_i = (int2) (get_global_id(0), get_global_id(1));
    float2 pixel_coord = (float2) (pixel_coord_i.x, pixel_coord_i.y);

    Ray ray = CreateRay(&camera, parameters.resolution, pixel_coord);

    float depth = 0.0f;
    float4 color = (float4) (0.0f, 0.0f, 0.0f, 1.0f);
    if (RayMarch(ray, &depth)) {
        color = (float4) (1.0f, 1.0f, 1.0f, 1.0f);
    }


    write_imagef(image, pixel_coord_i, color); 
}