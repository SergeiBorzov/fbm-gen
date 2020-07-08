

__kernel void main(write_only image2d_t image) {
    int2 coord = (int2) (get_global_id(0), get_global_id(1));
    float r = (float)(get_global_id(0) % 255) / 255.0f;
    float4 color = (float4) (r, 1.0f, 0.0f, 1.0f);
    write_imagef(image, coord, color);
}