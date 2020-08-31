#version 330 core
in vec2 v_texCoord;
uniform sampler2D u_texture;
out vec4 FragColor;
void main()
{
    vec4 src = texture(u_texture, v_texCoord);
    FragColor = src + vec4(0.001);
}