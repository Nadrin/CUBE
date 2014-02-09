#version 430

in vec2  ScreenPosition;
out vec4 Color;

layout (binding=0) uniform sampler2D Sampler0;

void main()
{
	vec3 tex = texture2D(Sampler0, ScreenPosition).rgb * 2.0;
	Color = vec4(tex, 1.0);
}
