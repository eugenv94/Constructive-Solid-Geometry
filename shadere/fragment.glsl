#version 330
layout(location = 0) out vec4 out_color;

uniform sampler2D textura_color;
uniform sampler2D textura_alpha;
uniform float has_alpha;

in vec2 texcoord;

void main(){

    vec3 color = texture(textura_color, texcoord).xyz;
	out_color = vec4(color, 1);
}
