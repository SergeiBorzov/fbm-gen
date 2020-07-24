__VERTEX__
#version 330
layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_uv;

out Data {
    vec2 uv;
} vs_out;


void main() {
    vs_out.uv = v_uv;
    gl_Position = vec4(v_pos, 0.0f, 1.0f);
}

__FRAGMENT__
#version 330


/*----------------------*/
/*-------GENERAL--------*/
/*----------------------*/
in Data {
    vec2 uv;
} fs_in;

#define M_PI 3.1415926535897932384f

#define MAX_DISTANCE 28.0f
#define MAX_ITER 256
#define EPS 0.002f

#define SKY_SAMPLES_VIEW_RAY 4
#define SKY_SAMPLES_LIGHT_RAY 4
#define BETA_R vec3(5.5e-6, 13.0e-6, 22.4e-6) // scattering coefficient sea level Rayleigh
#define BETA_M vec3(21e-6) // scattering coefficient sea level Mie
#define HEIGHT_SCALE_R 7994.0f
#define HEIGHT_SCALE_M 1200.0f
#define PLANET_RADIUS  6360e3
#define PLANET_ATM_RADIUS 6420e3 //planet + atmosphere radius

uniform mat4 u_ViewMatrix;
uniform vec3 u_SunDirection;
uniform vec2 u_Resolution;
uniform float u_SunIntensity;
/*----------------------*/

/*----------------------*/
/*-----RAY-MARCHING-----*/
/*----------------------*/


struct Ray {
    vec3 origin;
    vec3 direction;
};

Ray CreateRay() {
    vec2 xy = u_Resolution*fs_in.uv - u_Resolution*0.5f;
    float z = u_Resolution.y / tan(radians(60.0f) * 0.5f);
    vec3 direction = normalize(vec3(xy.x*u_ViewMatrix[0].xyz + xy.y*u_ViewMatrix[1].xyz + z*u_ViewMatrix[2].xyz));
    Ray ray;
    ray.origin = u_ViewMatrix[3].xyz;
    ray.direction = direction;
    return ray;
}

float Random(vec2 p){
    return fract(sin(dot(p.xy , vec2(12.9898f,78.233f))) * 43758.5453f);
}

#define QUINTIC_POLYNOMIAL

vec3 Noise(vec2 p) {
    vec2 i = floor(p); // floor of p
    vec2 f = fract(p);

#ifdef QUINTIC_POLYNOMIAL
    vec2 u = f*f*f*(f*(f*6.0f - 15.0f) + 10.0f);
    vec2 du = 30.0f*f*f*(f*(f - 2.0f) + 1.0f); // derivative
#endif

#ifdef CUBIC_POLYNOMIAL
    vec2 u = f*f*(-2.0f*f+ 3.0f);
    vec2 du = f*(-6.0f*f + 6.0f); // derivative
#endif
    
    float a = Random(i + vec2(0.0f, 0.0f));
    float b = Random(i + vec2(1.0f, 0.0f));
    float c = Random(i + vec2(0.0f, 1.0f));
    float d = Random(i + vec2(1.0f, 1.0f));
    
    // Bilinear interpolation to find noise value
    float n = mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
    /*  Note:
        n can be represented this way
        n = k0 + k1*v1 + k2*v2 + k3*v1v2
        k0 = a
        k1 = b - a
        k2 = c - a
        k3 = a - b - c + d
    */

    float k = a - b - c + d;

    return vec3(-1.0f + 2.0f*n, 2.0f*du*vec2(b - a + k*f.y, c - a + k*f.x));
}

float fbmM(vec2 p) {
    float f = 0.0f, w = 0.5f;
    
    for (int i = 0; i < 16; ++i) {
        vec3 n = Noise(p);
        f += w * n.x;
        w *= 0.5f; p *= 2.0f;
    }
    return f;
}

float SceneSDF(vec3 p) {
    return p.y - fbmM(p.xz);
}

// Normal through gradient
vec3 Normal(vec3 p) {
    float x = fbmM(vec2(p.x - 0.001f, p.z)) - fbmM(vec2(p.x + 0.001f, p.z));
    float y = 2.0f*EPS;
    float z = fbmM(vec2(p.x, p.z - 0.001f)) - fbmM(vec2(p.x, p.z + 0.001f));
    return normalize(vec3(x, y, z));
}

bool RayAtmosphereIntersect(Ray ray, out float t0) {
    float a = dot(ray.direction, ray.direction);
    vec3 d = ray.origin;
    float b = 2.0f * dot(ray.direction, d);
    float c = dot(d, d) - (PLANET_ATM_RADIUS * PLANET_ATM_RADIUS);
    if (b*b - 4.0f*a*c < 0.0f) {
        return false;
    }
    t0 = (-b + sqrt((b*b) - 4.0*a*c))/(2.0*a);
    return true;    
}

float RayMarching(Ray ray) {
    float t = 0.0f;
    float d = 0.0f;
    for (int i = 0; i < MAX_ITER; i++) {
        d = SceneSDF(ray.origin + ray.direction*t);
        if (d < EPS*t || t > MAX_DISTANCE) {
            break;
        }

        t += 0.4f*d;
    }
    return d < EPS*t ? t : -1.0f;
}
/*----------------------*/


/*----------------------*/
/*------ATMOSPHERE------*/
/*----------------------*/
float PhaseRayleigh(float mu) {
    return 3.0f*(1.0f + mu*mu) / 16.0f * M_PI;
}

float PhaseMie(float g, float mu) {
    float g2 = g*g;
    float numerator = 3.0f*(1 - g2)*(1 + mu*mu);
    float denominator = 1 + g2 - 2*g*mu;
    denominator *= sqrt(denominator)*(2 + g2)*8.0f*M_PI;
    return numerator / denominator;
}


vec3 Transmittance(vec3 eye, vec3 atmosphere_point, float scaleHeight, vec3 sea_level) {
    float optical_depth = 0.0;
    float segment_length = length(eye - atmosphere_point) / SKY_SAMPLES_LIGHT_RAY;
    for (int i = 0; i < SKY_SAMPLES_LIGHT_RAY; i++) {
        vec3 sample_point = mix(eye, atmosphere_point, (float(i) + 0.5f) / SKY_SAMPLES_LIGHT_RAY);
        float sample_height = length(sample_point) - PLANET_RADIUS;
        optical_depth += exp(-sample_height / scaleHeight) * segment_length;
    }
    return exp(-1.0f * sea_level * optical_depth);
}

vec3 SkyColorInternal(vec3 eye, vec3 atmosphere_point) {
    float mu = dot(normalize(atmosphere_point - eye), u_SunDirection);

    float phaseR = PhaseRayleigh(mu);
    float phaseM = PhaseMie(0.76f, mu);

    vec3 color_R = vec3(0.0f, 0.0f, 0.0f);
    vec3 color_M = vec3(0.0f, 0.0f, 0.0f);

    float segment_length = length(atmosphere_point - eye) / SKY_SAMPLES_VIEW_RAY;

    for (int i = 0; i < SKY_SAMPLES_VIEW_RAY; i++) {
        vec3 sample_point = mix(eye, atmosphere_point, (float(i) + 0.5f) / SKY_SAMPLES_VIEW_RAY);
        float sample_height = length(sample_point) - PLANET_RADIUS;

        float t = 0.0f;

        Ray light_ray;
        light_ray.origin = sample_point;
        light_ray.direction =u_SunDirection;
        if (RayAtmosphereIntersect(light_ray, t)) {
            vec3 atmosphere_intersection = sample_point + u_SunDirection*t;

            // Rayleigh Calculations
            vec3 scatteringAtHeight_R = BETA_R * exp(-sample_height/HEIGHT_SCALE_R);
            vec3 trans1_R = Transmittance(eye, sample_point, HEIGHT_SCALE_R, BETA_R);
            vec3 trans2_R = Transmittance(sample_point, atmosphere_intersection, HEIGHT_SCALE_R, BETA_R);
            color_R += trans1_R*trans2_R*scatteringAtHeight_R*segment_length;

            // Mie Calculations
            vec3 scatteringAtHeight_M = BETA_M*exp(-sample_height/HEIGHT_SCALE_M);
            vec3 trans1_M = Transmittance(eye, sample_point, HEIGHT_SCALE_M, BETA_M);
            vec3 trans2_M = Transmittance(sample_point, atmosphere_intersection, HEIGHT_SCALE_M, BETA_M);
            color_M += trans1_M*trans2_M*scatteringAtHeight_M*segment_length;
        }
    }
    return color_R*phaseR + color_M*phaseM;
}

vec3 SkyColor(vec3 view) {
    Ray ray;
    ray.origin = vec3(0.0f, PLANET_RADIUS + 1.0f, 0.0f);
    ray.direction = view;

    float t= 0.0f;

    vec3 sky_color = vec3(0.0f, 0.0f, 0.0f);
    if (RayAtmosphereIntersect(ray, t)) {
        vec3 atmosphere_intersection = ray.origin + ray.direction*t;
        sky_color = u_SunIntensity*SkyColorInternal(ray.origin, atmosphere_intersection);
    }
    return sky_color;
}

vec3 SunColor(vec3 view, vec3 sun_color) {
    float sun_dot = clamp(dot(u_SunDirection, view), 0.0f, 1.0f);
    vec3 core = sun_color*pow(sun_dot, 6000.0f);
   
    //vec3 halo1 = haloclr1 * pow ( sundot, 50.0);
    //vec3 halo2 = haloclr2 * pow ( sundot, 20.0);
    //vec3 sun = core + halo1 *0.5 + halo2 *0.9 ;

    return core;
}

/* Atmosphere - end */

/*   Move to another shader  */
/*---------------------------*/
/*------POST-PROCESSING------*/
/*---------------------------*/
vec3 ToneMapping(vec3 color) {
	color = max(vec3(0.0f), color - vec3(0.004f));
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
	return color;
}
/*--------------------*/
out vec4 final_color;

void main() {

    Ray ray = CreateRay();

    float t = RayMarching(ray);

    vec3 color = vec3(0.0f);
    if (t > 0.0f) {
        color = vec3(0.5f, 0.5f, 0.5f)*dot(Normal(ray.origin + ray.direction*t), u_SunDirection);
    }
    else {
        color = SkyColor(ray.direction) + SunColor(ray.direction, vec3(1.0f, 1.0f, 0.25f));
    }

    final_color = vec4(color, 1.0f);
}