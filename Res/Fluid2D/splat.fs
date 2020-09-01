#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D u_texture;
uniform float u_radius;
uniform vec2 u_point;
uniform vec3 u_value;

void main() {
    float d = distance(v_texCoord, u_point);
    float impulse = 0.0;
    if(d < u_radius) {
        float a = (u_radius - d) * 0.5;
        impulse = min(a, 1.0);
    }

    float source = texture(u_texture, v_texCoord).x;
    float result = max(0.0, mix(source, u_value.x, impulse));
    FragColor = vec4(result);
}