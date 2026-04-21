#version 450

layout(set = 0, binding = 1) uniform LightUBO
{
	vec4 lightPos;
	vec4 lightColor;
	vec4 viewPos;
} lightUbo;

layout(set = 1, binding = 0) uniform sampler2D textSampler;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTextureUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform MeshData {
	mat4 modelMatrix;
	mat4 normalMatrix;

	vec4  ambientClr;
	vec4  diffuseClr;
	vec4  specularClr;
	float shininess;
	float opacity;
	int   refractionIndex;
	int   illuminationModel;
} meshData;

void main()
{
	// Ambient
	vec3 ambient = meshData.ambientClr.xyz * lightUbo.lightColor.xyz;

	// Diffuse
	vec3 normalFrag = normalize(fragNormal);
	vec3 lightDir = normalize(lightUbo.lightPos.xyz - fragPos);
	float diff = max(dot(normalFrag, lightDir), 0.0);
	vec3 diffuse = diff * lightUbo.lightColor.xyz;		// meshData.diffuseClr.xyz * 

	// // Specular (Blinn-Phong)
	// vec3 viewDir = normalize(lightUbo.viewPos.xyz - fragPos);
	// vec3 halfwayDir = normalize(lightDir + viewDir);
	// float spec = pow(max(dot(normalFrag, halfwayDir), 0.0), meshData.shininess);
	// vec3 specular = meshData.specularClr.xyz * spec * lightUbo.lightColor.xyz;

	// outColor = vec4(result, meshData.opacity);

	// vec4 textColor = texture(textSampler, fragTextureUV);
	vec3 result = (ambient + diffuse) * meshData.diffuseClr.xyz;

	outColor = vec4(result, 1.0f);
}
