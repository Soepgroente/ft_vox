#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform SkyboxUBO
{
	mat4	view;
	mat4	projection;
}	ubo;

layout(location = 0) out vec3 fragDir;

void main()
{
    fragDir = inPosition;
	// mat4 viewNoTranslation = mat4(mat3(ubo.view)); // remove translation component of the view matrix
	vec4 pos = ubo.projection * ubo.view * vec4(inPosition, 1.0);
	gl_Position = pos.xyww;		// forcing fragment of far plane (depth = 1.0)
}
