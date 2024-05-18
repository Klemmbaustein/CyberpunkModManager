#version 330

in vec2 v_texcoords;
in vec2 v_position;
out vec4 f_color;

uniform vec4 u_color = vec4(1);
uniform vec4 u_borderColor = vec4(1);
uniform int u_usetexture;
uniform sampler2D u_texture;
uniform vec3 u_offset; // Scroll bar: X = scrolled distance; Y = MaxDistance; Z MinDistance
uniform float u_opacity = 1;
uniform int u_borderType = 1;
uniform float u_borderScale = 0.1;
uniform vec4 u_transform = vec4(vec2(0), vec2(1));
uniform float u_aspectratio = 16.0/9.0;
uniform vec2 u_screenRes = vec2(1600, 900);

#define NUM_SAMPLES 2

void main()
{
	vec2 scale = u_transform.zw * vec2(u_aspectratio, 1);
	vec2 scaledTexCoords = v_texcoords * scale;
	vec2 centeredTexCoords = abs((scaledTexCoords - scale / 2) * 2);
	vec2 borderTexCoords = vec2(0);
	vec4 drawnColor = u_color;
	bool IsInEdge = false;
	float opacity = u_opacity;
	if(centeredTexCoords.x >= scale.x - u_borderScale)
	{
		IsInEdge = true;
		if(u_borderType == 2) drawnColor = u_borderColor;
	}
	if(centeredTexCoords.y >= scale.y - u_borderScale)
	{
		if(u_borderType == 2) drawnColor = u_borderColor;
		if (IsInEdge && u_borderType == 1)
		{
			opacity *= clamp(1.0 / pow((length((scale - u_borderScale) - centeredTexCoords) / u_borderScale), 1000 * u_borderScale), 0, 1);
		}
	}
	if(u_offset.y > v_position.y)
	{
		discard;
	}
	if(u_offset.z < v_position.y)
	{
		discard;
	}
	if (u_usetexture == 1)
	{
		vec4 sampled = vec4(0);
		vec2 offset = (0.5 / scale) / u_screenRes;
		int samples = 0;
		for (int x = -NUM_SAMPLES; x < NUM_SAMPLES; x++)
		{
			for (int y = -NUM_SAMPLES; y < NUM_SAMPLES; y++)
			{
				vec4 newS = texture(u_texture, v_texcoords + offset * vec2(x, y));
				if (newS.a > 0)
				{
					sampled.xyz += newS.xyz;
					samples++;
				}
				sampled.w += newS.w;
			}
		}
		sampled.xyz /= samples;
		sampled.w /= NUM_SAMPLES * NUM_SAMPLES * 2 * 2;
		opacity *= sampled.w;
		f_color = vec4(clamp(drawnColor.xyz * sampled.xyz, vec3(0), vec3(1)), opacity);
	}
	else
	{
		f_color = vec4(drawnColor.xyz, opacity);
	}
}