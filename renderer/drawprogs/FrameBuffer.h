/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "renderer/tr_local.h"	
#define MULTIPASSES 1

class nmFrameBuffer
{
public:
	nmFrameBuffer(int Atachaments);
	~nmFrameBuffer()
	{
		glDeleteFramebuffers(1, &frameBuffer);

#if NMS_FRAME_BUFFER_TEXTURE
#else
		glDeleteTextures(5, ColorTextures);
#endif
		glDeleteRenderbuffers(5, ColorTextures);
	};

	static enum BindTo_t
	{
		TO_BIND = 0,
		TO_READ,
		TO_DRAW
	};

	static enum SCREEN_TEXTURE_TYPE
	{
		SCREEN_TEXTURE_ATTACHAMENT_0,
		SCREEN_TEXTURE_ATTACHAMENT_1,
		SCREEN_TEXTURE_ATTACHAMENT_2,
		SCREEN_TEXTURE_ATTACHAMENT_3,
		SCREEN_TEXTURE_ATTACHAMENT_4
	};

	//Create The Buffer
	bool			GenerateFB(int Width, int Height, bool HasDepth = false);

	//Bind The Buffer
	void			Bind(BindTo_t Bindt = TO_BIND);
	GLuint			GetFramebuffer() { return this->frameBuffer; };


#if NMS_FRAME_BUFFER_TEXTURE
	void			AttachBuffer(int format, int index);
	void			AttachImage(int target, const idImage* image, int index, int mipmapLod = 0);
#endif

	// back to default
	static void		BindDefalt()
	{ 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		backEnd.glState.currentFramebuffer = NULL;
	}

#if NMS_FRAME_BUFFER_TEXTURE
#else
	GLuint	GetRenderTextures(SCREEN_TEXTURE_TYPE type)	{	return ColorTextures[type];	}
	void	BindRenderTextures(GLenum TexEum, SCREEN_TEXTURE_TYPE type)	{ glBindTexture(TexEum, ColorTextures[type]); }
#endif

private:

	unsigned int Atachaments;

	//Frame Buffers
	GLuint frameBuffer;

	//the Render Buffer
	GLuint renderBuffer[5];

#if NMS_FRAME_BUFFER_TEXTURE
#else
	//Textures Buffers
	GLuint ColorTextures[5];
#endif
};


class nmFrameBuffersGlobal
{
public:
	nmFrameBuffersGlobal();
	~nmFrameBuffersGlobal();

	void				init();
	void				CreateScreen();
	nmFrameBuffer*		RenderBuffer;

	void				DrawScreenRect();
	void				ClearScreenRect();

	GLuint RSCreenVAO;
private:
	bool QuadScreenSet;

	GLuint RScreenVB[2];
};

extern nmFrameBuffersGlobal GlobalFrameBuffers;

#endif /* !FRAMEBUFFER_H */
