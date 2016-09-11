#include <GL/glew.h>
#include <gl/freeglut.h>

#include <iostream>
#include <chrono>
#include <errno.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Graphics.hpp>

#include "logging.h"
#include "SOIL.h"
#include "ShaderLoader.h"

#define M_PI atan(1)*4
using namespace std;
 
void display(void);
void centerOnScreen();
 

//  define the window position on screen
int window_x;
int window_y;
int window_width = 480;
int window_height = 480;
char *window_title = "Sample OpenGL FreeGlut App";
char * GL_LOG_FILE = "apple.txt";

LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double elapsedTime;
 
GLuint vao;
GLuint shader_program;
GLuint texture;
GLuint texture2;

glm::vec3 position(0.0f,0.0f,0.0f);
glm::vec3 direction(0.0f, 0.0f, 0.0f);
glm::vec3 camRight(0.0f, 0.0f, 0.0f);
glm::vec2 mousePos( -1.0f, -1.0f);
float deltaTime = 0;
float camPitch = 0;
float camYaw = 0;
float speed = 10.0;
chrono::time_point<chrono::steady_clock> t_start;
chrono::time_point<chrono::steady_clock> t_now;

bool msaa = false;

void mouseMove(int x, int y) {
	float sensitivity = 0.01;
	float limit = 0.6* M_PI/2.0f;
	glm::vec2 currentPos(x, y);
	glm::vec2 delta = mousePos - currentPos;

	camPitch += delta.y*sensitivity;
	if (camPitch > limit) camPitch = limit;
	if (camPitch < -limit) camPitch = -limit;
	camYaw -= delta.x * sensitivity;
	mousePos = glm::vec2(x, y);;
 
}
void mouse(int button, int state,int x, int y){

	mousePos = glm::vec2(x, y);
	
	if (button == 2 && state == 0) {
		static GLint CurrentMode;
		glGetIntegerv(GL_POLYGON_MODE, &CurrentMode);
 
		switch (CurrentMode) {
			case(GL_POINT): glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); cout << "LINE" << endl; break;
			case(GL_LINE): glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); cout << "FILL" << endl; break;
			case(GL_FILL): glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); cout << "POINT" << endl; break;
		}
	} else if (button == 0 && state == 0) {
		


		msaa = !msaa;
		cout << "MSAA is " << (msaa ? "True" : "False") << endl;
	}
}
void reshape(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = 1.0f* w / h;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

 
}
void keyboard(unsigned char key, int x, int y) {
	
	//  0	000	00	0000000	NUL(Null)							32	040	20	0100000	SP	64	100	40	1000000	@	96	140	60	1100000	`
	//	1	001	01	0000001	SOH(Start of Header)				33	041	21	0100001	!	65	101	41	1000001	A	97	141	61	1100001	a
	//	2	002	02	0000010	STX(Start of Text)					34	042	22	0100010	"	66	102	42	1000010	B	98	142	62	1100010	b
	//	3	003	03	0000011	ETX(End of Text)					35	043	23	0100011	#	67	103	43	1000011	C	99	143	63	1100011	c
	//	4	004	04	0000100	EOT(End of Transmission)			36	044	24	0100100	$	68	104	44	1000100	D	100	144	64	1100100	d
	//	5	005	05	0000101	ENQ(Enquiry)						37	045	25	0100101 %	69	105	45	1000101	E	101	145	65	1100101	e
	//	6	006	06	0000110	ACK(Acknowledgement)				38	046	26	0100110 &	70	106	46	1000110	F	102	146	66	1100110	f
	//	7	007	07	0000111	BEL(Bell(geluidssignaal))			39	047	27	0100111	'	71	107	47	1000111	G	103	147	67	1100111	g
	//	8	010	08	0001000	BS(Backspace)						40	050	28	0101000	(	72	110	48	1001000	H	104	150	68	1101000	h
	//	9	011	09	0001001	HT(Horizontal Tab)					41	051	29	0101001)	73	111	49	1001001	I	105	151	69	1101001	i
	//	10	012	0A	0001010	LF(Line Feed(nieuwe regel))			42	052	2A	0101010 *	74	112	4A	1001010	J	106	152	6A	1101010	j
	//	11	013	0B	0001011	VT(Vertical Tab)					43	053	2B	0101011 +	75	113	4B	1001011	K	107	153	6B	1101011	k
	//	12	014	0C	0001100	FF(Form Feed)						44	054	2C	0101100,	76	114	4C	1001100	L	108	154	6C	1101100	l
	//	13	015	0D	0001101	CR(Carriage Return(wagenterugloop))	45	055	2D	0101101 -	77	115	4D	1001101	M	109	155	6D	1101101	m
	//	14	016	0E	0001110	SO(Shift Out)						46	056	2E	0101110	.	78	116	4E	1001110	N	110	156	6E	1101110	n
	//	15	017	0F	0001111	SI(Shift In)						47	057	2F	0101111 /	79	117	4F	1001111	O	111	157	6F	1101111	o
	//	16	020	10	0010000	DLE(Data Link Escape)				48	060	30	0110000	0	80	120	50	1010000	P	112	160	70	1110000	p
	//	17	021	11	0010001	DC1(Device Control 1 (XON))			49	061	31	0110001	1	81	121	51	1010001	Q	113	161	71	1110001	q
	//	18	022	12	0010010	DC2(Device Control 2)				50	062	32	0110010	2	82	122	52	1010010	R	114	162	72	1110010	r
	//	19	023	13	0010011	DC3(Device Control 3 (XOFF))		51	063	33	0110011	3	83	123	53	1010011	S	115	163	73	1110011	s
	//	20	024	14	0010100	DC4(Device Control 4)				52	064	34	0110100	4	84	124	54	1010100	T	116	164	74	1110100	t
	//	21	025	15	0010101	NAK(Negative Acknowledgement)		53	065	35	0110101	5	85	125	55	1010101	U	117	165	75	1110101	u
	//	22	026	16	0010110	SYN(Synchronous Idle)				54	066	36	0110110	6	86	126	56	1010110	V	118	166	76	1110110	v
	//	23	027	17	0010111	ETB(End of Transmission Block)		55	067	37	0110111	7	87	127	57	1010111	W	119	167	77	1110111	w
	//	24	030	18	0011000	CAN(Cancel)							56	070	38	0111000	8	88	130	58	1011000	X	120	170	78	1111000	x
	//	25	031	19	0011001	EM(End of Medium)					57	071	39	0111001	9	89	131	59	1011001	Y	121	171	79	1111001	y
	//	26	032	1A	0011010	SUB(Substitute)						58	072	3A	0111010	:	90	132	5A	1011010	Z	122	172	7A	1111010	z
	//	27	033	1B	0011011	ESC(Escape)							59	073	3B	0111011;	91	133	5B	1011011 [	123	173	7B	1111011	{
	//	28	034	1C	0011100	FS(File Separator)					60	074	3C	0111100	<	92	134	5C	1011100	\	124	174	7C	1111100 |
	//	29	035	1D	0011101	GS(Group Separator)					61	075	3D	0111101 =	93	135	5D	1011101 ]	125	175	7D	1111101	}
	//  30	036	1E	0011110	RS(Record Separator)				62	076	3E	0111110	>	94	136	5E	1011110 ^	126	176	7E	1111110	~
	//	31	037	1F	0011111	US(Unit Separator)					63	077	3F	0111111 ?	95	137	5F	1011111	_	127	177	7F	1111111	DEL
	cout << glutGetModifiers();
	
	switch (key) {
		case 119: position += direction * deltaTime * speed; cout << key << endl; break; //W
		case 65: position -= camRight * deltaTime * speed; break; //A
		case 68: position += camRight * deltaTime * speed; break; //D
		case 115: position -= direction * deltaTime * speed; break; //S
		case 27: exit(0); break; //ESC
	}

}
void processKeys() {
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
		glutSetWindowTitle(tmp);

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

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}


	restart_gl_log();

	glutInit(&  argc, argv);
	centerOnScreen();
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(window_x, window_y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);// |GL_DEPTH);
	glutCreateWindow(window_title);



	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouseMove);
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	//glFrontFace(GL_CW); // GL_CCW for counter clock-wise
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glewExperimental = GL_TRUE;
	glewInit();
	/*
	GLfloat points[]{
		//Pos			//Col					//UV
		 0.5, 0.5, 0.0,   0.0, 0.0, 0.0, 0.0,    1.0, 1.0,
		 0.5,-0.5, 0.0,	  0.0, 0.0, 0.0, 0.0,	 1.0, 0.0,
		-0.5, 0.5, 0.0,	  0.0, 0.0, 0.0, 0.0,	 0.0, 1.0,
		-0.5,-0.5, 0.0,	  0.0, 0.0, 0.0, 0.0,	 0.0, 0.0,
	};
	*/
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
		
		GLuint vbo;
		glGenBuffers(1, &vbo);
		GLuint ebo;
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points) , points, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)*3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)*6));
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,0,GL_RGB, GL_UNSIGNED_BYTE, imageBird);
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

	glm::mat4 proj = glm::perspective(glm::radians(50.0f), (float)window_width / (float)window_height, 1.0f, 30.0f);
	GLint uniProj = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader_program, "ourTexture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(shader_program, "ourTexture2"), 1);

	glutMainLoop();

}

void display( )
{
	processKeys();
 
	t_now = chrono::high_resolution_clock::now();
	deltaTime =  chrono::duration<float>(t_now - t_start).count();
	if (deltaTime > 1.0)deltaTime = 1.0;
 
	t_start = chrono::high_resolution_clock::now();
	_update_fps_counter();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (msaa) {
		glEnable(GL_MULTISAMPLE_ARB);
	}
	else {
		glDisable(GL_MULTISAMPLE_ARB);
	}

	
	
	GLuint uniColor = glGetUniformLocation(shader_program, "triangleColor");
	glUniform1f(uniColor, (sin(deltaTime * 4.0f)/2.0 + 0.5 ));
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

	for (int i = 0; i < 10; i++) {
		glm::mat4 trans;
		trans = glm::rotate(trans, glm::radians(10.0f) + i, glm::vec3(0.0f, 0.0f, 1.0f));
		trans = glm::translate(trans, glm::vec3(i  -5 , i * 112 % 5, (i * 13 % 5)*2-2.5));
		GLint uniTrans = glGetUniformLocation(shader_program, "trans");
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
 
	
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	

	glutSwapBuffers();
}

