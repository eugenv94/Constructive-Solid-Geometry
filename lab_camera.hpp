//-------------------------------------------------------------------------------------------------
// Autor: Lucian Petrescu
// Data: 14 oct 2013
//-------------------------------------------------------------------------------------------------


#pragma once

#ifdef _WIN32
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#else
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

#include <vector>

namespace lab{

	struct CamVertexFormat{ 
		glm::vec3 position, color; 
		CamVertexFormat(const glm::vec3 &p, const glm::vec3 &c){ 
			position=p; color=c;
		} 
	};


	class Camera{
	public:
		Camera(){
			//initializeaza camera
			position = glm::vec3(0,0,50);
			forward = glm::vec3(0,0,-1);
			up = glm::vec3(0,1,0);
			right = glm::vec3(1,0,0);

			//creaza geometry si update
			createGeometry();
			updateGeometry();
		}
		Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up){
			//set camera
			set(position, center,up);

			//update geometrie si update
			createGeometry();
			updateGeometry();
		}
		~Camera(){
			//distruge geometrie
			destroyGeometry();
		}

		void set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up){
			//update camera
			this->position = position;
			forward = glm::normalize(center-position);
			right = glm::cross(forward, up);
			this->up=glm::cross(right,forward);

			//update geometrie
			updateGeometry();
		}

		void translateForward(float distance){
			//translatie
			position = position + glm::normalize(forward)*distance;

			//update geometrie
			updateGeometry();
		}
		void translateUpword(float distance){
			//translatie
			position = position + glm::normalize(up)*distance;

			//update geometrie
			updateGeometry();
		}
		void translateRight(float distance){
			//translatie
			position = position + glm::normalize(right)*distance;

			//update geometrie
			updateGeometry();
		}
		void rotateFPSoX(float angle){
			//rotatie
			forward = glm::normalize( glm::vec3( (glm::rotate(glm::mat4(1.0f),angle, right)*glm::vec4(forward,1)) ));
			up = glm::normalize(glm::cross(right,forward));

			//update geometrie
			updateGeometry();
		}
		void rotateFPSoY(float angle){
			//rotatie
			forward = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f),angle, up)*glm::vec4(forward,1)) ));
			right = glm::normalize(glm::vec3( (glm::rotate(glm::mat4(1.0f),angle, up)*glm::vec4(right,1)) ));


			//update geometrie
			updateGeometry();
		}
		void rotateFPSoZ(float angle){
			//rotatie
			right = glm::normalize(glm::vec3( (glm::rotate(glm::mat4(1.0f),angle, forward)*glm::vec4(right,1)) ));
			up = glm::normalize(glm::cross(right, forward));

			//update geometrie
			updateGeometry();
		}
		void rotateTPSoX(float angle, float distance){
			//rotatie
			translateForward(distance);
			rotateFPSoX(angle);
			translateForward(-distance);

			//update geometrie
			updateGeometry();
		}
		void rotateTPSoY(float angle, float distance){
			//rotatie
			translateForward(distance);
			rotateFPSoY(angle);
			translateForward(-distance);

			//update geometrie
			updateGeometry();
		}
		void rotateTPSoZ(float angle, float distance){
			//rotatie
			translateForward(distance);
			rotateFPSoZ(angle);
			translateForward(-distance);

			//update geometrie
			updateGeometry();
		}

		glm::mat4 getViewMatrix(){
			return glm::lookAt(position,position + glm::normalize(forward), up);
		}

        glm::vec3 getPosition(){ return position; }

	private:
		void createGeometry(){
			glGenVertexArrays(1,&vao);
			glBindVertexArray(vao);
			glGenBuffers(1,&vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glGenBuffers(1,&ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			std::vector<unsigned int> indices; for(unsigned int i=0;i<6;i++) indices.push_back(i);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(CamVertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(CamVertexFormat),(void*)(sizeof(float)*3));	
		}
		void destroyGeometry(){
			glDeleteVertexArrays(1,&vao);
			glDeleteBuffers(1,&vbo);
			glDeleteBuffers(1,&ibo);
		}

		void updateGeometry(){
			glBindVertexArray(vao);
			float axis_lenght=8;
			std::vector<CamVertexFormat> vertices;
			vertices.push_back( CamVertexFormat( position, glm::vec3(0,0,1)));
			vertices.push_back( CamVertexFormat( position + glm::normalize(forward)*axis_lenght, glm::vec3(0,0,1)));
			vertices.push_back( CamVertexFormat( position, glm::vec3(1,0,0)));
			vertices.push_back( CamVertexFormat( position + glm::normalize(right)*axis_lenght, glm::vec3(1,0,0)));
			vertices.push_back( CamVertexFormat( position, glm::vec3(0,1,0)));
			vertices.push_back( CamVertexFormat( position + glm::normalize(up)*axis_lenght, glm::vec3(0,1,0)));
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 6*sizeof(CamVertexFormat),&vertices[0],GL_STATIC_DRAW);
		}
	public:
		void drawGeometry(){
			glBindVertexArray(vao);
			glDrawElements(GL_LINES, 6,GL_UNSIGNED_INT, (void*)0);
		}

	private:
		//camera
		glm::vec3 position;
		glm::vec3 up;
		glm::vec3 forward;
		glm::vec3 right;

		//geometrie camera
		unsigned int vao,vbo,ibo;
	};
}
