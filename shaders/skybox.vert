#version 450


layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4	projectionViewMatrix;
	vec4	ambientLightColor;
	vec3	lightPosition;
	vec4	lightColor;
}	ubo;

void main()
{
	mat4 viewNoTranslation = mat4(mat3(projectionViewMatrix)); // remove translation component of the view matrix
	vec4 pos = proj * viewNoTranslation * vec4(aPos, 1.0);
	gl_Position = pos.xyww;		// forcing fragment of far plane (depth = 1.0)
}
