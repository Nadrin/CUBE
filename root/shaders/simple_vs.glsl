#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 CameraMatrix;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;

out vec3 fsNormal;

void main()
{
	fsNormal    = vec3(NormalMatrix * vec4(normal, 1.0));
	gl_Position = CameraMatrix * ModelMatrix * vec4(position, 1.0);
}