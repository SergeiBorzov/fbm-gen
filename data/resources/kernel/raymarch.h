#ifndef RAYMARCH_HEADER
#define RAYMARCH_HEADER

#define MAX_DISTANCE 1000.0f
#define MAX_ITER 256
#define EPS 0.0001f


typedef struct Camera {
    float3 position;
    float3 right;
    float3 up;
    float3 front;
} Camera;

typedef struct Ray {
    float3 origin;
    float3 direction;
} Ray;

float SceneSDF(float3 point) {    
    return length(point) - 1.0f;
}

Ray CreateRay(Camera* cam, float2 resolution, float2 pixel_coord) {
    float2 xy = pixel_coord - resolution / 2.0f;
    float z = resolution.y / tan(radians(45.0f) / 2.0f);
    float3 direction = normalize((float3) (xy.x * cam->right + xy.y * cam->up + z* cam->front));
    Ray ray = {cam->position, direction};
    return ray;
}

// Normal through gradient
float3 Normal(float3 p) {
    return normalize(p);
}


bool RaySphereIntersect(Ray ray, float sphereRadius, float* t0) {
    float a = dot(ray.direction, ray.direction);
    float3 d = ray.origin;
    float b = 2.0f * dot(ray.direction, d);
    float c = dot(d, d) - (sphereRadius * sphereRadius);
    if (b*b - 4.0f*a*c < 0.0f) {
        return false;
    }
    *t0 = (-b + sqrt((b*b) - 4.0*a*c))/(2.0*a);
    return true;    
}

bool RayMarch(Ray ray, float* depth) {
    float currentDepth = 0.0f;
    float currentDistance = 0.0f;
    for (int i = 0; i < MAX_ITER; i++) {
        currentDistance = SceneSDF(ray.origin + ray.direction*currentDepth);
        
        if (currentDistance < EPS) {
            *depth = currentDepth;
			return true;
        }
        currentDepth += currentDistance;
        if (currentDepth > MAX_DISTANCE) {
            return false;
        }
    }
    return false;
}

#endif /* End of RAYMARCH_HEADER */