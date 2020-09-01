#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D u_boundary;
uniform sampler2D u_pressure;
uniform sampler2D u_divergence;
uniform float u_alpha;
uniform float u_inverseBeta;
uniform vec2 u_inverseSize;
void main()
{
    float pN = texture(u_pressure, v_texCoord + vec2(0, u_inverseSize.y)).x;
    float pS = texture(u_pressure, v_texCoord + vec2(0, -u_inverseSize.y)).x;
    float pE = texture(u_pressure, v_texCoord + vec2(u_inverseSize.x, 0)).x;
    float pW = texture(u_pressure, v_texCoord + vec2(-u_inverseSize.x, 0)).x;
    float pC = texture(u_pressure, v_texCoord).x;
    			
    float bN = texture(u_boundary, v_texCoord + vec2(0, u_inverseSize.y)).x;
    float bS = texture(u_boundary, v_texCoord + vec2(0, -u_inverseSize.y)).x;
    float bE = texture(u_boundary, v_texCoord + vec2(u_inverseSize.x, 0)).x;
    float bW = texture(u_boundary, v_texCoord + vec2(-u_inverseSize.x, 0)).x;
    			
    if(bN > 0.0) pN = pC;
    if(bS > 0.0) pS = pC;
    if(bE > 0.0) pE = pC;
    if(bW > 0.0) pW = pC;
    			
    float bC = texture(u_divergence, v_texCoord).x;

    float result = (pW + pE + pS + pN + u_alpha * bC) * u_inverseBeta;

    FragColor = vec4(result, 0.0, 0.0, 1.0);
}