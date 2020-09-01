#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D u_boundary;
uniform sampler2D u_velocity;
uniform vec2 u_inverseSize;
void main()
{
    vec2 vN = texture(u_velocity, v_texCoord + vec2(0.0, u_inverseSize.y)).xy;
    vec2 vS = texture(u_velocity, v_texCoord + vec2(0.0, -u_inverseSize.y)).xy;
    vec2 vE = texture(u_velocity, v_texCoord + vec2(u_inverseSize.x, 0.0)).xy;
    vec2 vW = texture(u_velocity, v_texCoord + vec2(-u_inverseSize.x, 0.0)).xy;

    float bN = texture(u_boundary, v_texCoord + vec2(0.0, u_inverseSize.y)).x;
    float bS = texture(u_boundary, v_texCoord + vec2(0.0, -u_inverseSize.y)).x;
    float bE = texture(u_boundary, v_texCoord + vec2(u_inverseSize.x, 0.0)).x;
    float bW = texture(u_boundary, v_texCoord + vec2(-u_inverseSize.x, 0.0)).x;

    if(bN > 0.0) vN = vec2(0.0);
    if(bS > 0.0) vS = vec2(0.0);
    if(bE > 0.0) vE = vec2(0.0);
    if(bW > 0.0) vW = vec2(0.0);

    float result = 0.5 * (vE.x - vW.x + vN.y - vS.y);
    FragColor = vec4(result, 0.0, 0.0, 1.0);
}