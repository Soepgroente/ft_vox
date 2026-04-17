#version 450

layout(set = 0, binding = 0) uniform ViewProjectUBO
{
	mat4	view;
	mat4	projection;
}	ubo;

layout(set = 0, binding = 1) uniform MeshData
{
	mat4	model;
	mat4	normal;
} meshData;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureUV;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTextureUV;


void main()
{
	// in case model != Id(mat4) use those
	// vec4 worldPos = meshData.model * vec4(position, 1.0f);
	// fragNormal = meshData.normal * normal;

	vec4 worldPos = vec4(position, 1.0f);
	fragPos = worldPos.xyz;
	fragNormal = normal;
	fragTextureUV = textureUV;
	gl_Position = ubo.projection * ubo.view * worldPos;
}
