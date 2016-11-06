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

#include "images_GL.h"

GlTexture::GlTexture()
{
}

GlTexture::~GlTexture()
{
}

GLenum GlTexture::SetInternalFormat(TInternalFormat_T _IFormat)
{
	return 0;
}

GLenum GlTexture::SetFormat(TFormat_T _Format)
{
	GLenum ColorFormat;

	switch (_Format)
	{
	case TEXTFOR_RGB:
		ColorFormat = GL_RGB;
		break;

	case TEXTFOR_RGB_ALPHA:
		ColorFormat = GL_RGBA;
		break;

	case TEXTFOR_RGB_ALPHA_INTEGER:
		ColorFormat = GL_RGBA_INTEGER;
		break;

	case TEXTFOR_BGR:
		ColorFormat = GL_BGR;
		break;

	case TEXTFOR_BGR_ALPHA:
		ColorFormat = GL_BGRA;
		break;

	case TEXTFOR_BGR_ALPHA_INTEGER:
		ColorFormat = GL_BGRA_INTEGER;
		break;

	case TEXTFOR_RED:
		ColorFormat = GL_RED;
		break;

	case TEXTFOR_GREEN:
		ColorFormat = GL_GREEN;
		break;

	case TEXTFOR_BLUE:
		ColorFormat = GL_BLUE;
		break;

	case TEXTFOR_RED_INTEGER:
		ColorFormat = GL_RED_INTEGER;
		break;

	case TEXTFOR_GREEN_INTEGER:
		ColorFormat = GL_GREEN_INTEGER;
		break;

	case TEXTFOR_BLUE_INTEGER:
		ColorFormat = GL_BLUE_INTEGER;
		break;

	default:
		//Asume RGB as Defalt
		ColorFormat = GL_RGB;
		break;
	}

	return ColorFormat;
}

GLenum GlTexture::SetType(TType_t _Type)
{
	GLenum DataType;
	switch (_Type)
	{
	case TEXTY_BYTE:
		DataType = GL_BYTE;
		break;

	case TEXTY_UBYTE:
		DataType = GL_UNSIGNED_BYTE;
		break;

	case TEXTY_INT:
		DataType = GL_INT;
		break;

	case TEXTY_UINT:
		DataType = GL_UNSIGNED_INT;
		break;

	case TEXTY_SHORT:
		DataType = GL_SHORT;
		break;

	case TEXTY_USHOR:
		DataType = GL_UNSIGNED_SHORT;
		break;

	case TEXTY_FLOAT:
		DataType = GL_FLOAT;
		break;

	case TEXTY_HALF_FLOAT:
		DataType = GL_HALF_FLOAT;
		break;

	default: //Assume the unsigned is defalt data format
		DataType = GL_UNSIGNED_BYTE;
		break;
	}

	return DataType;
}

void GlTexture::GenerateImage(GLenum Texture, GLenum Target, void* data, ImageTpe_T GLImage, int Sample)
{

	//glBindTexture(GL_TEXTURE_3D, texnum);
	switch (GLImage)
	{
	case IT_GL_Image2D:
		glTexImage2D(Target, Sample, internalFormat, Width, Height, 0, format, type, data);
		break;

	case IT_GL_Image2DMultisample:
		glTexImage2DMultisample(Target, Sample, internalFormat, Width, Height, GL_FALSE);
		break;

	case IT_GL_Image2DCompressed:
		//glCompressedTexImage2D(Target, Sample, internalFormat);
		break;

	case IT_GL_Image1D:
		break;

	case IT_GL_Image3D:
		break;

	default:
		break;
	}
}