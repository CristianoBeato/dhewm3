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


===========================================================================
*/

#ifndef _IMAGES_GL_H_
#define _IMAGES_GL_H_

#include <GL\glew.h>

enum ImageTpe_T
{
	IT_GL_Image2D = 0,
	IT_GL_Image2DMultisample,
	IT_GL_Image2DCompressed,
	IT_GL_Image1D,
	IT_GL_Image3D
};

enum TInternalFormat_T
{

};

enum TFormat_T
{
	TEXTFOR_RGB = 0,
	TEXTFOR_RGB_ALPHA,
	TEXTFOR_RGB_ALPHA_INTEGER,
	TEXTFOR_BGR,
	TEXTFOR_BGR_ALPHA,
	TEXTFOR_BGR_ALPHA_INTEGER,
	TEXTFOR_RED,
	TEXTFOR_GREEN,
	TEXTFOR_BLUE,
	TEXTFOR_RED_INTEGER,
	TEXTFOR_GREEN_INTEGER,
	TEXTFOR_BLUE_INTEGER

};

enum TType_t
{
	TEXTY_BYTE = 0,
	TEXTY_UBYTE,
	TEXTY_INT,
	TEXTY_UINT,
	TEXTY_SHORT,
	TEXTY_USHOR,
	TEXTY_FLOAT,
	TEXTY_HALF_FLOAT
};

class GlTexture
{
public:
	GlTexture();
	~GlTexture();

	static const GLuint TEXTURE_NOT_LOADED = 0xFFFFFFFF;

	GLenum SetInternalFormat(TInternalFormat_T _Format);
	GLenum SetFormat(TFormat_T _Format);
	GLenum SetType(TType_t _Type);

	void	GenerateImage(GLenum Texture, GLenum Target, void* data, ImageTpe_T GLImage = IT_GL_Image2D, int Sample = 0);

private:
	GLenum type;
	GLenum format;
	GLenum internalFormat;
	int Width, Height;
};



#endif //!_IMAGES_GL_H_