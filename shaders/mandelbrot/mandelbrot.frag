#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform mandelbrotConstants {
    vec4 backgroundColor;
} pushConsts;

void main() {
    outColor = pushConsts.backgroundColor;
}