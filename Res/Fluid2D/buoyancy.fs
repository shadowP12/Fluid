#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D u_velocity;
uniform sampler2D u_temperature;
uniform sampler2D u_density;
uniform float u_ambientTemperature;
uniform float u_timeStep;
uniform float u_sigma;
uniform float u_kappa;

void main()
{
	vec2 v = texture(u_velocity, v_texCoord).xy;
    float t = texture(u_temperature, v_texCoord).x;
	float d = texture(u_density, v_texCoord).x;

	vec2 result = v;

	if(t > u_ambientTemperature) {
        result += (u_timeStep * (t - u_ambientTemperature) * u_sigma - d * u_kappa ) * vec2(0.0, 1.0);
    }
	FragColor = vec4(result, 0.0, 1.0);
}