#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
class geometry {
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	GLuint buffVertices;
	GLuint buffUvs;
	GLuint buffNormals;
	geometry(std::vector<glm::vec3>&vecs, std::vector<glm::vec2>&uv, std::vector<glm::vec3>&norm);
};