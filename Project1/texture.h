#pragma once
#include <GL/glew.h>
#include <soil\SOIL.h>
#include "logging.h"



class Texture {
	public:

		GLuint TextureId;
		int width = 1;
		int height = 1;
		static unsigned int TextureNumber;
		Texture() {
			glGenTextures(1, &TextureId);
			TextureNumber++;
		};

		void LoadTexture(const char * const path) {
			unsigned char* loadedImageData = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
			if (loadedImageData == NULL) {
				MY_PRINT(path);
				exit(-1);
			}
			glBindTexture(GL_TEXTURE_2D, TextureId);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedImageData);
			SOIL_free_image_data(loadedImageData);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void ActivateTexture(GLuint ID) {
			glActiveTexture(GL_TEXTURE0 + ID);
			glBindTexture(GL_TEXTURE_2D, TextureId);
		}

		~Texture() {
			glDeleteTextures(1, &TextureId);
			TextureNumber--;
		}
};

unsigned int Texture::TextureNumber = 0;