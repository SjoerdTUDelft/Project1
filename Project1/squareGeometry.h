#pragma once

#include "geometry.h"
#include <glm/glm.hpp>
#include <vector>

class squareGeometry : public geometry {
public:
	squareGeometry() {
		DrawMethod = GL_TRIANGLES;
		vector<glm::vec3> pos{ glm::vec3(-1.0, 1.0,0.0), glm::vec3(1.0, 1.0,0.0),
							   glm::vec3(-1.0,-1.0,0.0), glm::vec3(1.0,-1.0,0.0) };
		vector<glm::vec3> normals{ glm::vec3(0.0,0.0,1.0),glm::vec3(0.0,0.0,1.0),
								   glm::vec3(0.0,0.0,1.0),glm::vec3(0.0,0.0,1.0) };
		vector<glm::vec2> uvs{ glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
							   glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
		vector<VertexPNU> vertex{ VertexPNU{ pos[0], normals[0], uvs[0] }, VertexPNU{ pos[1], normals[1], uvs[1] },
									VertexPNU{ pos[2], normals[2], uvs[2] }, VertexPNU{ pos[3], normals[3], uvs[3] } };
		vector<GLuint> indices{
			2, 1, 0,   // First Triangle
			1, 2, 3    // Second Triangle
		};
		assignData( indices, vertex);
	}
};