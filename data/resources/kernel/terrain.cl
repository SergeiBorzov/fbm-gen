#include "fbm.h"
#include "raymarch.h"

typedef struct Parameters {
    float3 sun_direction;
    float3 resolution;
} Parameters;


constant float3 sun_color = (float3) (1.1f, 1.0f, 0.35f);


float3 GetSkyColor(float3 p) {
    float3 sky_color = (float3) (0.3f, 0.4f, 0.9f);

    float atmosphere = sqrt(1.0f -  p.y);

    float scatter = pow(p.y / 1.0f ,1.0f / 30.0f);
    scatter = 1.0 - clamp(scatter, 0.8f, 1.0f);

    float3 scatterColor = mix((float3) (1.0f, 1.0f, 1.0f), (float3) (1.0f, 0.3f, 0.0f)*1.5f, scatter);
    return mix(sky_color, scatterColor, atmosphere / 3.0f);
}

__kernel void main(write_only image2d_t image, Camera camera, Parameters parameters) {
    int2 pixel_coord_i = (int2) (get_global_id(0), get_global_id(1));
    float2 pixel_coord = (float2) (pixel_coord_i.x, pixel_coord_i.y);

    Ray ray = CreateRay(&camera, parameters.resolution.xy, pixel_coord);

    float depth = 0.0f;
    float3 final_color = (float3) (0.0f, 0.0f, 0.0f);

   
    float3 albedo = (float3) (0.2f, 0.0f, 0.0f);

    if (RayMarch(ray, &depth)) {
        final_color = sun_color*albedo*dot(Normal(ray.origin + ray.direction*depth), parameters.sun_direction);
    }
    else {
        final_color = GetSkyColor(ray.direction);
    }

    // gamma correction
    final_color = pow(final_color, 1.0f/2.2f);

    float4 value = (float4) (final_color, 1.0f);
    write_imagef(image, pixel_coord_i, value); 
}