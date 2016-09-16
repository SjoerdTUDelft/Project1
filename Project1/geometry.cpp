
#include "geometry.h"

	geometry::geometry(std::vector<glm::vec3>&vecs,std::vector<glm::vec2>&uv, std::vector<glm::vec3>&norm) : vertices(std::move(vecs)), uvs(std::move(uv)), normals(std::move(norm)) {
		//if (vertices.size() > 0) {
		//	glGenBuffers(1, &buffVertices);
		//	glBindBuffer(GL_ARRAY_BUFFER, buffVertices);
		//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		//}
		//if (uvs.size > 0) {
		//	glGenBuffers(1, &buffUvs);
		//	glBindBuffer(GL_ARRAY_BUFFER, buffUvs);
		//	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
		//}
		//if (normals.size() > 0) {
		//	glGenBuffers(1, &buffNormals);
		//	glBindBuffer(GL_ARRAY_BUFFER, buffNormals);
		//	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		//}
	}
	
