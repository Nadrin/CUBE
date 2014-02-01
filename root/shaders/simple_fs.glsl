#version 430

in vec3  fsNormal;
in vec2  fsTexCoord;

out vec4 FragColor;

uniform vec3 pcColor;
uniform vec3 pvLightDirection;

layout (binding=0) uniform sampler2D Sampler0;

void main()
{
	vec3 N = normalize(fsNormal);
	float dotNL = max(0.0f, dot(N, -pvLightDirection));

	FragColor = vec4(pcColor * dotNL, 1.0) * texture2D(Sampler0, fsTexCoord);
}
