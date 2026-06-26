#version 450

layout(set=0, binding=0) uniform CameraUBO
{
	mat4 view;
	mat4 proj;
}	cam;

layout(push_constant) uniform Push
{
	vec3 voxelMin;   // selected voxel min corner in world space
	float inflate;   // e.g. 0.02
	vec4 color;
}	pc;

layout(location=0) in vec3 inPos; // 0..1 cube vertices for the line box
layout(location=0) out vec4 vColor;

void main()
{
	vec3 p = inPos;
	// inflate around cube center to avoid z-fighting
	p = (p - vec3(0.5)) * (1.0 + pc.inflate) + vec3(0.5);

	vec3 worldPos = pc.voxelMin + p;
	gl_Position = cam.proj * cam.view * vec4(worldPos, 1.0);
	vColor = pc.color;
}