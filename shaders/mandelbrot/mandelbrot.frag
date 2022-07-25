#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform mandelbrotConstants {
    vec4 backgroundColor;
} pushConsts;

#define MAX_ITER 250

void main() {

    float x0 = ((gl_FragCoord.x / 750.0f) - 0.5) * 4.0;
    float y0 = ((gl_FragCoord.y / 750.0f) - 0.5) * 4.0;
    float x = 0.0;
    float y = 0.0;
    float x2 = 0.0;
    float y2 = 0.0;
    uint i = 0;

    while (x2 + y2 <= 4.0 && i < MAX_ITER) {
        y = (2 * x * y) + y0;
        x = x2 - y2 + x0;
        x2 = pow(x, 2);
        y2 = pow(y, 2);
        ++i;
    }

    vec3 color = mix(pushConsts.backgroundColor.xyz, vec3(0.0), (float(i) / float(MAX_ITER)));
    outColor = vec4(color, 1.0);
}