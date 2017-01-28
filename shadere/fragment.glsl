#version 330
layout(location = 0) out vec4 out_color;

uniform sampler2D textura_color;
uniform sampler2D textura_alpha;
uniform float has_alpha;
in vec2 texcoord;
in vec3 color;

void main( ){
	out_color = vec4(color, 1);
}

