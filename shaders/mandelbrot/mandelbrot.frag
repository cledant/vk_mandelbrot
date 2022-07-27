#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform mandelbrotConstants {
    vec4 backgroundColor;
    float fbW;
    float fbH;
    float screenRatio;
    uint maxIter;
    float zoom;
    vec2 offset;
} pushConsts;

void main() {

    float x0 = 4.0 * pushConsts.zoom * pushConsts.screenRatio * ((gl_FragCoord.x / pushConsts.fbW) - 0.5 - pushConsts.offset.x);
    float y0 = 4.0 * pushConsts.zoom * ((gl_FragCoord.y / pushConsts.fbH) - 0.5 + pushConsts.offset.y);
    float x = 0.0;
    float y = 0.0;
    float x2 = 0.0;
    float y2 = 0.0;
    uint i = 0;

    while (x2 + y2 <= 4.0 && i < pushConsts.maxIter) {
        y = (2 * x * y) + y0;
        x = x2 - y2 + x0;
        x2 = pow(x, 2);
        y2 = pow(y, 2);
        ++i;
    }

    vec3 color = mix(pushConsts.backgroundColor.xyz, vec3(0.0), (float(i) / float(pushConsts.maxIter)));
    outColor = vec4(color, 1.0);
}