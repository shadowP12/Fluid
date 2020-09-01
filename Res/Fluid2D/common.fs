#version 330 core
in vec2 v_texCoord;
uniform sampler2D u_boundary;
uniform sampler2D u_density;
out vec4 FragColor;
void main()
{
    vec3 col = vec3(1.0) * texture(u_density, v_texCoord).x;
    //float obs = texture(u_boundary, v_texCoord).x;
    //vec3 result = mix(col, vec3(1.0), obs);
    FragColor = vec4(col, 1.0);
}