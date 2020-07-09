#include "fbm.h"
#include "raymarch.h"



__kernel void main(write_only image2d_t image, struct Camera camera) {
    int2 coord = (int2) (get_global_id(0), get_global_id(1));
    float g = camera.position.z;
    float r = fbm((float2) (get_global_id(0) / 250.0f, get_global_id(1) / 152.0f), 6);
    float4 color = (float4) (r, r, g, 1.0f);
    write_imagef(image, coord, color); 
}