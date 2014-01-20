#version 430

uniform vec4 test;
uniform vec4 pc_test;

out vec4 outputColor;

void main()
{
	outputColor = test + pc_test;
}