#version 430

in vec3  fsNormal;

out vec4 FragColor;

void main()
{
	vec3 N = normalize(fsNormal);
	float dotNL = max(0.0f, dot(N, -vec3(0.0f, 0.0f, 1.0f)));

	FragColor = vec4(dotNL, dotNL, dotNL, 1.0);
}
