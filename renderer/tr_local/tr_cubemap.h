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

#ifndef _TR_CUBEMAP_H_
#define _TR_CUBEMAP_H_

#include "../tr_local.h"

class Cubemap
{
public:
	Cubemap();
	~Cubemap();

	//Create a Cubemap interation
	void	Create(bool Dinamic, int width, int height, idVec3 Position, idStr Img = nullptr);

	//bind the cubemap if is dinamic
	void	Bind();

	//check if is a dinamic
	bool	IsDinamic()
	{
		return this->Dinamic;
	}

private:

	//verify the frame buffer status
	void verifyStatus()
	{
		GLenum FB_STATUS = glCheckFramebufferStatus(CmFrameB);

		if (FB_STATUS != GL_FRAMEBUFFER_COMPLETE)
			common->Warning("Can't bind framebuffer: FRAMEBUFFER INCOMPLETE ");
	}

	enum
	{
		POSITIVE_X = 0,
		NEGATIVE_X,
		POSITIVE_Y,
		NEGATIVE_Y,
		POSITIVE_Z,
		NEGATIVE_Z
	};
	
	//the if cubemap are static or dinamic
	bool	Dinamic;

	//the cubemap position on word
	idVec3	Position;

	//the cubemap resolution
	GLint	width, height;

	//the cubemap texture
	GLuint	CubemapTexture;

	//Cubemap Frame And depth buffer
	GLuint	CmFrameB, CmDepthB;

	//set the dinamic cubemap
	void	dinamicCM();
	
	//set the static cubemap
	void	staticCM(idStr Img);


};



#endif _TR_CUBEMAP_H_ //!_TR_CUBEMAP_H_