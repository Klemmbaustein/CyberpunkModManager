#version 330 
in vec2 TexCoords;
in vec2 v_position;
in vec3 v_color;
out vec4 color;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance; Z MinDistance
uniform sampler2D u_texture;
uniform vec3 textColor;
uniform float u_opacity = 1.0f;
uniform vec2 u_screenRes = vec2(1500, 1500);
uniform vec3 transform;
#define NUM_SAMPLES 3

void main()
{   
	if(u_offset.y > v_position.y)
	{
		discard;
	}
	if(u_offset.z < v_position.y)
	{
		discard;
	}
	float sampled = 0;
	vec2 offset = (1.5 / transform.z) / u_screenRes;
	for (int x = -NUM_SAMPLES; x < NUM_SAMPLES; x++)
	{
		for (int y = -NUM_SAMPLES; y < NUM_SAMPLES; y++)
		{
			sampled += texture(u_texture, TexCoords + offset * vec2(x, y)).a;
		}
	}
	sampled /= NUM_SAMPLES * NUM_SAMPLES * 2 * 2;
	color = vec4(v_color, sampled * u_opacity);
}