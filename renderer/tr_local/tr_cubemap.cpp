/*
===========================================================================

NMS-DOOM 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the NMS-DOOM 3 GPL Source Code ("NMS-DOOM 3 Source Code").

NMS-DOOM 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

NMS-DOOM 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with NMS-DOOM 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the NMS-DOOM 3 Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the NMS-DOOM 3 Source Code.
If not, please request a copy in writing from id Software at the address below.
see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#include "tr_cubemap.h"
Cubemap::Cubemap()
{
	//gen image texture
	glGenTextures(1, &CubemapTexture);
	//gen framebuffers
	glGenFramebuffers(1, &CmFrameB);
	glGenRenderbuffers(1, &CmDepthB);
}

Cubemap::~Cubemap()
{
	//delete the Cubemap texture
	glDeleteTextures(1, &CubemapTexture);
	//delete the framebuffers
	glDeleteFramebuffers(1, &CmFrameB);
	glDeleteFramebuffers(1, &CmDepthB);
}

void Cubemap::Create(bool Dinamic, int width, int height, idVec3 Position, idStr Img)
{

	//set the cubemap data
	this->width = width;
	this->height = height;
	this->Position = Position;

	this->Dinamic = Dinamic;

	//create a static or a dinamic cubemap
	if (Dinamic)
		this->dinamicCM();
	else
		this->staticCM(Img);
}

//Create a dinamic cubemap
void Cubemap::dinamicCM()
{
	// create the cubemap
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);

#if 0
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

	// set textures
	for (int i = 0; i < 6; ++i)
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB16F, GL_UNSIGNED_BYTE, 0);//FOR HDR Rendering

	// create the fbo
	glBindFramebuffer(GL_FRAMEBUFFER, CmFrameB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_COLOR_COMPONENTS, width, height);

#if 0
	// create the uniform depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, CmDepthB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
#endif

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

#if 0
	// attach it
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, CmFrameB);
#endif

	// attach only the +X cubemap texture (for completeness)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, CubemapTexture, CmFrameB);
	verifyStatus();

	// disable
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}



void Cubemap::Bind()
{
	// iterate over this variable six times, once for each face. See below
	unsigned int face = 0;

	//set the position to render
	glTranslatef(Position.x, Position.y, Position.z);

	//process face b face
	for (face = 0; face < 6; face++)
	{

		// attach new texture and renderbuffer to fbo
#if 0
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)face, CubemapTexture, 0);
		verifyStatus();
#else
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)face, CubemapTexture, 0);
#endif
		GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			printf("Status error: %08x\n", status);

		// clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// play around with the near and far values
		gluPerspective(90, 1, 1, 1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// setup lookat depending on current face
		switch (face)
		{
		case POSITIVE_X:
			gluLookAt(0, 0, 0, 1, 0, 0, 0, 1, 0);
			break;

		case NEGATIVE_X:
			gluLookAt(0, 0, 0, -1, 0, 0, 0, 1, 0);
			break;

		case POSITIVE_Y:
			gluLookAt(0, 0, 0, 0, 10, 0, 1, 0, 0);
			break;

		case NEGATIVE_Y:
			gluLookAt(0, 0, 0, 0, -10, 0, 1, 0, 0);
			break;

		case POSITIVE_Z:
			gluLookAt(0, 0, 0, 0, 0, 10, 0, 1, 0);
			break;

		case NEGATIVE_Z:
			gluLookAt(0, 0, 0, 0, 0, -10, 0, 1, 0);
			break;

		default:
			break;
		};
	}
}