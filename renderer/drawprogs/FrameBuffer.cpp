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

#include "FrameBuffer.h"

nmFrameBuffersGlobal GlobalFrameBuffers;

nmFrameBuffer::nmFrameBuffer(int Atachaments)
{
	frameBuffer = GLuint(0);

	//Maximum of 5 Color Attachament
	assert(Atachaments <= 5);
	this->Atachaments = Atachaments;

#if NMS_FRAME_BUFFER_TEXTURE
#else
	memset(ColorTextures, 0, sizeof(ColorTextures));
#endif

}

bool	nmFrameBuffer::GenerateFB(int Width, int Height, bool HasDepth)
{
	//Render FreameBuffer
	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

#if NMS_FRAME_BUFFER_TEXTURE
#else
	//the Color Textures
	glGenTextures(Atachaments, ColorTextures);
	for (unsigned int i = 0; i < Atachaments; i++)
	{
		glBindTexture(GL_TEXTURE_2D, ColorTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColorTextures[i], 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
#endif

	glGenRenderbuffers(Atachaments, renderBuffer);
	for (unsigned int i = 0; i < Atachaments; i++)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
#if NMS_FRAME_BUFFER_TEXTURE 
#else
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, renderBuffer[i]);
#endif
	}

	GLenum DrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT5,
		};

	GLuint DBlength = sizeof(DrawBuffers[0]) / sizeof(DrawBuffers);
	glDrawBuffers(DBlength, DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) 
	{
		common->Warning("FRAMEBUFFER error, status: 0x%x\n", Status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

#if 0
void	nmFrameBuffer::AttachBuffer(int format, int index)
{
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer[index]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, renderBuffer[index]);

	GL_CheckErrors();
}

void	nmFrameBuffer::AttachImage(int target, const idImage* image, int index, int mipmapLod)
{
	if ((target != GL_TEXTURE_2D) && (target != GL_TEXTURE_2D_MULTISAMPLE))
	{
		common->Error("::Framebuffer::AttachImage2D( %s ): invalid target");
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, image->texnum, mipmapLod);
}
#endif
void	nmFrameBuffer::Bind(BindTo_t Bindt)
{
	GLenum Bindtype;

	//type of binding
	switch (Bindt)
	{
	case nmFrameBuffer::TO_BIND:
		Bindtype = GL_FRAMEBUFFER;
		break;

	case nmFrameBuffer::TO_READ:
		Bindtype = GL_READ_FRAMEBUFFER;
		break;

	case nmFrameBuffer::TO_DRAW:
		Bindtype = GL_DRAW_FRAMEBUFFER;
		break;

	default:
		Bindtype = GL_FRAMEBUFFER;
		break;
	}

	//Bind Frame Buffer 
	glBindFramebuffer(Bindtype, frameBuffer);

	//Check The Bind
	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		common->Error("::nmFrameBuffer::Incomplete, incomplete attachment");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		common->Error("::nmFrameBuffer:: Incomplete, missing attachment");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		common->Error("::nmFrameBuffer::Incomplete, missing draw buffer");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		common->Error("::nmFrameBuffer::Incomplete, missing read buffer");
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		common->Error("::nmFrameBuffer::Incomplete, Unsupported framebuffer format");
		break;

	default:
		break;
	}
}

nmFrameBuffersGlobal::nmFrameBuffersGlobal()
{

	QuadScreenSet = false;

	//Create a Buffer whit 3 colors Attachaments
	RenderBuffer = new nmFrameBuffer(3);
}

nmFrameBuffersGlobal::~nmFrameBuffersGlobal()
{
	delete RenderBuffer;

	ClearScreenRect();
}

void	nmFrameBuffersGlobal::init()
{
	//the Render Buffer that Stores the Render Color, Render Normal, Depth, and Render Specular
	//for the deffered passes
	RenderBuffer->GenerateFB(glConfig.vidWidth, glConfig.vidHeight);

#if NMS_FRAME_BUFFER_TEXTURE
	//Bind the Textures
	RenderBuffer->AttachImage(GL_TEXTURE_2D, globalImages->FirstPostFXPASS_COLOR, 0, 0); //the Color Attachament 
	RenderBuffer->AttachBuffer(GL_RGBA16F, 0);

	RenderBuffer->AttachImage(GL_TEXTURE_2D, globalImages->FirstPostFXPASS_NORMAL, 1, 0);//the Normal Attachament 
	RenderBuffer->AttachBuffer(GL_RGBA16F, 1);

	RenderBuffer->AttachImage(GL_TEXTURE_2D, globalImages->FirstPostFXPASS_SPECULAR, 2, 0);//the Specular attachament
	RenderBuffer->AttachBuffer(GL_RGBA16F, 2);
#endif

}

//creathe the render rect based on th view Port Size
void	nmFrameBuffersGlobal::CreateScreen()
{

#if 1
	// screen power of two correction factor, assuming the copy to _currentRender
	// also copied an extra row and column for the bilerp
	float	parm[2];
	int	 w = backEnd.viewDef->viewport.x2 - backEnd.viewDef->viewport.x1 + 1;
	int pot = globalImages->currentRenderImage->uploadWidth;
	parm[0] = (float)w / pot;

	int	 h = backEnd.viewDef->viewport.y2 - backEnd.viewDef->viewport.y1 + 1;
	pot = globalImages->currentRenderImage->uploadHeight;
	parm[1] = (float)h / pot;
#endif

	int VPw = glConfig.vidWidth / 2;
	int VPh = glConfig.vidHeight / 2;

	GLfloat quadVertices[] = { // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// Positions		// TexCoords
		//0.0f, 0.0f,			0.0f, parm[1],
		0.0f, 0.0f,			//0.0f, 0.0f,
		//VPw, 0,				0.0f, 0.0f,
		VPw, 0,			//	1.0f, 0.0f,
		//VPw, VPh,			parm[1], 0.0f,
		VPw, VPh,		//	1.0f, 1.0f,
		//0.0f, 0.0f,			0.0f, parm[1],
		0.0f, 0.0f,		//	0.0f, 0.0f,
		//VPw, VPh,			parm[1], 0.0f,
		VPw, VPh,		//	1.0f, 1.0f,
		//0.0f, VPh,			parm[1], parm[1]
		0.0f, VPh,		//	0.0f, 1.0f
	};

	GLfloat quadTextCoord[] = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

	glGenVertexArrays(1, &RSCreenVAO);
	glGenBuffers(2, RScreenVB);

	glBindVertexArray(RSCreenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, RScreenVB[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(quadVertices) * sizeof(GLfloat), (GLvoid*)0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, RScreenVB[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextCoord), &quadTextCoord, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(quadTextCoord)* sizeof(GLfloat), (GLvoid*)0);
	

	glBindVertexArray(0);

	QuadScreenSet = true;
};

void nmFrameBuffersGlobal::DrawScreenRect()
{
	//are not Set
	if (!QuadScreenSet)
		return;

	// Draw Screen
	glBindVertexArray(RSCreenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindVertexArray(1);
}

void nmFrameBuffersGlobal::ClearScreenRect()
{
	glDeleteBuffers(1, RScreenVB);
	glDeleteVertexArrays(1, &RSCreenVAO);
}