//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//incarcator de meshe
#include "lab_mesh_loader.hpp"
//geometrie: drawSolidCube, drawWireTeapot...
#include "lab_geometry.hpp"
//incarcator de shadere
#include "lab_shader_loader.hpp"
//interfata cu glut, ne ofera fereastra, input, context opengl
#include "lab_glut.hpp"
//texturi
#include "lab_texture_loader.hpp"
//framebuffer
#include "lab_framebuffer.hpp"

#include "lab_csg.hpp"
//time
#include <ctime>


class Laborator : public lab::glut::WindowListener{


private:
	glm::mat4 view_matrix_normal, view_matrix_reflectiv, projection_matrix;
	unsigned int gl_program_shader;


	Object *object;
	lab::Mesh object_mesh;

	unsigned int screen_width;
	unsigned int screen_height;



public:
	Laborator(){
		glClearColor(0.5,0.5,0.5,1);
		glClearDepth(1);			
		glEnable(GL_DEPTH_TEST);	
		
        gl_program_shader = lab::loadShader("shadere/vertex.glsl", "shadere/fragment.glsl");
		
		lab::Mesh torus_mesh;
		lab::loadObj("resurse/torus.obj", torus_mesh);

		lab::Mesh cube_mesh;
		lab::loadObj("resurse/cube.obj", cube_mesh);

		Object *torus = torus_mesh.to_object();
		Object *cube = torus_mesh.to_object();

		object = cube->substract (torus);
		object_mesh = lab::Mesh (object);

		view_matrix_normal = glm::lookAt (glm::vec3 (0, 50, 100), glm::vec3 (0, 10, 0), glm::vec3 (0, 1, 1));
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	}

	~Laborator(){
		glDeleteProgram(gl_program_shader);
	}
		
	void notifyBeginFrame(){}
    
	void notifyDisplayFrame(){

        glViewport (0, 0, screen_width, screen_height);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram (gl_program_shader);
        glUniformMatrix4fv (glGetUniformLocation(gl_program_shader, "view_matrix"), 1, false, glm::value_ptr(view_matrix_normal));
        glUniformMatrix4fv (glGetUniformLocation(gl_program_shader, "projection_matrix"), 1, false, glm::value_ptr(projection_matrix));
	}

	void notifyEndFrame(){}
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		if (height == 0) height = 1;
		float aspect = (float)width / (float)height;
        screen_width = width;
        screen_height = height;
		projection_matrix = glm::perspective (75.0f, aspect, 0.1f, 10000.0f);
	}


	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();
		if(key_pressed == 32) {
			glDeleteProgram(gl_program_shader);
			gl_program_shader = lab::loadShader("shadere/vertex.glsl", "shadere/fragment.glsl");
		}

	}
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){	}
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
	}
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){}
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ }

};

int main(){
	lab::glut::WindowInfo window(std::string("lab 3 reflectii & framebuffere"), 800, 600, 100, 100, true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,true,true);
	lab::glut::init(window,context, framebuffer);

	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	Laborator mylab;
	lab::glut::setListener(&mylab);

    std::cout << "Taste:" << std::endl << "\tESC ... iesire" << std::endl << "\tSPACE ... reincarca shadere" << "t trece intre reflectie si textura" <<std::endl;

	lab::glut::run();

	return 0;
}
