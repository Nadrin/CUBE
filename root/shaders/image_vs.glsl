#version 430

layout(location=0) in vec2 Position;
layout(location=1) in vec2 TexCoord;

out vec2 ScreenPosition;

void main()
{
	ScreenPosition = TexCoord;
	gl_Position    = vec4(Position, 0.0, 1.0);
}
