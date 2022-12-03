#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_fp64 : enable

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform mandelbrotConstants {
    vec4 color;
    vec4 maxIterColor;
    double fbW;
    double fbH;
    uint maxIter;
    double zoom;
    double zoomMultScreenRatio;
    double offsetX;
    double offsetY;
} pushConsts;

void main() {

    double x0 = pushConsts.zoomMultScreenRatio * ((gl_FragCoord.x / pushConsts.fbW) - 0.5) + pushConsts.offsetX;
    double y0 = pushConsts.zoom * ((gl_FragCoord.y / pushConsts.fbH) - 0.5) + pushConsts.offsetY;
    double x = 0.0;
    double y = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;
    uint i = 0;

    while ((x2 + y2) <= 4.0 && i < pushConsts.maxIter) {
        y = (2.0 * x * y) + y0;
        x = x2 - y2 + x0;
        x2 = x * x;
        y2 = y * y;
        ++i;
    }

    if (i >= pushConsts.maxIter) {
        outColor = vec4(pushConsts.maxIterColor.xyz, 1.0);
        return;
    }
    vec3 mixColor = mix(pushConsts.color.xyz, pushConsts.maxIterColor.xyz, (float(i) / float(pushConsts.maxIter)));
    outColor = vec4(mixColor, 1.0);
}