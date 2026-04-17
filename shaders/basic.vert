#version 450

layout(set = 0, binding = 0) uniform ViewProjectUBO
{
	mat4	view;
	mat4	projection;
}	ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec2 fragTexCoord;


void main()
{
	gl_Position = ubo.projection * ubo.view * vec4(position, 1.0);
	fragTexCoord = uv;
}
