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

#define MAX_DISTANCE 50.0f
#define MAX_ITER 512
#define EPS 0.002f

#define SKY_SAMPLES_VIEW_RAY 6
#define SKY_SAMPLES_LIGHT_RAY 6
#define BETA_R vec3(5.5e-6, 13.0e-6, 22.4e-6) // scattering coefficient sea level Rayleigh
#define BETA_M vec3(21e-6) // scattering coefficient sea level Mie
#define HEIGHT_SCALE_R 7994.0f
#define HEIGHT_SCALE_M 1200.0f
#define PLANET_RADIUS  6360e3
#define PLANET_ATM_RADIUS 6420e3 //planet + atmosphere radius

#define SOFT_SHADOW_ITERATIONS 64


uniform vec2 u_Resolution;
uniform mat4 u_ViewMatrix;

uniform vec3 u_SunDirection;
uniform vec3 u_SunColor;
uniform float u_SunIntensity;
uniform float u_SunSize;

uniform float u_FbmFrequency;
uniform float u_FbmAmplitude;
uniform int u_FbmOctaves;
#ifdef USE_DERIVATIVES
uniform float u_FbmSmoothness;
#endif

uniform float u_TerrainHeight;


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


float Random(float n) {
    return fract(sin(n)*43758.5453123);
}

float Random(vec2 p){
    return fract(sin(dot(p.xy , vec2(12.9898f,78.233f))) * 43758.5453f);
}

//#define QUINTIC_POLYNOMIAL
//#define USE_DERIVATIVES

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
    
    float n = i.x + i.y*124.0f;
    float a = Random(i + vec2(0.0f, 0.0f));
    float b = Random(i + vec2(1.0f, 0.0f));
    float c = Random(i + vec2(0.0f, 1.0f));
    float d = Random(i + vec2(1.0f, 1.0f));
    
    // Bilinear interpolation to find noise value
    float value = mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
    /*  Note:
        n can be represented this way
        n = k0 + k1*v1 + k2*v2 + k3*v1v2
        k0 = a
        k1 = b - a
        k2 = c - a
        k3 = a - b - c + d
    */

    float k = a - b - c + d;

    return vec3(-1.0f + 2.0f*value, 2.0f*du*vec2(b - a + k*f.y, c - a + k*f.x));
}

/* fbmM - for ray marching */
const mat2 m2 = mat2(0.8,-0.6,0.6,0.8);

float fbmM(vec2 p) {
    vec2 scaled_p = p*u_FbmFrequency;
    float f = 0.0f, w = 0.5f;
#ifdef USE_DERIVATIVES
    vec2  d = vec2(0.0);
#endif
    for (int i = 0; i < u_FbmOctaves; i++) {
        vec3 n =  u_FbmAmplitude*Noise(scaled_p);
#ifdef USE_DERIVATIVES
        d += w*n.yz;
        f += w*n.x/(1.0f + u_FbmSmoothness*dot(d, d));
#else
        f += w*n.x;
#endif
        w *= 0.5f; 
        scaled_p = m2*scaled_p*2.0f;
    }
    return smoothstep(-.95, .5, f) * f * u_TerrainHeight;
}

/* fbmH - for computing normal */
/* Has 7 more iterations */
float fbmH(vec2 p) {
    vec2 scaled_p = p*u_FbmFrequency;
    float f = 0.0f, w = 0.5f;
#ifdef USE_DERIVATIVES
    vec2  d = vec2(0.0);
#endif
    for (int i = 0; i < u_FbmOctaves + 7; i++) {
        vec3 n = u_FbmAmplitude*Noise(scaled_p);
#ifdef USE_DERIVATIVES
        d += w*n.yz;
        f += w*n.x / (1.0f + u_FbmSmoothness*dot(d, d));
#else
        f += w*n.x;
#endif
        w *= 0.5f; 
        scaled_p = m2*scaled_p*2.0f;
    }
    return smoothstep(-.95, .5, f) * f * u_TerrainHeight;
}

float SceneSDF(vec3 p) {
    return p.y - fbmM(p.xz);
}

// Normal through gradient
vec3 Normal(vec3 p) {
    float x = fbmH(vec2(p.x - 0.001f, p.z)) - fbmH(vec2(p.x + 0.001f, p.z));
    float y = 2*EPS;
    float z = fbmH(vec2(p.x, p.z - 0.001f)) - fbmH(vec2(p.x, p.z + 0.001f));
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

// Formulas from Inigo Quilez:
// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float SoftShadow(Ray ray) {
    float t = 0.01f;
    float shadow = 1.0f;
    float previousDistance = 1e20;
  
    for (int i = 0; i < SOFT_SHADOW_ITERATIONS; i++) {
        float d = SceneSDF(ray.origin + ray.direction*t);
       
        float y = d*d/(2.0*previousDistance);
        float g = sqrt(d*d-y*y);
        shadow = min(shadow, 10.0f*g/max(0.0,t-y) );
        previousDistance = d;

        t += d;
        if (shadow < 0.0001f || t > MAX_DISTANCE) break;
    }
    shadow = 3*shadow*shadow - 2*shadow*shadow*shadow;
    return clamp(shadow, 0.0f, 1.0f);
}

float AmbientOcclusion(vec3 p, vec3 n) {
    float step = 8.0f;
    float ao = 0.0f;
    float dist;
    for (int i = 0; i < 3; i++) {
        dist = step * float(i);
		ao += max(0.0f, (dist - SceneSDF(p + n * dist)) / dist);  
    }
    return 1.0f - ao;
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

// Based on https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky/simulating-colors-of-the-sky
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
        sky_color = u_SunColor*SkyColorInternal(ray.origin, atmosphere_intersection);
    }
    return sky_color;
}

vec3 SunColor(vec3 view) {
    float sun_dot = max(dot(u_SunDirection, view), 0.0f);
    vec3 core = u_SunColor*pow(sun_dot, 10000.0f / u_SunSize);
    return core;
}

// https://www.iquilezles.org/www/articles/fog/fog.htm - better fog
vec3 ApplyFog(vec3 color, vec3 eye, float distance) {
    float fog_amount = 1.0f - exp(-distance*0.025f);
    float sun_amount = max(dot(eye, u_SunDirection), 0.0f);
    vec3 fog_color= mix(vec3(0.5f, 0.6f, 0.7f), u_SunColor, pow(sun_amount, 16.0f))*u_SunIntensity*0.2f;
    return mix(color, fog_color, fog_amount);
}

vec3 TerrainColor(vec3 point, vec3 normal) {
    vec3 rock = vec3(0.10f, 0.05f, 0.03f);
    vec3 snow = vec3 (0.9f, 0.9f, 0.9f);
    vec3 grass = vec3(0.08f, 0.15f, 0.05f);

    /* if surface is flat add grass */
    vec3 color = mix(rock, grass,smoothstep(0.95,1.0, normal.y));
    /* if not add more rocks */
    color = mix(rock, color, smoothstep(.4, .7, normal.y));

    float h = point.y + fbmM(point.xz);
    color = mix(color, snow, smoothstep(1.4f, 3.0f, h));

    float r = Noise(7.0*point.xz).x;
    color = (r*0.05f + 0.95f)*0.9f*color;
    /* vec3 color = mix(grass, rock, smoothstep(0., .9 * u_TerrainHeight,
								point.y)); 
            color = mix(color, snow, smoothstep(.9 * u_TerrainHeight,
							1.4 * u_TerrainHeight, point.y));
            color = mix(rock, color, smoothstep(.4, .7, normal.y));*/
    //color = mix(color, snow, smoothstep(0.7f*u_TerrainHeight, 1.4f*u_TerrainHeight, point.y));
    //color = mix(rock, color, smoothstep(0.25f*u_TerrainHeight, 0.7f*u_TerrainHeight, normal.y));

  
    // here we begin coloring the terrain. The base color mixes between
    // a dark brown base color and a slightly more colorful brown at the top
    // terrain2 is used here as just another random noise function whose input scales
    // are on the same magnitude as the position; it's used to give the vertical bands
    // in the terrain
    //float heightMix = clamp(fbmH(vec2(point.x, point.y*48.0f))/200.0f, 0.0f, 1.0f);
    // mix the color with a more reddish hue if the normal points more up (how flat it is)
    //vec3 col = (r*0.25f + 0.75f)*0.9f*mix(vec3(0.10f, 0.05f, 0.03f), vec3(0.13f, 0.10f, 0.08f), heightMix);
    //col = mix( col, 0.17*vec3(0.5,.23,0.04)*(0.50+0.50*r),smoothstep(0.70,0.9, normal.y) );
    // and if they're *really* flat, give it some green for grass
    //col = mix( col, 0.10*vec3(0.2,.30,0.00)*(0.25+0.75*r),smoothstep(0.95,1.0, normal.y) );
    //col *= 0.75;
     // height factor -- higher places have more snow (h=1), lower places have no snow (h=0)
    //float h = smoothstep(55.0,80.0,point.y + 25.0*fbmM(0.01*point.xz) );
    // normal+cliff factor -- land that is flatter gets more snow, cliff-like land doesn't get much snow
    // normal+cliff factor is also scaled by height, aka higher up the slope factor is stronger
    //float e = smoothstep(1.0-0.5*h,1.0-0.1*h,normal.y);
    // wind directional factor -- if you have a high x slope, put less snow.
    // so hilly terrain that is going up in one direction will have snow,
    // going down on the other side will have no snow
    //float o = 0.3 + 0.7*smoothstep(0.0,0.1,normal.x+h*h);
    //float s = h*e*o;
    //s = smoothstep( 0.1, 0.9, s );
    // mix color with dark grey color for snow
    //col = mix( col, 0.4*vec3(0.6,0.65,0.7), s );*/
    return color;
}

/* Atmosphere - end */

/*   Move to another shader  */
/*---------------------------*/
/*------POST-PROCESSING------*/
/*---------------------------*/
vec3 simpleReinhardToneMapping(vec3 color)
{
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	color = pow(color, vec3(1. / 2.2f));
	return color;
}
/*--------------------*/
out vec4 final_color;

void main() {

    Ray ray = CreateRay();

    float t = RayMarching(ray);

    vec3 color = vec3(0.0f);
    if (t > 0.0f) {
        vec3 hit_point = ray.origin + t*ray.direction;
        vec3 hit_normal = Normal(hit_point);
        hit_point += hit_normal*1000*EPS;

        Ray shadowRay;
        shadowRay.origin = hit_point;
        shadowRay.direction = u_SunDirection;

        float shadow = SoftShadow(shadowRay);
        vec3 cshadow = pow(vec3(shadow), u_SunColor);

        // First light - sky
        vec3 terrain_color = TerrainColor(hit_point, hit_normal);
        color = cshadow*terrain_color*max(dot(hit_normal, u_SunDirection), 0.0f)*u_SunIntensity;
        // Second light - sun
        color += terrain_color*AmbientOcclusion(hit_point, hit_normal)*SkyColor(hit_normal)*u_SunIntensity*0.2f;   
        // Third light - Indirect approximation
        color += terrain_color*max(dot(hit_normal, u_SunDirection*vec3(-1.0f, 0.0f, -1.0f)), 0.0f)*0.1f*u_SunIntensity;
        color = ApplyFog(color, ray.direction, t);
        
    }
    else {
        color = (SkyColor(ray.direction) + SunColor(ray.direction))*u_SunIntensity;
    }

    color = simpleReinhardToneMapping(color);
    final_color = vec4(color, 1.0f);
}