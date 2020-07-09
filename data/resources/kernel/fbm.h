
#ifndef FBM_HEADER
#define FBM_HEADER

float random(float2 p){
    float floor;
    return fract(sin(dot(p.xy , (float2)(12.9898f,78.233f))) * 43758.5453f, &floor);
}

float noise2D(float2 p) {
    float2 fr;
    float2 fl;
    fr = fract(p, &fl);

    float a = random(fl);
    float b = random(fl + (float2)(1.0f, 0.0f));
    float c = random(fl + (float2)(0.0f, 1.0f));
    float d = random(fl + (float2)(1.0f, 1.0f));

    

    float2 u = smoothstep(0.0f, 1.0f, fr);

    return mix(a, b, u.x) + (c - a)*u.y*(1.0 - u.x) + (d - b)*u.x*u.y;
}

float fbm(float2 p, int octaves) {
    float result = 0.0f;
    float amplitude = 0.5f;

    for (int i = 0; i < octaves + 2; i++) {
        result += amplitude*noise2D(p);
        p *= 5.0f;
        amplitude *= 0.5f;
    }
    return result;
}

#endif /* End of FBM_HEADER */