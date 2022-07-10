#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertexPosition;

void main() {
    vec4 pos = vec4(inVertexPosition, 1.0);
    gl_Position = pos.xyzw;
}