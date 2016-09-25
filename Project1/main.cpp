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
#include "objLoader.h"
#include "ShaderLoader.h"
#include "squareGeometry.h"
#include "frameBuffer.h"
#include "texture.h"
#include "skyboxGeometry.h"

#define M_PI atan(1)*4

using namespace std;
 
 //  define the window position on screen
int window_x;
int window_y;
int window_width = 960;
int window_height = 540;
sf::Window * windowref;
char *window_title = "Sample OpenGL FreeGlut App";
char * GL_LOG_FILE = "apple.txt";

LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double elapsedTime;

FrameBuffer * Buffer;

GLuint shader_program;
GLuint shader_intOutline;
GLuint shader_postProcessing;
GLuint shader_skyBox;
GLuint shader_basic;

glm::vec3 position(0.0f,0.0f,0.0f);
glm::vec3 direction(0.0f, 0.0f, 0.0f);
glm::vec3 camLeft(0.0f, 0.0f, 0.0f);
glm::vec2 mousePos( -1.0f, -1.0f);
const float sensitivity = 0.2;
const float limit = 0.8* M_PI / 2.0f;
float deltaTime = 0;
float camPitch = 0;
float camYaw = 0;
const float minSpeed = 8.0;
const float maxSpeed = 16.0;
float speed = minSpeed;
const float extraFov = 60;
const float minFov = 35;
float fovScalar = 1;
float smoothFovScalar = 1;
chrono::time_point<chrono::steady_clock> t_start_app;
chrono::time_point<chrono::steady_clock> t_start;
chrono::time_point<chrono::steady_clock> t_now;

bool msaa = false;
int lightType = 0;
 
void processKeys() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
		cout << "ESCAPE WAS PRESSED" << endl;
		exit(0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		position -= camLeft * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		position += camLeft * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		position += direction * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		position -= direction * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		position += glm::vec3(0.0,1.0, 0.0) * deltaTime * speed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
		fovScalar -= deltaTime * 3;
		if (fovScalar < 0) fovScalar = 0;
		smoothFovScalar = glm::smoothstep(0.0f, 1.0f, fovScalar);
		speed = maxSpeed;
	} else {
		speed = minSpeed;
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
 
void _update_fps_counter() {
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) *1000.0 / frequency.QuadPart;
	QueryPerformanceCounter(&t1);

	static int frame_count;
	static double elapsed_seconds;
	elapsed_seconds += elapsedTime;

	if (elapsed_seconds > 500.0) {
		float ms = (double)(elapsed_seconds)/(double)frame_count ;
		float fps = (double)frame_count/ (double)(elapsed_seconds/1000.0);
		char tmp[64];
		sprintf(tmp, "opengl @ fps: %3.2f, ms: %3.2f", fps,ms);
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
	settings.antialiasingLevel = 0;

	sf::Window window(sf::VideoMode(window_width, window_height, 32), "OpenGL", sf::Style::Default, settings);
	windowref = &window;
	windowref->setMouseCursorGrabbed(true);
	windowref->setKeyRepeatEnabled(false);
	windowref->setMouseCursorVisible(false);

	restart_gl_log();


	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	//glDepthMask(GL_FALSE);
	//glewExperimental = GL_TRUE;
	glewInit();

	//GEOMETRYLOADING
 	geometry testObject = bloadObj("./objects/testobject.obj");
	squareGeometry square;
	skyboxGeometry skyBox;

	//TEXTURELOADING
	Texture texture;  texture.LoadTexture("images/testobject.png");
	GLuint CubeMap; glGenTextures(1, &CubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	vector<string> textures_face;
	for (int i = 0; i < 6; i++) {
		//Right Left Top Bottom Back Front
		textures_face.push_back("images/CubeMaps/3DScene/" + to_string(i+1) + ".jpg");
		//cout << textures_face.at(i) << endl;
	}



	int width, height;
	for(int i = 0; i < 6; i++) {
		unsigned char* loadedImageData  = SOIL_load_image(textures_face.at(i).c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		 if (loadedImageData == NULL) {
			 MY_PRINT(textures_face.at(i).c_str());
			 exit(-1);
		 }
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, loadedImageData
		);
	}




	//SHADERLOADING
	loadShader(shader_program, "./shaders/standardmaterial.vert", "./shaders/standardmaterial.frag");
	loadShader(shader_intOutline, "./shaders/standardmaterial.vert", "./shaders/testshader.frag");
	loadShader(shader_postProcessing, "./shaders/postprocess.vert", "./shaders/postprocess.frag");
	loadShader(shader_skyBox, "./shaders/skybox.vert", "./shaders/skybox.frag");
	loadShader(shader_basic, "./shaders/basic.vert", "./shaders/basic.frag");

	//SHADERPARAMETERS
	glUseProgram(shader_program);
	glUniform1i(glGetUniformLocation(shader_program, "ourTexture1"), 0);

	glUniform1f(glGetUniformLocation(shader_program, "directionalLights[0].intensity"), .30f);
	glUniform3f(glGetUniformLocation(shader_program, "directionalLights[0].direction"), 0.0f, -1.0, 1.0);
	glUniform1f(glGetUniformLocation(shader_program, "pointLights[0].intensity"), 3.0f);

	//glUniform1f(glGetUniformLocation(shader_program, "spotLights[0].intensity"), .20f);
	//glUniform1f(glGetUniformLocation(shader_program, "spotLights[0].outerAngle"), 30.0f);
	//glUniform1f(glGetUniformLocation(shader_program, "spotLights[0].innerAngle"), 25.0f);
	//glUniform3f(glGetUniformLocation(shader_program, "spotLights[0].direction"), 0.0f, 0.0, 1.0);


	FrameBuffer Portal; Portal.SetupFrameBuffer(1600, 1600 * window_height / window_width);
	FrameBuffer SceneRender; SceneRender.SetupFrameBuffer(window_width, window_height);
	Buffer = &SceneRender;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		MY_PRINT("BUFFER NOT ATTACHED");
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	texture.ActivateTexture(0);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
	Portal.FrameTexture.ActivateTexture(2);
	SceneRender.FrameTexture.ActivateTexture(3);


	GLenum err = glGetError();
	if (err != 0) {
		cout << err << endl;
	}

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
				window_width = windowEvent.size.width;
				window_height = windowEvent.size.height;
				glViewport(0, 0, window_width, window_height);
				Buffer->Resize(window_width, window_height);
				break;
			case sf::Event::MouseButtonPressed:
				switch (windowEvent.mouseButton.button) {
				case sf::Mouse::Left:
					msaa = !msaa;
					lightType++;
						if (lightType > 2) lightType = 0;
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
					//cout << windowEvent.mouseMove.x  << " AND " << windowEvent.mouseMove.y << " AND "<< frameCount << endl;
				}
				break;
			}
		}
		if (windowref->hasFocus()) {

			glm::vec2 currentPos(sf::Mouse::getPosition(*windowref).x, sf::Mouse::getPosition(*windowref).y);
	
			//cout << currentPos.x << " AND " << currentPos.y << endl;
			//cout << mousePos.x << " AND " << mousePos.y << endl;
			//cout << frameCount << endl;
			glm::vec2 delta = mousePos - currentPos;

			camPitch += delta.y*sensitivity * deltaTime;
			if (camPitch > limit) camPitch = limit;
			else if (camPitch < -limit) camPitch = -limit;
			camYaw += delta.x * sensitivity * deltaTime;

			//cout << delta.x << " AND " << delta.y << " AND " << frameCount << endl;


			sf::Vector2u windowSize = windowref->getSize();
			windowSize.x /= 2;
			windowSize.y /= 2;
			sf::Mouse::setPosition(sf::Vector2i(windowSize.x, windowSize.y), *windowref);
			mousePos.x = sf::Mouse::getPosition(*windowref).x;
			mousePos.y = sf::Mouse::getPosition(*windowref).y;
		}
		GLenum err = glGetError();
		if (err != 0) {
			cout << err << endl;
		}
		t_now = chrono::high_resolution_clock::now();
		deltaTime = chrono::duration<float>(t_now - t_start).count();
		t_start = chrono::high_resolution_clock::now();
		processKeys();
		_update_fps_counter();

		
		double time = chrono::duration<double>(t_now - t_start_app).count();
		time = (sin(time) / 2.0 + 0.5) * 6 + 1;
		time = 0;

		//direction = glm::vec3(
		//	cos(camPitch) * sin(camYaw),
		//	cos(camPitch) * cos(camYaw),
		//	sin(camPitch)
		//);
		//camLeft = glm::vec3(
		//	sin(camYaw - 3.14f / 2.0f),
		//	cos(camYaw - 3.14f / 2.0f),
		//	0.0f

		//);
		//glm::vec3 up = glm::cross(direction, camLeft);
		//glm::mat4 V = glm::lookAt(
		//	glm::vec3(position),
		//	glm::vec3(position + direction),
		//	glm::vec3(up)
		//);

		direction = glm::vec3(
			cos(camPitch) * sin(camYaw),
			sin(camPitch),
			cos(camPitch) * cos(camYaw)
		);
		camLeft = glm::vec3(
			sin(camYaw - 3.14f / 2.0f),
			0.0f,
			cos(camYaw - 3.14f / 2.0f)

		);
		glm::vec3 up = glm::cross(camLeft, direction);
		glm::mat4 V = glm::lookAt(
			glm::vec3(position),
			glm::vec3(position + direction),
			glm::vec3(up)
		);

 
		glm::mat4 P = glm::perspective(glm::radians(minFov + extraFov*smoothFovScalar), (float)window_width / (float)window_height, 0.1f, 100.0f);

		//FRAMEBUFFER PORTAL
		glBindFramebuffer(GL_FRAMEBUFFER, Portal.FrameBufferId);
		glViewport(0, 0, Portal.FrameTexture.width, Portal.FrameTexture.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//SKYBOX
		glPolygonMode(GL_FRONT, GL_FILL);
		glUseProgram(shader_skyBox);
		glUniform1i(glGetUniformLocation(shader_skyBox, "skybox"), 1);
		glm::mat4 view = glm::mat4(glm::mat3(V));
		glUniformMatrix4fv(glGetUniformLocation(shader_skyBox, "V"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader_skyBox, "P"), 1, GL_FALSE, glm::value_ptr(P));
		glDisable(GL_STENCIL_TEST);
		skyBox.DrawArray();
		glEnable(GL_STENCIL_TEST);


		glUseProgram(shader_program);
		glUniform1i(glGetUniformLocation(shader_program, "ourTexture1"), 0);
		for (int i = 0; i < 100; i++) {
			glPolygonMode(GL_FRONT, GL_FILL);
			glm::mat4 M;
			
			M = glm::translate(M, glm::vec3(cos(M_PI *2.0*(float)i / 20) * 7,
				floor((float)i / 20) * 2,
				15 + sin(M_PI *2.0*(float)i / 20) * 7));

			M = glm::rotate(M, (float)(-0.5 *M_PI - M_PI *2.0 * (float)i / 20), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat3 N(V*M);
			N = glm::inverse(N);

			glEnable(GL_DEPTH_TEST);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glUseProgram(shader_program);
			
			glUniformMatrix3fv(glGetUniformLocation(shader_program, "N"), 1, GL_TRUE, glm::value_ptr(N));
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "M"), 1, GL_FALSE, glm::value_ptr(M));
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "V"), 1, GL_FALSE, glm::value_ptr(V));
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "P"), 1, GL_FALSE, glm::value_ptr(P));
				   glUniform1f(glGetUniformLocation(shader_program, "Time") , time);
				   glUniform1f(glGetUniformLocation(shader_program, "fov") , fovScalar);
				   glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].position"), 0.0f, 0.0, time);

			testObject.DrawElements();
			glLineWidth(3);
			glPolygonMode(GL_FRONT, GL_LINE);

			glDisable(GL_DEPTH_TEST);
			glStencilMask(0x00);
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	 
			glUseProgram(shader_intOutline);
			glUniformMatrix3fv(glGetUniformLocation(shader_intOutline, "N"), 1, GL_TRUE, glm::value_ptr(N));
			glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "M"),  1, GL_FALSE, glm::value_ptr(M));
			glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "V"),  1, GL_FALSE, glm::value_ptr(V));
			glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "P"), 1, GL_FALSE, glm::value_ptr(P));

			testObject.DrawElements();
			glStencilMask(0xFF);
 
		}



		if (msaa) {
			glBindFramebuffer(GL_FRAMEBUFFER, SceneRender.RenderBufferId);
			glViewport(0, 0, window_width, window_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glPolygonMode(GL_FRONT, GL_FILL);
			glUseProgram(shader_skyBox);
			glUniform1i(glGetUniformLocation(shader_skyBox, "skybox"), 1);
			glm::mat4 view = glm::mat4(glm::mat3(V));
			glUniformMatrix4fv(glGetUniformLocation(shader_skyBox, "V"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shader_skyBox, "P"), 1, GL_FALSE, glm::value_ptr(P));
			glDisable(GL_STENCIL_TEST);
			skyBox.DrawArray();
			glEnable(GL_STENCIL_TEST);

			for (int i = 0; i < 100; i++) {
				glPolygonMode(GL_FRONT, GL_FILL);
				glm::mat4 M;

				M = glm::translate(M, glm::vec3(cos(M_PI *2.0*(float)i / 20) * 7,
					floor((float)i / 20) * 2,
					15 + sin(M_PI *2.0*(float)i / 20) * 7));

				M = glm::rotate(M, (float)(-0.5 *M_PI - M_PI *2.0 * (float)i / 20), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat3 N(V*M);
				N = glm::inverse(N);

				glEnable(GL_DEPTH_TEST);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glUseProgram(shader_program);

				glUniformMatrix3fv(glGetUniformLocation(shader_program, "N"), 1, GL_TRUE, glm::value_ptr(N));
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "M"), 1, GL_FALSE, glm::value_ptr(M));
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "V"), 1, GL_FALSE, glm::value_ptr(V));
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "P"), 1, GL_FALSE, glm::value_ptr(P));
				glUniform1f(glGetUniformLocation(shader_program, "Time"), time);
				glUniform1f(glGetUniformLocation(shader_program, "fov"), fovScalar);
				glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].position"), 0.0f, 0.0, time);

				testObject.DrawElements();
				glLineWidth(3);
				glPolygonMode(GL_FRONT, GL_LINE);

				glDisable(GL_DEPTH_TEST);
				glStencilMask(0x00);
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

				glUseProgram(shader_intOutline);
				glUniformMatrix3fv(glGetUniformLocation(shader_intOutline, "N"), 1, GL_TRUE, glm::value_ptr(N));
				glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "M"), 1, GL_FALSE, glm::value_ptr(M));
				glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "V"), 1, GL_FALSE, glm::value_ptr(V));
				glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "P"), 1, GL_FALSE, glm::value_ptr(P));

				testObject.DrawElements();
				glStencilMask(0xFF);

			}
			glDisable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT, GL_FILL);


			glUseProgram(shader_basic);
			glUniform1i(glGetUniformLocation(shader_basic, "ourTexture1"), 2);

			glm::mat4 M;
			float imagescale = 6.0;

			M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			M = glm::scale(M, glm::vec3(imagescale,
				imagescale*((float)window_height / window_width),
				1.0));

			glUniformMatrix4fv(glGetUniformLocation(shader_basic, "M"), 1, GL_FALSE, glm::value_ptr(M));
			glUniformMatrix4fv(glGetUniformLocation(shader_basic, "V"), 1, GL_FALSE, glm::value_ptr(V));
			glUniformMatrix4fv(glGetUniformLocation(shader_basic, "P"), 1, GL_FALSE, glm::value_ptr(P));

			square.DrawElements();
			

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(1.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glUseProgram(shader_postProcessing);
			glUniform1i(glGetUniformLocation(shader_postProcessing, "ourTexture1"), 3);


			glDisable(GL_DEPTH_TEST);

			square.DrawElements();

			glClearColor(0.0, 0.0, 1.0, 0.0);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.0, 0.0, 1.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, window_width, window_height);

			glEnable(GL_STENCIL_TEST);


			for (int i = 0; i < 100; i++) {
				glPolygonMode(GL_FRONT, GL_FILL);
				glm::mat4 M;

				M = glm::translate(M, glm::vec3(cos(M_PI *2.0*(float)i / 20) * 7,
												floor((float)i / 20) * 2,
												15 + sin(M_PI *2.0*(float)i / 20) * 7));

				M = glm::rotate(M, (float)(-0.5 *M_PI - M_PI *2.0 * (float)i / 20), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat3 N(V*M);
				N = glm::inverse(N);

				glEnable(GL_DEPTH_TEST);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glUseProgram(shader_program);

				glUniformMatrix3fv(glGetUniformLocation(shader_program, "N"), 1, GL_TRUE, glm::value_ptr(N));
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "M"), 1, GL_FALSE, glm::value_ptr(M));
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "V"), 1, GL_FALSE, glm::value_ptr(V));
				glUniformMatrix4fv(glGetUniformLocation(shader_program, "P"), 1, GL_FALSE, glm::value_ptr(P));
				glUniform1f(glGetUniformLocation(shader_program, "Time"), time);
				glUniform1f(glGetUniformLocation(shader_program, "fov"), fovScalar);
				glUniform3f(glGetUniformLocation(shader_program, "pointLights[0].position"), 0.0f, 0.0, time);

				testObject.DrawElements();
				glLineWidth(3);
				glPolygonMode(GL_FRONT, GL_LINE);

				glDisable(GL_DEPTH_TEST);
				glStencilMask(0xFF);
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

				glUseProgram(shader_intOutline);
				glUniformMatrix3fv(glGetUniformLocation(shader_intOutline, "N"), 1, GL_TRUE, glm::value_ptr(N));
				glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "M"), 1, GL_FALSE, glm::value_ptr(M));
				glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "V"), 1, GL_FALSE, glm::value_ptr(V));
				glUniformMatrix4fv(glGetUniformLocation(shader_intOutline, "P"), 1, GL_FALSE, glm::value_ptr(P));

				testObject.DrawElements();
				glStencilMask(0xFF);
			}
			//glDisable(GL_DEPTH_TEST);
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glPolygonMode(GL_FRONT, GL_FILL);
			glUseProgram(shader_skyBox);
			glUniform1i(glGetUniformLocation(shader_skyBox, "skybox"), 1);
			glm::mat4 view = glm::mat4(glm::mat3(V));
			glUniformMatrix4fv(glGetUniformLocation(shader_skyBox, "V"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shader_skyBox, "P"), 1, GL_FALSE, glm::value_ptr(P));
			skyBox.DrawArray();



			glDisable(GL_STENCIL_TEST);
			glPolygonMode(GL_FRONT, GL_FILL);

			glUseProgram(shader_basic);
			glUniform1i(glGetUniformLocation(shader_basic, "ourTexture1"), 2);

			glm::mat4 M;
			float imagescale = 6.0;

			M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			M = glm::scale(M, glm::vec3(imagescale,
				imagescale*((float)window_height / window_width),
				1.0));

			glUniformMatrix4fv(glGetUniformLocation(shader_basic, "M"), 1, GL_FALSE, glm::value_ptr(M));
			glUniformMatrix4fv(glGetUniformLocation(shader_basic, "V"), 1, GL_FALSE, glm::value_ptr(V));
			glUniformMatrix4fv(glGetUniformLocation(shader_basic, "P"), 1, GL_FALSE, glm::value_ptr(P));

			square.DrawElements();

			


		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		frameCount++;
		window.display();
	}


	glDeleteProgram(shader_program);


	//glDeleteBuffers(1, &vbo);
	//glDeleteBuffers(1, &ebo);
	
	//glDeleteVertexArrays(1, &vao);
 
	window.close();
}
 