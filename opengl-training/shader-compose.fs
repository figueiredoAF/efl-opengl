#version 330 core
out vec4 FragColor;

in vec2 TextCoords;

uniform sampler2D composeTexture;

void main()
{
    vec3 col = texture(composeTexture, TextCoords).rgb;
    FragColor = vec4(col, 1.0);
}