#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 2) in vec2 in_texcoord;

uniform mat4 model_matrix, view_matrix, projection_matrix;

out vec2 texcoord;
out vec3 color;

void main() {
	texcoord = in_texcoord.xy;
	color = vec3(abs(  sin( cos(view_matrix[0][0] +model_matrix[1][2])*in_position[2] + projection_matrix[0][0] ) ), abs(  sin( cos(view_matrix[1][2] +model_matrix[1][1])+in_position[0]*projection_matrix[0][0] )   ), abs(   sin( cos(view_matrix[3][2]+model_matrix[1][3]) +in_position[1]*view_matrix[0][0] )   ) );
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(in_position,1); 
}

