#version 450

layout(set = 0, binding = 2) uniform LightUBO
{
	vec4 lightPos;
	vec4 lightColor;
	vec4 viewPos;
} lightUbo;

layout(set = 0, binding = 3) uniform MeshMaterial
{
	vec4	ambientClr;
	vec4	diffuseClr;
	vec4	specularClr;
	float	shininess;
	float	opacity;
	int		refractionIndex;
	int		illuminationModel;
} material;

layout(set = 1, binding = 0) uniform sampler2D textSampler;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTextureUV;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 textColor = texture(textSampler, fragTextureUV).rgb;
	vec3 normalFrag = normalize(fragNormal);

	// Ambient
	vec3 ambient = material.ambientClr.xyz * lightUbo.lightColor.xyz;

	// Diffuse
	vec3 lightDir = normalize(lightUbo.lightPos.xyz - fragPos);
	float diff = max(dot(normalFrag, lightDir), 0.0);
	vec3 diffuse = diff * lightUbo.lightColor.xyz;

	// Specular (Blinn-Phong)
	vec3 viewDir = normalize(lightUbo.viewPos.xyz - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normalFrag, halfwayDir), 0.0), material.shininess);
	vec3 specular = material.specularClr.xyz * spec * lightUbo.lightColor.xyz;

	vec3 result = (ambient + diffuse + specular) * textColor;
	outColor = vec4(result, 1.0);
}
