#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

// layout(location = 0) flat out vec3 fragColor;
// layout(location = 1) out vec3 fragPosWorld;
// layout(location = 2) out vec3 fragNormalWorld;
layout(location = 0) out vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform TerrainUBO
{
	mat4	model;
	mat4	view;
	mat4	projection;
	vec4	ambientLightColor;
	vec3	lightPosition;
	vec4	lightColor;
}	ubo;

// layout(push_constant) uniform PushConstants
// {
// 	mat4	modelMatrix;
// 	mat4	normalMatrix;
// }	push;

void main()
{
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(position, 1.0);

//	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
//	fragPosWorld = worldPostion.xyz;
//	fragColor = color;
	fragTexCoord = uv;
}
