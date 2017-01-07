#pragma once

#ifdef _WIN32
#include "dependente\glew\glew.h"
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#else
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif


namespace lab {
	struct VertexFormat {
		float position_x, position_y, position_z;
		float normal_x, normal_y, normal_z;
		float texcoord_x, texcoord_y;

		VertexFormat(){
			position_x = position_y =position_z=0;
			normal_x = normal_y = normal_z=0;
			texcoord_x = texcoord_y=0;
		}
		VertexFormat(float px, float py, float pz ){
			position_x =px;		position_y =py;		position_z =pz;
			normal_x =normal_y= normal_z =0;
			texcoord_x=	texcoord_y=0;
		}
		VertexFormat(float px, float py, float pz, float nx, float ny, float nz){
			position_x =px;		position_y =py;		position_z =pz;
			normal_x =nx;		normal_y =ny;		normal_z =nz;
			texcoord_x=	texcoord_y=0;
		}
		VertexFormat(float px, float py, float pz, float tx, float ty){
			position_x =px;		position_y =py;		position_z =pz;
			texcoord_x=tx;		texcoord_y=ty;
			normal_x =normal_y= normal_z =0;
		}
		VertexFormat(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty){
			position_x =px;		position_y =py;		position_z =pz;
			normal_x =nx;		normal_y =ny;		normal_z =nz;
			texcoord_x=tx;		texcoord_y=ty;
		}
		VertexFormat operator=(const VertexFormat &rhs){ 
			position_x = rhs.position_x;	position_y = rhs.position_y;	position_z = rhs.position_z;
			normal_x = rhs.normal_x;		normal_y = rhs.normal_y;		normal_z = rhs.normal_z;
			texcoord_x = rhs.texcoord_x;	texcoord_y = rhs.texcoord_y;
			return (*this);
		}

		glm::vec3 get_position() {
			return glm::vec3 (position_x, position_y, position_z);
		}

		glm::vec3 get_normal() {
			return glm::vec3  (normal_x, normal_y, normal_z);
		}

		void flip () {
			normal_x *= -1;
			normal_y *= -1;
			normal_z *= -1;
		}

		VertexFormat interpolate (const VertexFormat &other, float t) {
			float new_position_x = position_x + (other.position_x - position_x) * t;
			float new_position_y = position_y + (other.position_y - position_y) * t;
			float new_position_z = position_z + (other.position_z - position_z) * t;

			float new_normal_x = normal_x + (other.normal_x - normal_x) * t;
			float new_normal_y = normal_y + (other.normal_y - normal_y) * t;
			float new_normal_z = normal_z + (other.normal_z - normal_z) * t;

			float new_texcoord_x = texcoord_x + (other.texcoord_x - texcoord_x) * t;
			float new_texcoord_y = texcoord_y + (other.texcoord_y - texcoord_y) * t;

			return VertexFormat (new_position_x, new_position_z, new_position_z,
							     new_normal_x, new_normal_y, new_normal_z,
							     new_texcoord_x, new_texcoord_y);
		}

	};
}