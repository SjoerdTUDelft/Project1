#include <GL/glew.h>
#include <gl/freeglut.h>

#include <iostream>
#include <chrono>
#include <errno.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Window.hpp>

#include "logging.h"
#include <soil\SOIL.h>
#include "ShaderLoader.h"

#define M_PI atan(1)*4
using namespace std;
 
void display(void);
void centerOnScreen();
 

//  define the window position on screen
int window_x;
int window_y;
int window_width = 580;
int window_height = 580;
sf::Window * windowref;
char *window_title = "Sample OpenGL FreeGlut App";
char * GL_LOG_FILE = "apple.txt";

LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double elapsedTime;
 
GLuint vao;
GLuint vbo;
GLuint ebo;

GLuint shader_program;
GLuint texture;
GLuint texture2;

glm::vec3 position(0.0f,0.0f,0.0f);
glm::vec3 direction(0.0f, 0.0f, 0.0f);
glm::vec3 camRight(0.0f, 0.0f, 0.0f);
glm::vec2 mousePos( -1.0f, -1.0f);
const float sensitivity = 0.08;
const float limit = 0.6* M_PI / 2.0f;
float deltaTime = 0;
float camPitch = 0;
float camYaw = 0;
const float speed = 10.0;
const float extraFov = 50;
const float minFov = 35;
float fovScalar = 1;
float smoothFovScalar = 1;
chrono::time_point<chrono::steady_clock> t_start_app;
chrono::time_point<chrono::steady_clock> t_start;
chrono::time_point<chrono::steady_clock> t_now;

bool msaa = false;

 
void processKeys() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
		cout << "ESCAPE WAS PRESSED" << endl;
		exit(0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		position -= camRight * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		position += camRight * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		position += direction * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		position -= direction * deltaTime * speed;
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		fovScalar -= deltaTime * 2;
		if (fovScalar < 0) fovScalar = 0;
		smoothFovScalar = glm::smoothstep(0.0f, 1.0f, fovScalar);
	}
	else {
		fovScalar += deltaTime * 2;
		if (fovScalar > 1) fovScalar = 1;
		smoothFovScalar = glm::smoothstep(0.0f, 1.0f, fovScalar);
	}
 
};
void centerOnScreen()
{
	window_x = (glutGet(GLUT_SCREEN_WIDTH) - window_width) / 2;
	window_y = (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2;
}
void _update_fps_counter() {
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) *1000.0 / frequency.QuadPart;
	QueryPerformanceCounter(&t1);

	static int frame_count;
	static double elapsed_seconds;
	elapsed_seconds += elapsedTime;

	if (elapsed_seconds > 150.0) {
		double fps = (double)frame_count / (elapsed_seconds / 1000.0);
		char tmp[32];
		sprintf(tmp, "opengl @ fps: %3.2f", fps);
		windowref->setTitle(tmp);

		elapsed_seconds = 0;
		frame_count = 0;
	}
	frame_count++;
}


//-------------------------------------------------------------------------
//  Program Main method.
//-------------------------------------------------------------------------
void main(int argc, char **argv)
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 16;

	sf::Window window(sf::VideoMode(window_width, window_height, 32), "OpenGL", sf::Style::Default, settings);
	windowref = &window;
	windowref->setVerticalSyncEnabled(true);
	windowref->setMouseCursorGrabbed(true);
	windowref->setKeyRepeatEnabled(false);
	windowref->setMouseCursorVisible(false);

	restart_gl_log();


	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	//glFrontFace(GL_CW); // GL_CCW for counter clock-wise
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glewExperimental = GL_TRUE;
	glewInit();


	GLfloat points[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 2,   // First Triangle
		1, 2, 3    // Second Triangle
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	
	glGenBuffers(1, &vbo);

	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	loadShader(shader_program, "./shaders/testhader.vert", "./shaders/testhader.frag");

	int width, height;
	unsigned char* imageBird = SOIL_load_image("images/bird.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if (imageBird == NULL) {
		cout << "Failed loading image at line: " << __LINE__ << endl;
		return;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageBird);
	SOIL_free_image_data(imageBird);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned char* imageCrow = SOIL_load_image("images/crow.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if (imageCrow == NULL) {
		cout << "Failed loading image at line: " << __LINE__ << endl;
		return;
	}

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageCrow);
	SOIL_free_image_data(imageCrow);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(shader_program);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader_program, "ourTexture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(shader_program, "ourTexture2"), 1);

	t_start_app = chrono::high_resolution_clock::now();

	bool running = true;
	int frameCount = 0;
	while (running) {
		sf::Event windowEvent;

		while (window.pollEvent(windowEvent))
		{
			switch (windowEvent.type)
			{
			case sf::Event::Closed:
				running = false;
				break;
			case sf::Event::Resized:
				glViewport(0, 0, windowEvent.size.width, windowEvent.size.height);
				break;
			case sf::Event::MouseButtonPressed:
				switch (windowEvent.mouseButton.button) {
				case sf::Mouse::Left:
					msaa = !msaa;
					cout << "MSAA is " << (msaa ? "True" : "False") << endl;
					break;
				case sf::Mouse::Middle:
					static GLint CurrentMode;
					glGetIntegerv(GL_POLYGON_MODE, &CurrentMode);

					switch (CurrentMode) {
					case(GL_POINT): glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); cout << "LINE" << endl; break;
					case(GL_LINE): glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); cout << "FILL" << endl; break;
					case(GL_FILL): glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); cout << "POINT" << endl; break;
					}
				}
				break;
			case sf::Event::MouseMoved:
				if (windowref->hasFocus()) {


					glm::vec2 currentPos(windowEvent.mouseMove.x, windowEvent.mouseMove.y);
					//cout << currentPos.x << " AND " << currentPos.y << endl;
					//cout << mousePos.x << " AND " << mousePos.y << endl;
					//cout << frameCount << endl;
					glm::vec2 delta = mousePos - currentPos;

					camPitch -= delta.y*sensitivity * deltaTime;
					if (camPitch > limit) camPitch = limit;
					if (camPitch < -limit) camPitch = -limit;
					camYaw += delta.x * sensitivity * deltaTime;



					//cout << windowEvent.mouseMove.x  << " AND " << windowEvent.mouseMove.y << " AND "<< frameCount << endl;
				}
				break;
			}
		}

		if (windowref->hasFocus()) {
			sf::Vector2u windowSize = windowref->getSize();
			windowSize.x /= 2;
			windowSize.y /= 2;
			sf::Mouse::setPosition(sf::Vector2i(windowSize.x, windowSize.y), *windowref);
			mousePos.x = sf::Mouse::getPosition(*windowref).x;
			mousePos.y = sf::Mouse::getPosition(*windowref).y;
		}

		processKeys();

		t_now = chrono::high_resolution_clock::now();
		deltaTime = chrono::duration<float>(t_now - t_start).count();
		if (deltaTime > 1.0)deltaTime = 1.0;
		t_start = chrono::high_resolution_clock::now();

		_update_fps_counter();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (msaa) {
			glEnable(GL_MULTISAMPLE_ARB);
		} else {
			glDisable(GL_MULTISAMPLE_ARB);
		}


		auto now = chrono::high_resolution_clock::now();
		double time = chrono::duration<double>(now - t_start_app).count();

		GLuint uniColor = glGetUniformLocation(shader_program, "triangleColor");
		glUniform1f(uniColor, (sin(time * 4.0f) / 2.0 + 0.5));
		GLenum err = glGetError();
		if (err != 0) {
			cout << err << endl;
		}

		direction = glm::vec3(
			cos(camPitch) * sin(camYaw),
			cos(camPitch) * cos(camYaw),
			sin(camPitch)
		);
		camRight = glm::vec3(
			sin(camYaw - 3.14f / 2.0f),
			cos(camYaw - 3.14f / 2.0f),
			0.0f

		);
		glm::vec3 up = glm::cross(camRight, direction);
		glm::mat4 view = glm::lookAt(
			//sin(time) * 2.0f),
			glm::vec3(position),
			glm::vec3(position + direction),
			glm::vec3(up)
		);

		GLint uniView = glGetUniformLocation(shader_program, "view");
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(vao);

		glm::mat4 proj = glm::perspective(glm::radians(minFov + extraFov*smoothFovScalar), (float)window_width / (float)window_height, 1.0f, 30.0f);
		GLint uniProj = glGetUniformLocation(shader_program, "proj");
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

		for (int i = 0; i < 10; i++) {
			glm::mat4 trans;
			trans = glm::rotate(trans, glm::radians(10.0f) + i, glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::translate(trans, glm::vec3(i - 5, i * 112 % 5, (i * 13 % 5) * 2 - 2.5));
			GLint uniTrans = glGetUniformLocation(shader_program, "trans");
			glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		frameCount++;
		window.display();
	}

	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
	glDeleteProgram(shader_program);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	
	glDeleteVertexArrays(1, &vao);
 
	window.close();
}
 