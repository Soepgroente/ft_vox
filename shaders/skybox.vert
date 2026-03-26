#version 450

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4	model;
	mat4	view;
	mat4	projection;
	vec4	ambientLightColor;
	vec3	lightPosition;
	vec4	lightColor;
}	ubo;

void main()
{
	mat4 viewNoTranslation = mat4(mat3(ubo.view)); // remove translation component of the view matrix
	vec4 pos = ubo.projection * viewNoTranslation * vec4(position, 1.0);
	gl_Position = pos.xyww;		// forcing fragment of far plane (depth = 1.0)
}
