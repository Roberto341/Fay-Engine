#version 330 core

layout (location = 0) out vec4 fragColor;

uniform sampler2D textures[16];    // Up to 16 bound textures
uniform vec4 colour = vec4(1.0);   // Optional fallback

in DATA
{
	vec4 worldPosition;
	vec2 uv;
	float tid;
	vec4 color;
} fs_in;

void main()
{
	vec4 texColor = fs_in.color;

	if (fs_in.tid > 0.0)
	{
		int tid = int(fs_in.tid - 0.5); // Safe rounding
		vec4 sampled = texture(textures[tid], fs_in.uv);
		texColor *= sampled;           // Modulate texture with vertex color
	}

	// Optional: discard fully transparent fragments
	// if (texColor.a < 0.01) discard;

	fragColor = texColor;
}