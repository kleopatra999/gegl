static const char* gaussian_blur_cl_source =
"__kernel void fir_ver_blur(const global float4 *src_buf,                      \n"
"                              const int src_width,                            \n"
"                              global float4 *dst_buf,                         \n"
"                              constant float *cmatrix,                        \n"
"                              const int matrix_length,                        \n"
"                              const int yoff)                                 \n"
"{                                                                             \n"
"    int gidx = get_global_id(0);                                              \n"
"    int gidy = get_global_id(1);                                              \n"
"    int gid  = gidx + gidy * get_global_size(0);                              \n"
"                                                                              \n"
"    int radius = matrix_length / 2;                                           \n"
"    int src_offset = gidx + (gidy + yoff) * src_width;                        \n"
"                                                                              \n"
"    float4 v = 0.0f;                                                          \n"
"                                                                              \n"
"    for (int i=-radius; i <= radius; i++)                                     \n"
"      {                                                                       \n"
"        v += src_buf[src_offset + i * src_width] * cmatrix[i+radius];         \n"
"      }                                                                       \n"
"                                                                              \n"
"    dst_buf[gid] = v;                                                         \n"
"}                                                                             \n"
"                                                                              \n"
"__kernel void fir_hor_blur(const global float4 *src_buf,                      \n"
"                              const int src_width,                            \n"
"                              global float4 *dst_buf,                         \n"
"                              constant float *cmatrix,                        \n"
"                              const int matrix_length,                        \n"
"                              const int xoff)                                 \n"
"{                                                                             \n"
"    int gidx = get_global_id(0);                                              \n"
"    int gidy = get_global_id(1);                                              \n"
"    int gid  = gidx + gidy * get_global_size(0);                              \n"
"                                                                              \n"
"    int radius = matrix_length / 2;                                           \n"
"    int src_offset = gidy * src_width + (gidx + xoff);                        \n"
"                                                                              \n"
"    float4 v = 0.0f;                                                          \n"
"                                                                              \n"
"    for (int i=-radius; i <= radius; i++)                                     \n"
"      {                                                                       \n"
"        v += src_buf[src_offset + i] * cmatrix[i+radius];                     \n"
"      }                                                                       \n"
"                                                                              \n"
"    dst_buf[gid] = v;                                                         \n"
"}                                                                             \n"
;
