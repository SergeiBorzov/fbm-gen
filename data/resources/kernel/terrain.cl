#include "fbm.h"
#include "raymarch.h"

typedef struct Parameters {
    float3 sun_direction;
    float3 resolution;
} Parameters;


/* Sky */
#define SKY_SAMPLES_VIEW_RAY 8
#define SKY_SAMPLES_LIGHT_RAY 10
#define BETA_R (float3) (5.5e-6, 13.0e-6, 22.4e-6) // scattering coefficient sea level Rayleigh
#define BETA_M (float3) (21e-6) // scattering coefficient sea level Mie
#define HEIGHT_SCALE_R 7994
#define HEIGHT_SCALE_M 1200
#define PLANET_RADIUS  6360e3
#define PLANET_ATM_RADIUS 6420e3 //planet + atmosphere radius


constant float3 sun_color = (float3) (1.0f, 1.0f, 0.35f);

float PhaseRayleigh(float mu) {
    return 3.0f*(1.0f + mu*mu) / 16.0f * M_PI_F;
}

float PhaseMie(float g, float mu) {
    float g2 = g*g;
    float numerator = 3.0f*(1 - g2)*(1 + mu*mu);
    float denominator = 1 + g2 - 2*g*mu;
    denominator *= sqrt(denominator)*(2 + g2)*8.0f*M_PI_F;
    return numerator / denominator;
}


float3 Transmittance(float3 eye, float3 atmosphere_point, float scaleHeight, float3 sea_level) {
    float optical_depth = 0.0;
    float segment_length = length(eye - atmosphere_point) / SKY_SAMPLES_LIGHT_RAY;
    for (int i = 0; i < SKY_SAMPLES_LIGHT_RAY; i++) {
        float3 sample_point = mix(eye, atmosphere_point, ((float)(i) + 0.5f) / SKY_SAMPLES_LIGHT_RAY);
        float sample_height = length(sample_point) - PLANET_RADIUS;
        optical_depth += exp(-sample_height / scaleHeight) * segment_length;
    }
    return exp(-1.0f * sea_level * optical_depth);
}

float3 SkyColorInternal(float3 eye, float3 atmosphere_point, float3 sun_direction) {
    float mu = dot(normalize(atmosphere_point - eye), sun_direction);

    float phaseR = PhaseRayleigh(mu);
    float phaseM = PhaseMie(0.76f, mu);

    float3 color_R = (float3) (0.0f, 0.0f, 0.0f);
    float3 color_M = (float3) (0.0f, 0.0f, 0.0f);

    float segment_length = length(atmosphere_point - eye) / SKY_SAMPLES_VIEW_RAY;

    for (int i = 0; i < SKY_SAMPLES_VIEW_RAY; i++) {
        float3 sample_point = mix(eye, atmosphere_point, ((float)(i) + 0.5f) / SKY_SAMPLES_VIEW_RAY);
        float sample_height = length(sample_point) - PLANET_RADIUS;

        float t = 0.0f;

        Ray light_ray;
        light_ray.origin = sample_point;
        light_ray.direction = sun_direction;
        if (RaySphereIntersect(light_ray, PLANET_ATM_RADIUS, &t)) {
            float3 atmosphere_intersection = sample_point + sun_direction*t;

            // Rayleigh Calculations
            float3 scatteringAtHeight_R = BETA_R * exp(-sample_height/HEIGHT_SCALE_R);
            float3 trans1_R = Transmittance(eye, sample_point, HEIGHT_SCALE_R, BETA_R);
            float3 trans2_R = Transmittance(sample_point, atmosphere_intersection, HEIGHT_SCALE_R, BETA_R);
            color_R += trans1_R*trans2_R*scatteringAtHeight_R*segment_length;

            // Mie Calculations
            float3 scatteringAtHeight_M = BETA_M*exp(-sample_height/HEIGHT_SCALE_M);
            float3 trans1_M = Transmittance(eye, sample_point, HEIGHT_SCALE_M, BETA_M);
            float3 trans2_M = Transmittance(sample_point, atmosphere_intersection, HEIGHT_SCALE_M, BETA_M);
            color_M += trans1_M*trans2_M*scatteringAtHeight_M*segment_length;
        }
    	
        color_R = 2.0f * phaseR * color_R;
        color_M = 2.0f * phaseM * color_M;
    
        return color_R + color_M;
    }
}

float3 SkyColor(float3 direction, float3 sun_direction) {
    Ray ray;
    ray.origin = (float3) (0.0f, PLANET_RADIUS + 1.0f, 0.0f);
    ray.direction = direction;

    float t= 0.0f;

    float3 sky_color = (float3) (0.0f, 0.0f, 0.0f);
    if (RaySphereIntersect(ray, PLANET_ATM_RADIUS, &t)) {
        float3 atmosphere_intersection = ray.origin + ray.direction*t;
        sky_color = SkyColorInternal(ray.origin, atmosphere_intersection, sun_direction);
    }
    return sky_color;
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
        final_color = SkyColor(ray.direction, parameters.sun_direction);
    }

    // gamma correction
    final_color = pow(final_color, 1.0f/2.2f);

    float4 value = (float4) (final_color, 1.0f);
    write_imagef(image, pixel_coord_i, value); 
}