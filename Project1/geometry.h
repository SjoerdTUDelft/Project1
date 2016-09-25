#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>


struct VertexPNU {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class geometry {
public:
 
	std::vector<GLfloat> FloatData;
	std::vector<VertexPNU> vertPNU;
	std::vector<GLuint> indices;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint DrawMethod;
	GLuint buffVertex;
	GLuint buffVertices;
	GLuint buffIndices;
	GLuint buffUvs;
	GLuint buffNormals;
	geometry(std::vector<glm::vec3>&vecs, std::vector<glm::vec2>&uv, std::vector<glm::vec3>&norm)
		: vertices(std::move(vecs)), uvs(std::move(uv)), normals(std::move(norm)) {
	
			//DRAWARRAYS
		
		glGenBuffers(1, &buffVertices);
		glBindBuffer(GL_ARRAY_BUFFER, buffVertices);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices[0], GL_STATIC_DRAW);


		glGenBuffers(1, &buffUvs);
		glBindBuffer(GL_ARRAY_BUFFER, buffUvs);
		glBufferData(GL_ARRAY_BUFFER, this->uvs.size() * sizeof(glm::vec2), &this->uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &buffNormals);
		glBindBuffer(GL_ARRAY_BUFFER, buffNormals);
		glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_STATIC_DRAW);




		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffVertices);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, buffNormals);
		glVertexAttribPointer(
			1,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, buffUvs );
		glVertexAttribPointer(
			2,                  // attribute
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	
	
	
	};
	geometry() {}
	
	void assignData(std::vector<GLuint>&indicesinput, std::vector<VertexPNU>&vertex) {

		//DRAWELEMENTS

		vertPNU = std::move(vertex);
		indices = std::move(indicesinput);

		glGenVertexArrays(1, &this->buffVertex);
		glGenBuffers(1, &this->buffVertices);
		glGenBuffers(1, &this->buffIndices);

		glBindVertexArray(this->buffVertex);
		glBindBuffer(GL_ARRAY_BUFFER, this->buffVertices);
		glBufferData(GL_ARRAY_BUFFER, this->vertPNU.size() * sizeof(VertexPNU),
			&this->vertPNU[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffIndices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
			&this->indices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNU),
			(GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNU),
			(GLvoid*)offsetof(VertexPNU, normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPNU),
			(GLvoid*)offsetof(VertexPNU, uv));

		glBindVertexArray(0);
	}

	void assignData(std::vector<GLuint>&indicesinput, std::vector<GLfloat> & vertex) {

		//DRAWELEMENTS

		FloatData = std::move(vertex);
		indices = std::move(indicesinput);
		
		glGenBuffers(1, &buffVertices);
		glBindBuffer(GL_ARRAY_BUFFER, buffVertices);
		glBufferData(GL_ARRAY_BUFFER, this->FloatData.size() * sizeof(GLfloat) , &this->FloatData[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffVertices);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);


 		glBindVertexArray(0);
	}



	void DrawArray() {
		glDrawArrays(GL_TRIANGLES, 0, this->FloatData.size());
	}
	void DrawElements() {
	 	glBindVertexArray(this->buffVertex);
		glDrawElements(DrawMethod, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};
