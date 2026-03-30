#version 450
layout(binding = 0) uniform samplerCube skybox;
layout(location = 0) in vec3 fragDir;

void main()
{
    outColor = texture(skybox, fragDir);
}
