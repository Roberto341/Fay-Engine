#version 330 core

layout (location = 0) in vec3 position;     // 3D position
layout (location = 1) in vec2 uv;
layout (location = 2) in float tid;
layout (location = 3) in vec4 color;

uniform mat4 pr_matrix;                    // Projection matrix
uniform mat4 vw_matrix;					  // View matrix
uniform mat4 ml_matrix;      // Model matrix
out DATA
{
	vec4 worldPosition;
	vec2 uv;
	float tid;
	vec4 color;
} vs_out;

void main()
{
	vec4 worldPos = ml_matrix * vec4(position, 1.0);
	gl_Position = pr_matrix * vw_matrix * ml_matrix * vec4(position, 1.0);
	//gl_Position =  vec4(position, 1.0);
	vs_out.worldPosition = worldPos;
	vs_out.uv = uv;
	vs_out.tid = tid;
	vs_out.color = color;
}