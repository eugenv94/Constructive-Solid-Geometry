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

//camera
#include "lab_camera.hpp"

//time
#include <ctime>
#include <stdlib.h>


class Laborator : public lab::glut::WindowListener{

// States for the application:
// 1) CHOOSING_PRIMITIVE: In this state we can select the primitive: cube,
// sphere, cylinder, torus.
// 2) TRANSFORMING: Here we can modify the primitive selected before by applying
// some scaling/rotating or translating operations.
// 3) CHOOSING_OPERATION: After selecting two primitives we also have to select
// the operation that we want to perform (add, subtract or intersect).
// 4) FINISHED: After the operation is complete, we have the posibility to start
// a new one (maybe continue from the previous object) or to exit the program.
enum State {
	CHOOSING_PRIMITIVE,
	TRANSFORMING,
	CHOOSING_OPERATION,
	FINISHED
};

private:
	glm::mat4 view_matrix_normal, view_matrix_reflectiv, projection_matrix;
	unsigned int gl_program_shader;

	lab::Camera camera;

	// Objects
	Object *object;
	lab::Mesh object_mesh;

	std::vector<lab::Mesh> primitives;
	glm::mat4 primitive_model_matrix;

	std::vector<int> operands;
	std::vector<glm::mat4> operand_model_matrices;

	State state;
	int primitive_choice;
	int current_operand;

	// Textures
	unsigned int screen_width;
	unsigned int screen_height;
	unsigned int ground_texture;
	int key_state[256];

public:
	Laborator(){

		glClearColor(0.5, 0.5, 0.5, 1);
		glClearDepth(1);
		glEnable(GL_DEPTH_TEST);

		gl_program_shader = lab::loadShader("shadere/vertex.glsl", "shadere/fragment.glsl");
		ground_texture = lab::loadTextureBMP("resurse/ground.bmp");

		state = CHOOSING_PRIMITIVE;
		primitive_choice = 0;	

		for (int i = 0; i < 4; i++) {
			primitives.push_back(lab::Mesh());
		}

		primitive_model_matrix = glm::mat4 (1);

		current_operand = 0;
		operands.push_back (0);
		operands.push_back (1);
		operand_model_matrices.push_back (glm::mat4(0));
		operand_model_matrices.push_back (glm::mat4(1));

		// Load primitives.
		lab::loadObj("resurse/sphere.obj", primitives[0]);
		lab::loadObj("resurse/cube.obj", primitives[1]);
		lab::loadObj("resurse/torus.obj", primitives[2]);
		lab::loadObj("resurse/cylinder.obj", primitives[3]);

		camera.set(glm::vec3(0, 5, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 1));
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	~Laborator(){
		glDeleteProgram(gl_program_shader);
	}

	void notifyBeginFrame(){
	}

	void notifyDisplayFrame(){

		treatInput();
		glViewport(0, 0, screen_width, screen_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(gl_program_shader);
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "view_matrix"), 1, false, glm::value_ptr(camera.getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "projection_matrix"), 1, false, glm::value_ptr(projection_matrix));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "model_matrix"), 1, false, glm::value_ptr(primitive_model_matrix));

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, ground_texture);
		glUniform1i(glGetUniformLocation(gl_program_shader, "textura_color"), 1);

		if (state == CHOOSING_PRIMITIVE || state == TRANSFORMING) {
			primitives[primitive_choice].Bind();
			primitives[primitive_choice].Draw();
		}

		if (state == TRANSFORMING && current_operand == 1) {
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "model_matrix"), 1, false, glm::value_ptr(operand_model_matrices[0]));
			primitives[operands[0]].Bind();
			primitives[operands[0]].Draw();
		}
		if (state == FINISHED) {
			primitives[primitives.size() - 1].Bind();
			primitives[primitives.size() - 1].Draw();
		}
	}

	// After frame render
	void notifyEndFrame(){}


	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//reshape
		if (height == 0) height = 1;
		float aspect = (float)width / (float)height;
		screen_width = width;
		screen_height = height;
		projection_matrix = glm::perspective(75.0f, aspect, 0.1f, 10000.0f);
	}

	void treatInput() {
		if (key_state['w']) camera.translateForward(0.3f);
		if (key_state['a']) camera.translateRight(0.3f);
		if (key_state['s']) camera.translateForward(-0.3f);
		if (key_state['d']) camera.translateRight(-0.3f);
		if (key_state['q']) camera.rotateFPSoY(-0.01f);
		if (key_state['e']) camera.rotateFPSoY(0.01f);
		if (key_state['r']) camera.translateUpword(0.1f);
		if (key_state['f']) camera.translateUpword(-0.1f);
	}

	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y) {
		key_state[key_released] = 0;
	}

	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if (key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if (key_pressed == 32) {
			glDeleteProgram(gl_program_shader);
			gl_program_shader = lab::loadShader("shadere/vertex.glsl", "shadere/fragment.glsl");
		}
		key_state[key_pressed] = 1;


			// CHOOSING_PRIMITIVE STATE
		if (state == CHOOSING_PRIMITIVE && key_pressed >= '0' && key_pressed <= '9') {
			int new_choice = key_pressed - '0';

			if (new_choice > primitives.size()) {
				std::cout << "No primitive assigned to button #" << new_choice <<std::endl;
				std::cout << "Please select another!\n";
			}
			else {
				// Start counting primitives from 1.
				primitive_choice = key_pressed - '0' - 1;
			}
			return;

		}
		if (state == CHOOSING_PRIMITIVE && key_pressed == 32) {
			std::cout << "Now transform it (arrows, +/-, </>) and press Enter when finished!\n";
			state = TRANSFORMING;
			return;
		}

		// TRANSFORMING STATE (look in notifySpecialKeyPressed for extra buttons)
		if(state == TRANSFORMING && key_pressed == 'i') {
			primitive_model_matrix = glm::translate (primitive_model_matrix, glm::vec3 (0, -0.1, 0));
		}
		if(state == TRANSFORMING && key_pressed == 'k') {
			primitive_model_matrix = glm::translate (primitive_model_matrix, glm::vec3 (0, 0.1, 0));
		}
		if (state == TRANSFORMING && key_pressed == 'n') {
			primitive_model_matrix = glm::rotate (primitive_model_matrix, float(M_PI / 12.0), glm::vec3(0, 0, 1));
		}
		if (state == TRANSFORMING && key_pressed == 'm') {
			primitive_model_matrix = glm::rotate (primitive_model_matrix, float(M_PI / 12.0), glm::vec3(0, 0, -1));
		}
		if (state == TRANSFORMING && key_pressed == 'b') {
			primitive_model_matrix = glm::rotate (primitive_model_matrix, float(M_PI / 12.0), glm::vec3(0, 1, 0));
		}
		if (state == TRANSFORMING && key_pressed == 'v') {
			primitive_model_matrix = glm::rotate (primitive_model_matrix, float(M_PI / 12.0), glm::vec3(0, -1, 0));
		}
		if (state == TRANSFORMING && key_pressed == 'c') {
			primitive_model_matrix = glm::rotate (primitive_model_matrix, float(M_PI / 12.0), glm::vec3(1, 0, 0));
		}
		if (state == TRANSFORMING && key_pressed == 'x') {
			primitive_model_matrix = glm::rotate (primitive_model_matrix, float(M_PI / 12.0), glm::vec3(-1, 0, 0));
		}

		if(state == TRANSFORMING && key_pressed == '=') {
			primitive_model_matrix = glm::scale (primitive_model_matrix, glm::vec3 (1.1, 1.1, 1.1));
		}
		if(state == TRANSFORMING && key_pressed == '-') {
			primitive_model_matrix = glm::scale (primitive_model_matrix, glm::vec3 (0.9, 0.9, 0.9));
		}
		if(state == TRANSFORMING && key_pressed == 32) {
			std::cout << "Primitive saved!\n";

			operands[current_operand] = primitive_choice;
			operand_model_matrices[current_operand] = primitive_model_matrix;

			primitive_choice = 0;
			primitive_model_matrix = glm::mat4(1);

			if (current_operand == 0) {
				std::cout << "Choose a primitive for the second operand!\n";
				current_operand++;
				state = CHOOSING_PRIMITIVE;
			}
			else {
				current_operand = 0;
				state = CHOOSING_OPERATION;
				std::cout <<"Please select an operation! (a(add), s(subtract), i(intersect))\n";
			}
			return;
		}

		// CHOOSING_OPERATION
		if (state == CHOOSING_OPERATION && key_pressed == 'a') {
			std::cout << "Adding operands...\n";
			Object *operand1 = primitives[operands[0]].to_object(operand_model_matrices[0]);
			Object *operand2 = primitives[operands[1]].to_object(operand_model_matrices[1]);
			Object *result = operand1->combine(operand2);

			primitives.push_back (lab::Mesh());
			lab::loadBSB (result, primitives[primitives.size() - 1]);
			std::cout << "Result saved slot #" << primitives.size() << std::endl;
			std::cout << "Press enter for another operation or ESC to exit\n";
			state = FINISHED;

			lab::loadObj("resurse/sphere.obj", primitives[0]);
			lab::loadObj("resurse/cube.obj", primitives[1]);
			lab::loadObj("resurse/torus.obj", primitives[2]);
			lab::loadObj("resurse/cylinder.obj", primitives[3]);

			return;
		}
		if (state == CHOOSING_OPERATION && key_pressed == 's') {
			std::cout << "Subtracting operands...\n";
			Object *operand1 = primitives[operands[0]].to_object(operand_model_matrices[0]);
			Object *operand2 = primitives[operands[1]].to_object(operand_model_matrices[1]);
			Object *result = operand1->subtract (operand2);

			primitives.push_back (lab::Mesh());
			lab::loadBSB (result, primitives[primitives.size() - 1]);
			std::cout << "Result saved slot #" << primitives.size() << std::endl;
			std::cout << "Press enter for another operation or ESC to exit\n";
			state = FINISHED;


			lab::loadObj("resurse/sphere.obj", primitives[0]);
			lab::loadObj("resurse/cube.obj", primitives[1]);
			lab::loadObj("resurse/torus.obj", primitives[2]);
			lab::loadObj("resurse/cylinder.obj", primitives[3]);
			return;
		}
		if (state == CHOOSING_OPERATION && key_pressed == 'i') {
			std::cout << "Intersecting operands...\n";
			Object *operand1 = primitives[operands[0]].to_object(operand_model_matrices[0]);
			Object *operand2 = primitives[operands[1]].to_object(operand_model_matrices[1]);
			Object *result = operand1->intersect(operand2);

			primitives.push_back (lab::Mesh());
			lab::loadBSB (result, primitives[primitives.size() - 1]);
			std::cout << "Result saved slot #" << primitives.size() <<std::endl;
			std::cout << "Press enter for another operation or ESC to exit\n";
			state = FINISHED;

			lab::loadObj("resurse/sphere.obj", primitives[0]);
			lab::loadObj("resurse/cube.obj", primitives[1]);
			lab::loadObj("resurse/torus.obj", primitives[2]);
			lab::loadObj("resurse/cylinder.obj", primitives[3]);

			return;
		}

		// FINISHED
		if (state == FINISHED && key_pressed == 32) {
			std::cout << "Choose a primitive for the first operand!\n";
			state = CHOOSING_PRIMITIVE;
			return;
		}

	}

	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(state == TRANSFORMING && key_pressed == GLUT_KEY_UP) {
			primitive_model_matrix = glm::translate (primitive_model_matrix, glm::vec3 (0, 0, -0.1));
		}
		if(state == TRANSFORMING && key_pressed == GLUT_KEY_DOWN) {
			primitive_model_matrix = glm::translate (primitive_model_matrix, glm::vec3 (0, 0, 0.1));
		}
		if(state == TRANSFORMING && key_pressed == GLUT_KEY_LEFT) {
			primitive_model_matrix = glm::translate (primitive_model_matrix, glm::vec3 (0.1, 0, 0));
		}
		if(state == TRANSFORMING && key_pressed == GLUT_KEY_RIGHT) {
			primitive_model_matrix = glm::translate (primitive_model_matrix, glm::vec3 (-0.1, 0, 0));
		}
	}
};
int main(){
	srand(time(NULL));

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

