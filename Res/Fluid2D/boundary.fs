#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D u_texture;
uniform vec2 u_inverseSize;
void main()
{
    vec4 result = vec4(0,0,0,0);
    if(v_texCoord.x <= 0.001)
        result = vec4(1,1,1,1);
    if(v_texCoord.x >= 1.0-0.001)
        result = vec4(1,1,1,1);
    if(v_texCoord.y <= 0.001)
        result = vec4(1,1,1,1);
    if(v_texCoord.y >= 1.0-0.001)
        result = vec4(1,1,1,1);
    FragColor = result;
}