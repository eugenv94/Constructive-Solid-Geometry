#version 330
layout(location = 0) out vec4 out_color;

uniform sampler2D textura_color;
uniform sampler2D textura_alpha;
uniform float has_alpha;

in vec2 texcoord;

void main(){

    if (has_alpha > 0.5){
        vec3 opacity = texture(textura_alpha, texcoord).xyz;
        if (opacity.r<0.1 && opacity.g<0.1 && opacity.b<0.1) discard;
    }
    vec3 color = texture(textura_color, texcoord).xyz;

	out_color = vec4(color, 1);
}