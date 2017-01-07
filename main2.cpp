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
//time
#include <ctime>


class Laborator : public lab::glut::WindowListener{

//variabile
private:
	glm::mat4 view_matrix_normal, view_matrix_reflectiv, projection_matrix;			    //matrici 4x4 pt modelare vizualizare proiectie
	unsigned int gl_program_shader;					                                    //id-ul de opengl al obiectului de tip program shader

	//mesh
    lab::Mesh bamboo_mesh;
    unsigned int bamboo_texture_color, bamboo_texture_alpha;

    //ground
    lab::Mesh ground_mesh;
    unsigned int ground_texture;

    //quad
    lab::Mesh quad_mesh;
    unsigned int quad_texture;

    //framebuffer
    lab::Framebuffer framebuffer;
    unsigned int screen_width, screen_height;

    bool toggle_quad_texture;
//metode
public:
	
	//constructor .. e apelat cand e instantiata clasa
	Laborator(){
		glClearColor(0.5,0.5,0.5,1);
		glClearDepth(1);			
		glEnable(GL_DEPTH_TEST);	
		
		//incarca un shader din fisiere si gaseste locatiile matricilor relativ la programul creat
        gl_program_shader = lab::loadShader("shadere/vertex.glsl", "shadere/fragment.glsl");
		
		//mesh
		lab::loadObj("resurse/bamboo.obj",bamboo_mesh);	
		bamboo_texture_color = lab::loadTextureBMP("resurse/bamboo.bmp");
        bamboo_texture_alpha = lab::loadTextureBMP("resurse/bambooalpha.bmp");

        //ground
        lab::loadObj("resurse/ground.obj", ground_mesh);
        ground_texture = lab::loadTextureBMP("resurse/ground.bmp");

        //quad reflectiv
        lab::createQuad(lab::VertexFormat(-70, -20, -60, 0, 0), lab::VertexFormat(70, -20, -60, 1, 0), lab::VertexFormat(70, 30, -60, 1, 1), lab::VertexFormat(-70, 30, -60, 0, 1), quad_mesh);
        quad_texture = lab::loadTextureBMP("resurse/reflectie.bmp");

		//matrici de modelare si vizualizare
		view_matrix_normal = glm::lookAt(glm::vec3(0,70,250), glm::vec3(0,10,0), glm::vec3(0,-1,10));
        view_matrix_reflectiv = glm::lookAt(glm::vec3(100, 20, -60), glm::vec3(0, 10, 0), glm::vec3(0, 1, 10));

		//desenare wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//destructor .. e apelat cand e distrusa clasa
	~Laborator(){
		//distruge shader
		glDeleteProgram(gl_program_shader);

		//distruge texturi, meshele sunt distruse in destructorul de la Mesh
        glDeleteTextures(1, &bamboo_texture_alpha);	glDeleteTextures(1, &bamboo_texture_color);  glDeleteTextures(1, &ground_texture); glDeleteTextures(1, &quad_texture);
	}
		


	//--------------------------------------------------------------------------------------------
	//functii de cadru ---------------------------------------------------------------------------

	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){}
	//functia de afisare (lucram cu banda grafica)
    void DrawScene(unsigned int shader){
        static float bamboo_angle = 0;
        bamboo_angle += 0.1f;

        //bamboos
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, bamboo_texture_color);
        glUniform1i(glGetUniformLocation(shader, "textura_color"), 1);
        glActiveTexture(GL_TEXTURE0 + 6);
        glBindTexture(GL_TEXTURE_2D, bamboo_texture_alpha);
        glUniform1i(glGetUniformLocation(shader, "textura_alpha"), 6);
        glUniform1f(glGetUniformLocation(shader, "has_alpha"), 1);
        bamboo_mesh.Bind();
        for (int i = 0; i < 16; i++) {
            glm::mat4 model_matrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-40 + (i % 4) * 25, -20, -20 + (i / 4) * 25)), (bamboo_angle + i)*((i%2)*2-1), glm::vec3(0, 1, 0));
            glUniformMatrix4fv(glGetUniformLocation(shader, "model_matrix"), 1, false, glm::value_ptr(model_matrix));
            bamboo_mesh.Draw();
        }

        //ground
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, ground_texture);
        glUniform1i(glGetUniformLocation(shader, "textura_color"), 1);
        glUniform1f(glGetUniformLocation(shader, "has_alpha"), 0);
        glm::mat4 model_matrix = glm::translate( glm::mat4(1), glm::vec3(0,-20,0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model_matrix"), 1, false, glm::value_ptr(model_matrix));
        ground_mesh.Bind();
        ground_mesh.Draw();

    }
	void notifyDisplayFrame(){
		
        
        //pass 1: ce vede oglinda -> framebuffer 
        //TODO
		
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		framebuffer.bind();

        //pass2: scena + quad reflectiv -> ecran
        glViewport(0, 0, framebuffer.GetWidth() - 50, framebuffer.GetHeight() - 50);//screen_width, screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(gl_program_shader);
        glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "view_matrix"), 1, false, glm::value_ptr(view_matrix_normal));
        glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "projection_matrix"), 1, false, glm::value_ptr(projection_matrix));
        DrawScene(gl_program_shader);

		framebuffer.unbind();



        glUseProgram(gl_program_shader);

        glViewport(0, 0, screen_width, screen_height);
        glUseProgram(gl_program_shader);
        glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "view_matrix"), 1, false, glm::value_ptr(view_matrix_normal));
        glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "projection_matrix"), 1, false, glm::value_ptr(projection_matrix));
        DrawScene(gl_program_shader);

        glActiveTexture(GL_TEXTURE0 + 1);
        //if (toggle_quad_texture){
        //    //TODO
        //    glBindTexture(GL_TEXTURE_2D, quad_texture);
        //}
        //else glBindTexture(GL_TEXTURE_2D, quad_texture);
		glBindTexture(GL_TEXTURE_2D, framebuffer.getColorTexture());
        glViewport(0, 0, screen_width, screen_height);

        glUniform1i(glGetUniformLocation(gl_program_shader, "textura_color"), 1);
        glUniform1f(glGetUniformLocation(gl_program_shader, "has_alpha"), 0);
        glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "model_matrix"), 1, false, glm::value_ptr(glm::mat4(1)));
        quad_mesh.Bind();
        quad_mesh.Draw();

	}
	//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functei care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//reshape
		if(height==0) height=1;
		float aspect = (float)width / (float)height;
        screen_width = width;
        screen_height = height;
		projection_matrix = glm::perspective(75.0f, aspect,0.1f, 10000.0f);
	}


	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if(key_pressed == 32) {
			//SPACE reincarca shaderul si recalculeaza locatiile (offseti/pointeri)
			glDeleteProgram(gl_program_shader);
			gl_program_shader = lab::loadShader("shadere/vertex.glsl", "shadere/fragment.glsl");
		}
        if (key_pressed == 't') toggle_quad_texture = !toggle_quad_texture;

	}
	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
	}
	//tasta speciala ridicata
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){}
	//drag cu mouse-ul
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	//am miscat mouseul (fara sa apas vreun buton)
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	//am apasat pe un boton
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
	//scroll cu mouse-ul
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ }

};

int main(){
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("lab 3 reflectii & framebuffere"),800,600,100,100,true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,true,true);
	lab::glut::init(window,context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//DUPA GLEW!!! ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Laborator mylab;
	lab::glut::setListener(&mylab);

	//taste
    std::cout << "Taste:" << std::endl << "\tESC ... iesire" << std::endl << "\tSPACE ... reincarca shadere" << "t trece intre reflectie si textura" <<std::endl;

	//run
	lab::glut::run();

	return 0;
}
