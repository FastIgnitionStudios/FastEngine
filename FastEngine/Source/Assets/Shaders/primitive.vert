#version 450

layout(set = 0, binding = 0) uniform  SceneData{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec4 ambientColor;
    vec4 sunlightDirection; //w for sun power
    vec4 sunlightColor;
} sceneData;

layout(buffer_reference, std430) readonly buffer VertexBuffer{
    Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
    mat4 render_matrix;
    VertexBuffer vertexBuffer;
} PushConstants;

void main() {
    //const array of positions for the triangle
    const vec3 positions[6] = vec3[6](
    vec3(0.f,0.f, 0.0f),
    vec3(0.f,1.f, 0.0f),
    vec3(1.f,0.f, 0.0f),
    vec3(1.f,0.f, 0.0f),
    vec3(1.f,-1.f, 0.0f),
    vec3(0.f,0.f, 0.0f)
    );
    vec4 position = vec4(positions[gl_VertexIndex], 1.0f);
    gl_Position = vec4(sceneData.viewproj * PushConstants.render_matrix * position);
}
