 
#include <GL/glew.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "logging.h"
#include "ShaderLoader.h"
using namespace std;

char *textFileRead(const char *fn)
{
	FILE *fp;
	char *content = NULL;
	
	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");
		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}

	if (content == NULL)
	{
		fprintf(stderr, "ERROR: could not load in file %s\n", fn);
		exit(1);
	}
	return content;
}
int textFileWrite(char *fn, char *s)
{
	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn, "w");

		if (fp != NULL) {

			if (fwrite(s, sizeof(char), strlen(s), fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}
bool bCheckShaderCompile(GLuint shader) {

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar * errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		gl_log_err(errorLog);
 
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		delete[] errorLog;
		return false;
	}
	return true;

}
GLint loadShader(GLuint &shader, const char * const pathVertex, const char * const pathFragment) {
	char *vs, *fs;
	GLuint shader_vertex, shader_fragment;
	shader_vertex = glCreateShader(GL_VERTEX_SHADER);
	shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
	vs = textFileRead(pathVertex);
	fs = textFileRead(pathFragment);
	const char * ff = fs;
	const char * vv = vs;
	glShaderSource(shader_vertex, 1, &vv, NULL);
	glShaderSource(shader_fragment, 1, &ff, NULL);
	free(vs); free(fs);

	glCompileShader(shader_vertex);
	if (bCheckShaderCompile(shader_vertex) == 0) {
		cout << "Error loading vertexshader at FILE: " << __FILE__ << "\nAnd Line: " << __LINE__ << endl;
		cout << pathVertex << endl;
		exit(-1);
	}
	glCompileShader(shader_fragment);
	if (bCheckShaderCompile(shader_fragment) == 0) {
		cout << "Error loading fragmentshader at FILE: " << __FILE__ << "\nAnd Line: "<< __LINE__ << endl;
		cout << pathFragment << endl;
		exit(-1);
	}
	shader = glCreateProgram();
	glAttachShader(shader, shader_fragment);
	glAttachShader(shader, shader_vertex);
	glLinkProgram(shader);
	GLint isLinked = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar * errorLog = new GLchar[maxLength];
		glGetProgramInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		gl_log_err(errorLog);
		cout << "Error Linking Program" << endl;

		delete[] errorLog;
		glDeleteProgram(shader);
		exit(-1);
	}


	glDetachShader(shader, shader_vertex);
	glDetachShader(shader, shader_fragment);
	glDeleteShader(shader_vertex);
	glDeleteShader(shader_fragment);
	return shader;

}