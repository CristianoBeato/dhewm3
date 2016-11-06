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

#ifndef __DRAW_GLSL_H__
#define __DRAW_GLSL_H__

#include "renderer/tr_local.h"
#include "renderer/drawprogs/shadingsis/glslshading.h"
#include "idlib/containers/nmMap.h"

typedef enum
{
	GL_VER_40, //GLSL Version 4.00 (For Tesselation Shader, and more news infuture)
	GL_VER_30, //GLSL Version 3.00
	GL_VER_12  //GLSL Version 1.20 (compatibility)
}GLVer_t;

class nmDrawGLSL
{
public:
	nmDrawGLSL();
	~nmDrawGLSL();

	//init the GLSL shader system and load the built ins shadder
	void		R_GLSL_Init(void);//Init GLSL Shading
	//set the liht interations
	void		RB_GLSL_DrawInteractions(void);
	//draw the shadonw interation shader
	void		RB_GLSL_DrawShadownInteration(int i, idVec4 LightOrigin);//Draw the specific shadow Interation
	//draw light shader interaction
	void		RB_GLSL_DrawInteraction(const drawInteraction_t *din);//Draw Shader by Shader interation 
	//reload al shader programs
	void		R_ReloadGLSLPrograms_f();
	//print a list whit all loaded programs 
	void			R_listProgram(const idCmdArgs &args);//Print the shader programs
	//clear shader program but keep name and shaders indexes to reload
	void			KillAllShaders();
	//search for a shader and if not found create a new
	int				R_FindGLSLShader(const char *name, GLenum target);
	//try find for a render program,
	int				R_FindGLSLProgram(const char* name, int vIndex, int fIndex, int gIndex = 0);
	//draw the selected shader by builtin
	void			R_DrawShader(BuilthinIdent_t programIdent);
	//delete shader from the list
	void			R_DeleteShader(BuilthinIdent_t programIdent);
	//get shader program 
	GLuint			R_GetShaderProgram(BuilthinIdent_t programIdent);
	//get the render program by identation
	RenderProgram	R_GetRenderProgram(BuilthinIdent_t programIdent);
	//add new uniform
	void		R_SetUniform(BuilthinIdent_t programIdent, int textureEnum, const char* Bultinname);

private:

	enum Unit
	{
		//Texture location
		TEXUNIT_NORMAL = 0,
		TEXUNIT_LIGHTFALLOF,
		TEXUNIT_LIGHTPOJECTION,
		TEXUNIT_DIFUSE,
		TEXUNIT_SPECULAR,
		TEXUNIT_MATERIAL,
		TEXUNIT_AMBIENT,

		//Attrib Location
		ATTRB_NORMAL = 1,
		ATTRB_TEXTCOORD,
		ATTRB_TANGENT,
		ATTRB_BITANGENT
	};

	//Store the shaders to bind a render program
	idList<Shader*>				Shaders;

	//the shaders programs list
	idList<RenderProgram>		glslPrograms;

	//load built ins render prorams
	void		R_LoadBuiltinsPrograms();

	//set the built ins programs uniforms
	void		R_LoadProgramUniforms(RenderProgram &RenderProgram);

	//create shader by shader Iteration
	void		RB_GLSL_CreateDrawInteractions(const drawSurf_t *surf);


};
extern nmDrawGLSL		DrawGLSL;		//global pointer to GLSL render
//NMS End

#endif /* !__DRAW_GLSL_H__ */
