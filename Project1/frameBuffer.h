#pragma once

#include <GL\glew.h>
#include "texture.h"
class FrameBuffer {

public:

	Texture FrameTexture;
	GLuint FrameBufferId;
	GLuint RenderBufferId;


	FrameBuffer() {}
	void SetupFrameBuffer( int width, int height) {
		FrameTexture.width = width;
		FrameTexture.height = height;
		glGenFramebuffers(1, &FrameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferId);

		
		glBindTexture(GL_TEXTURE_2D, FrameTexture.TextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FrameTexture.TextureId , 0);

 
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	}

	void Resize(int width, int height) {
		glBindTexture(GL_TEXTURE_2D, FrameTexture.TextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}

	~FrameBuffer() {
		glDeleteFramebuffers(1, &FrameBufferId);
	}
};