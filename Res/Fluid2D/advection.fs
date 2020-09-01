#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D u_boundary;
uniform sampler2D u_source;
uniform sampler2D u_velocity;
uniform float u_timeStep;
uniform float u_dissipation;
uniform vec2 u_inverseSize;
void main()
{
    vec2 u = texture(u_velocity, v_texCoord).xy;

    vec2 coord = v_texCoord - (u * u_inverseSize * u_timeStep);

	vec4 result = u_dissipation * texture(u_source, coord);

    float solid = texture(u_boundary, v_texCoord).x;

    if(solid > 0.0)
        result = vec4(0,0,0,0);

    FragColor = result;
}