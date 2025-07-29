layout (location = 0) out vec4 color;

uniform vec4 colour;

in DATA
{
	vec4 position;
	vec2 uv;
	float tid;
	vec4 color;
} fs_in;

uniform sampler2D textures[16];

void main()
{
	vec4 texColor = fs_in.color;

	if (fs_in.tid > 0.0)
	{
		int tid = int(fs_in.tid - 0.5);
		vec4 sampled = texture(textures[tid], fs_in.uv);

		// Apply both texture and vertex color (with proper alpha)
		texColor = fs_in.color * sampled;
	}
	// Optionally discard fully transparent pixels
	
	color = texColor;
	//color = vec4(0.0, 1.0, 0.0, 0.2); // 20% transparent green
}