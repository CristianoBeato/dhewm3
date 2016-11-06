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

#include "draw_glsl.h"

#include "renderer/VertexCache.h"

nmDrawGLSL		DrawGLSL;

idCVar r_useGeometryShader("r_useGeometryShader", "1", CVAR_RENDERER | CVAR_BOOL, "Enable the use of Geometry Shaders");
idCVar r_RimLightIntencity("r_RimLightIntencity", "0.9", CVAR_RENDERER | CVAR_FLOAT, "The Amount of rim light aplied on models");
idCVar r_RimLightPower("r_RimLightPower", "8.0", CVAR_RENDERER | CVAR_FLOAT, "the power of the rim light");

/*
=========================================================================================

GENERAL INTERACTION RENDERING

=========================================================================================
*/
nmDrawGLSL::nmDrawGLSL()
{
	this->Shaders = idList<Shader*>();
	this->glslPrograms = idList<RenderProgram>();
}

nmDrawGLSL::~nmDrawGLSL()
{
	this->KillAllShaders();
	this->glslPrograms.Clear();
	this->Shaders.Clear();
}

static void GL_SelectTextureNoClient( int unit ) 
{
	backEnd.glState.currenttmu = unit;
	glActiveTexture(GL_TEXTURE0 + unit);
	RB_LogComment( "glActiveTexture( %i )\n", unit );
}

void nmDrawGLSL::RB_GLSL_DrawInteraction(const drawInteraction_t *din)
{
	static const float zero[4] = { 0, 0, 0, 0 };
	static const float one[4] = { 1, 1, 1, 1 };
	static const float negOne[4] = { -1, -1, -1, -1 };

	// load all the shader parameters
	for (int i = 0; i < glslPrograms.Num(); ++i)
	{
		if (din->ambientLight)
		{
			if (glslPrograms[i].ident == BUILTIN_INTERACTION_AMBIENT)
			{
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_ORIGIN), 1, din->localLightOrigin.ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_VIEW_ORIGIN), 1, din->localViewOrigin.ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_PROJECT_S), 1, din->lightProjection[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_PROJECT_T), 1, din->lightProjection[1].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_PROJECT_Q), 1, din->lightProjection[2].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_FALLOFF_S), 1, din->lightProjection[3].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_BUMP_MATRIX_S), 1, din->bumpMatrix[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_BUMP_MATRIX_T), 1, din->bumpMatrix[1].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_DIFFUSE_MATRIX_S), 1, din->diffuseMatrix[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_DIFFUSE_MATRIX_T), 1, din->diffuseMatrix[1].ToFloatPtr());
			
				switch (din->vertexColor) 
				{
				case SVC_IGNORE:
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_MODULATE), zero[0], zero[1], zero[2], zero[3]);
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_ADD), one[0], one[1], one[2], one[3]);
					break;
				case SVC_MODULATE:
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_MODULATE), one[0], one[1], one[2], one[3]);
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_ADD), zero[0], zero[1], zero[2], zero[3]);
					break;
				case SVC_INVERSE_MODULATE:
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_MODULATE), negOne[0], negOne[1], negOne[2], negOne[3]);
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_ADD), one[0], one[1], one[2], one[3]);
					break;
				}

				// set the constant color
				glUniform4fv(glslPrograms[i].GetUniform(UN_DIFFUSE_COLOR), 1, din->diffuseColor.ToFloatPtr());
			}

		}
		else
		{
			if (glslPrograms[i].ident == BUILTIN_INTERACTION)
			{
				//the rimLight Amount
				glUniform1f(glslPrograms[i].GetUniform(UN_RIM_LIGHT_AMOUNT), r_RimLightIntencity.GetFloat());
				glUniform1f(glslPrograms[i].GetUniform(UN_RIM_LIGHT_POWER), r_RimLightPower.GetFloat());
	
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_ORIGIN), 1, din->localLightOrigin.ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_VIEW_ORIGIN), 1, din->localViewOrigin.ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_PROJECT_S), 1, din->lightProjection[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_PROJECT_T), 1, din->lightProjection[1].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_PROJECT_Q), 1, din->lightProjection[2].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_LIGHT_FALLOFF_S), 1, din->lightProjection[3].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_BUMP_MATRIX_S), 1, din->bumpMatrix[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_BUMP_MATRIX_T), 1, din->bumpMatrix[1].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_DIFFUSE_MATRIX_S), 1, din->diffuseMatrix[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_DIFFUSE_MATRIX_T), 1, din->diffuseMatrix[1].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_SPECULAR_MATRIX_S), 1, din->specularMatrix[0].ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_SPECULAR_MATRIX_T), 1, din->specularMatrix[1].ToFloatPtr());

				switch (din->vertexColor) 
				{
				case SVC_IGNORE:
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_MODULATE), zero[0], zero[1], zero[2], zero[3]);
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_ADD), one[0], one[1], one[2], one[3]);
					break;
				case SVC_MODULATE:
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_MODULATE), one[0], one[1], one[2], one[3]);
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_ADD), zero[0], zero[1], zero[2], zero[3]);
					break;
				case SVC_INVERSE_MODULATE:
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_MODULATE), negOne[0], negOne[1], negOne[2], negOne[3]);
					glUniform4f(glslPrograms[i].GetUniform(UN_COLOR_ADD), one[0], one[1], one[2], one[3]);
					break;
				}

				// set the constant colors
				glUniform4fv(glslPrograms[i].GetUniform(UN_DIFFUSE_COLOR), 1, din->diffuseColor.ToFloatPtr());
				glUniform4fv(glslPrograms[i].GetUniform(UN_SPECULAR_COLOR), 1, din->specularColor.ToFloatPtr());
			}
		}

		// set the textures

		// texture 0 will be the per-surface bump map
		GL_SelectTextureNoClient(TEXUNIT_NORMAL);
		din->bumpImage->Bind();

		// texture 1 will be the light falloff texture
		GL_SelectTextureNoClient(TEXUNIT_LIGHTFALLOF);
		din->lightFalloffImage->Bind();

		// texture 2 will be the light projection texture
		GL_SelectTextureNoClient(TEXUNIT_LIGHTPOJECTION);
		din->lightImage->Bind();

		// texture 3 is the per-surface diffuse map
		GL_SelectTextureNoClient(TEXUNIT_DIFUSE);
		din->diffuseImage->Bind();

		//Ambient Light Dont cast specular
		if (!din->ambientLight)
		{
			// texture 4 is the per-surface specular map
			GL_SelectTextureNoClient(TEXUNIT_SPECULAR);
			din->specularImage->Bind();
		}
		else
		{
#if 0
			// texture 4 is a ambient cubemap
			GL_SelectTextureNoClient(TEXUNIT_MATERIAL);
			din->materialMask->Bind();
#endif
		}

		// draw it
		RB_DrawElementsWithCounters(din->surf->geo);
	}
}

static void DrawInteraction(const drawInteraction_t *din)
{
	DrawGLSL.RB_GLSL_DrawInteraction(din);
}

void nmDrawGLSL::RB_GLSL_CreateDrawInteractions(const drawSurf_t *surf) 
{
	if (!surf)
	{
		return;
	}

	// perform setup here that will be constant for all interactions
	GL_State(GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc);
	
	// bind the vertex and fragment program
	for (int i = 0; i < glslPrograms.Num(); ++i)
	{
		// bind the vertex and fragment program
		if (backEnd.vLight->lightShader->IsAmbientLight())
		{
			if (glslPrograms[i].ident == BUILTIN_INTERACTION_AMBIENT)
				glslPrograms[i].UseProgram();//run the program 
		}
		else
		{
			if (glslPrograms[i].ident == BUILTIN_INTERACTION)
				glslPrograms[i].UseProgram();//run the progam
		}

		if (glslPrograms[i].ident == BUILTIN_INTERACTION || glslPrograms[i].ident == BUILTIN_INTERACTION_AMBIENT)
		{
			glslPrograms[i].SetAttrbLocation("attr_Normal", ATTRB_NORMAL);
			glslPrograms[i].SetAttrbLocation("attr_TexCoord", ATTRB_TEXTCOORD);
			glslPrograms[i].SetAttrbLocation("attr_Tangent", ATTRB_TANGENT);
			glslPrograms[i].SetAttrbLocation("attr_Bitangent", ATTRB_BITANGENT);
		}
	}


	// enable the vertex arrays
	glEnableVertexAttribArray(ATTRB_NORMAL);
	glEnableVertexAttribArray(ATTRB_TEXTCOORD);
	glEnableVertexAttribArray(ATTRB_TANGENT);
	glEnableVertexAttribArray(ATTRB_BITANGENT);

	glEnableClientState(GL_COLOR_ARRAY);

#if TODO
	// texture 0 is the normalization cube map for the vector towards the light
	GL_SelectTextureNoClient(0);
	if (backEnd.vLight->lightShader->IsAmbientLight()) {
		globalImages->ambientNormalMap->Bind();
	}
	else {
		globalImages->normalCubeMapImage->Bind();
	}

	// texture 6 is the specular lookup table
	GL_SelectTextureNoClient(6);
	if (r_testARBProgram.GetBool()) {
		globalImages->specular2DTableImage->Bind();	// variable specularity in alpha channel
	}
	else {
		globalImages->specularTableImage->Bind();
	}
#endif

	// perform setup here that will not change over multiple interaction passes
	for (; surf; surf = surf->nextOnLight)
	{
		// set the vertex pointers
		idDrawVert	*ac = (idDrawVert *)vertexCache.Position(surf->geo->ambientCache);

		//pointer to the vertex color
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(idDrawVert), ac->color);
		
		//pointer to thevertex position
		glVertexPointer(3, GL_FLOAT, sizeof(idDrawVert), ac->xyz.ToFloatPtr());

		glVertexAttribPointerARB(ATTRB_NORMAL, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->normal.ToFloatPtr());
		glVertexAttribPointerARB(ATTRB_TANGENT, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->tangents[1].ToFloatPtr());
		glVertexAttribPointerARB(ATTRB_BITANGENT, 3, GL_FLOAT, false, sizeof(idDrawVert), ac->tangents[0].ToFloatPtr());
		glVertexAttribPointerARB(ATTRB_TEXTCOORD, 2, GL_FLOAT, false, sizeof(idDrawVert), ac->st.ToFloatPtr());

		for (int i = 0; i < glslPrograms.Num(); ++i)
		{
			// set model matrix
			if (backEnd.vLight->lightShader->IsAmbientLight())
				if (glslPrograms[i].ident == BUILTIN_INTERACTION_AMBIENT)
					glUniformMatrix4fv(glslPrograms[i].GetUniform(UN_MODEL_MATRIX), 1, false, surf->space->modelMatrix);
			else
				if (glslPrograms[i].ident == BUILTIN_INTERACTION)
					glUniformMatrix4fv(glslPrograms[i].GetUniform(UN_MODEL_MATRIX), 1, false, surf->space->modelMatrix);
		}
		// this may cause RB_GLSL_DrawInteraction to be executed multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_CreateSingleDrawInteractions(surf, DrawInteraction);
	}

	glDisableVertexAttribArrayARB(ATTRB_NORMAL);
	glDisableVertexAttribArrayARB(ATTRB_TEXTCOORD);
	glDisableVertexAttribArrayARB(ATTRB_TANGENT);
	glDisableVertexAttribArrayARB(ATTRB_BITANGENT);

	glDisableClientState(GL_COLOR_ARRAY);

	//set null textures
	GL_SelectTextureNoClient(TEXUNIT_MATERIAL);
	globalImages->BindNull();


	GL_SelectTextureNoClient(TEXUNIT_SPECULAR);
	globalImages->BindNull();

	GL_SelectTextureNoClient(TEXUNIT_DIFUSE);
	globalImages->BindNull();

	GL_SelectTextureNoClient(TEXUNIT_LIGHTPOJECTION);
	globalImages->BindNull();

	GL_SelectTextureNoClient(TEXUNIT_LIGHTFALLOF);
	globalImages->BindNull();

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture(TEXUNIT_NORMAL);
	
	for (int i = 0; i < glslPrograms.Num(); ++i)
	{
		if (glslPrograms[i].ident == BUILTIN_INTERACTION || glslPrograms[i].ident == BUILTIN_INTERACTION_AMBIENT)
			glslPrograms[i].DisableProgram();
	}
	RenderProgram::UseNull();
}

void nmDrawGLSL::RB_GLSL_DrawInteractions(void) {
	viewLight_t		*vLight;

	GL_SelectTexture( 0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	//
	// for each light, perform adding and shadowing
	//
	for ( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next ) {
		backEnd.vLight = vLight;

		// do fogging later
		if ( vLight->lightShader->IsFogLight() ) 
		{
			continue;
		}

		if ( vLight->lightShader->IsBlendLight() )
		{
			continue;
		}

		// if there are no interactions, get out!
		if ( !vLight->localInteractions && !vLight->globalInteractions && !vLight->translucentInteractions )
		{
			continue;
		}

		// clear the stencil buffer if needed
		if ( vLight->globalShadows || vLight->localShadows ) 
		{
			backEnd.currentScissor = vLight->scissorRect;
			if ( r_useScissor.GetBool() ) {
				glScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
					backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
					backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
					backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}
			glClear( GL_STENCIL_BUFFER_BIT );
		} 
		else 
		{
			// no shadows, so no need to read or write the stencil buffer
			// we might in theory want to use GL_ALWAYS instead of disabling
			// completely, to satisfy the invarience rules
			glStencilFunc( GL_ALWAYS, 128, 255 );
		}

		if ( r_useShadowVertexProgram.GetBool() ) 
		{

			for (int i = 0; i < glslPrograms.Num(); ++i)
			{
				if (glslPrograms[i].ident == BUILTIN_SHADOW)
				{
					//glUseProgram(glslPrograms[i].program);
					glslPrograms[i].UseProgram();

					RB_StencilShadowPass(vLight->globalShadows);
					this->RB_GLSL_CreateDrawInteractions(vLight->localInteractions);

					//glUseProgram(glslPrograms[i].program);
					glslPrograms[i].UseProgram();
					RB_StencilShadowPass(vLight->localShadows);
					this->RB_GLSL_CreateDrawInteractions(vLight->globalInteractions);

					//glUseProgram(0);	// if there weren't any globalInteractions, it would have stayed on
					RenderProgram::UseNull();
				}
			}
		}
		else
		{
			RB_StencilShadowPass( vLight->globalShadows );
			this->RB_GLSL_CreateDrawInteractions( vLight->localInteractions );

			RB_StencilShadowPass( vLight->localShadows );
			this->RB_GLSL_CreateDrawInteractions( vLight->globalInteractions );
		}

		// translucent surfaces never get stencil shadowed
		if ( r_skipTranslucent.GetBool() ) 
		{
			continue;
		}

		glStencilFunc( GL_ALWAYS, 128, 255 );

		backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
		this->RB_GLSL_CreateDrawInteractions( vLight->translucentInteractions );
		backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
	}

	// disable stencil shadow test
	glStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

void nmDrawGLSL::RB_GLSL_DrawShadownInteration(int i, idVec4 LightOrigin)
{
	for (int idx = 0; idx < glslPrograms.Num(); ++idx)
	{
		if (glslPrograms[idx].ident == BUILTIN_SHADOW)
		{
			glUniform4fv(glslPrograms[idx].GetUniform(UN_LIGHT_ORIGIN), i, LightOrigin.ToFloatPtr());
		}
	}
}

//===================================================================================\\

void nmDrawGLSL::R_SetUniform(BuilthinIdent_t programIdent, int textureEnum, const char* Bultinname)
{
	for (int i = 0; i < glslPrograms.Num(); i++)
	{
		if (glslPrograms[i].ident == programIdent)
		{
			glslPrograms[i].appendUniform(Bultinname, programUniform_t(UN_UNIFORM_0 + textureEnum));
			glUniform1i(glslPrograms[i].GetUniform(programUniform_t(UN_UNIFORM_0 + textureEnum)), textureEnum);
		}
	}
}

void nmDrawGLSL::R_LoadBuiltinsPrograms()
{
	typedef struct builtinShaders_s //The Shader Structure
	{
		idStr					name;	//the program name (name used to load the programs files)
		idStr					VertexShader;
		idStr					FragmentShader;
		BuilthinIdent_t			ident;
		bool					hasGS;	//if have or not Geometry Shader

	}builtinShaders_t;

	const char * Vsulfix;
	const char * Fsulfix;
	const char * Gsulfix = char();
	bool	GShader = true;

	if (idStr::Icmp(r_GLSLVer.GetString(), "GLSL4") == 0)
	{
		Vsulfix = "_400.vs";
		Fsulfix = "_400.fs";
		Gsulfix = "_400.gs";
	}
	else if (idStr::Icmp(r_GLSLVer.GetString(), "GLSL3") == 0)
	{
		Vsulfix = "_300.vs";
		Fsulfix = "_300.fs";
		Gsulfix = "_300.gs";
	}
	else //GLSL 1.20
	{
		//GLSL 1.20 dont suport geometry shaders
		GShader = false;
		Vsulfix = "_120.vs";
		Fsulfix = "_120.fs";
	}

	builtinShaders_t	builtins[] =
	{
		//Main Light Interation
		{ "interaction",		"interaction",			"PBR_interaction",	BUILTIN_INTERACTION, false },
		//{ "interaction", "interaction", "interaction", BUILTIN_INTERACTION, false },
		//Main Ambient Light Interation use the same vertex program as  interaction
		{ "ambientInteraction", "interaction",			"ambientInteraction", BUILTIN_INTERACTION_AMBIENT, false },
		//stencil shadow
		{ "stencilshadow",		"stencilshadow",		"stencilshadow", BUILTIN_SHADOW },//Stencil Shadow
		{ "environment",		"environment",			"environment", BUILTIN_ENVIRONMENT },//Ambient enviroment interation
		{ "bumpyenvironment",	"bumpyenvironment",		"bumpyenvironment", BUILTIN_BUMP_ENVIRONMENT },
		{ "filmgrain",			"filmgrain",			"filmgrain", BUILTIN_FILMGRAIN },
		{ "vignete",			"vignete",				"vignete", BUILTIN_VIGNETE }
	};

	//Create the Built ins shader programs
	int numBuiltins = sizeof(builtins) / sizeof(builtins[0]);
	for (int i = 0; i < numBuiltins; i++)
	{
		/*			the shaders			*/
		uint GeomIdx = uint();

		//load new Vertex shader
		uint VerIdx = this->R_FindGLSLShader(idStr(builtins[i].VertexShader + Vsulfix), GL_VERTEX_SHADER);

		//load the geometry shader
		if (builtins[i].hasGS  && r_useGeometryShader.GetBool())
			GeomIdx = this->R_FindGLSLShader(idStr(builtins[i].name + Gsulfix), GL_GEOMETRY_SHADER);

		//load new fragment shade
		uint FramIdx = this->R_FindGLSLShader(idStr(builtins[i].FragmentShader + Fsulfix), GL_FRAGMENT_SHADER);

		/*		the shader program		*/
		//create a new program in the list
		uint ProgramIDX = glslPrograms.Append(RenderProgram(builtins[i].name, builtins[i].ident, true));

		//bind the shaders to the main program
		if (builtins[i].hasGS && r_useGeometryShader.GetBool() == true)
			glslPrograms[ProgramIDX].LinkProgram(Shaders[VerIdx], Shaders[FramIdx], Shaders[GeomIdx]);
		else
			glslPrograms[ProgramIDX].LinkProgram(Shaders[VerIdx], Shaders[FramIdx]);

		//add the program unifroms location
		this->R_LoadProgramUniforms(glslPrograms[ProgramIDX]);

		//save the shades indexes reload shader program later
		glslPrograms[ProgramIDX].VSIndex = VerIdx;
		if (builtins[i].hasGS && r_useGeometryShader.GetBool() == true)
			glslPrograms[ProgramIDX].GSIndex = GeomIdx;
		glslPrograms[ProgramIDX].FSIndex = FramIdx;

		//now clear, but keep source dir, if need reload later
		Shaders[VerIdx]->ClearShader();
		Shaders[FramIdx]->ClearShader();
		if (builtins[i].hasGS && r_useGeometryShader.GetBool() == true)
			Shaders[GeomIdx]->ClearShader();

	}
}

void nmDrawGLSL::R_LoadProgramUniforms(RenderProgram &RenderProgram)
{
	//check the program
	if (!RenderProgram.ValidateProgram())
	{
		return;
	}
	// If whe dont have any error set the uniforms locations
	else
	{
		if (RenderProgram.ident == BUILTIN_INTERACTION)
		{
			//Append  uniforms to program
			RenderProgram.appendUniform("u_modelMatrix", UN_MODEL_MATRIX);
			RenderProgram.appendUniform("u_lightOrigin", UN_LIGHT_ORIGIN);
			RenderProgram.appendUniform("u_viewOrigin", UN_VIEW_ORIGIN);

			RenderProgram.appendUniform("u_lightProjectionS", UN_LIGHT_PROJECT_S);
			RenderProgram.appendUniform("u_lightProjectionT", UN_LIGHT_PROJECT_T);
			RenderProgram.appendUniform("u_lightProjectionQ", UN_LIGHT_PROJECT_Q);
			RenderProgram.appendUniform("u_lightFalloff", UN_LIGHT_FALLOFF_S);

			RenderProgram.appendUniform("u_bumpMatrixS", UN_BUMP_MATRIX_S);
			RenderProgram.appendUniform("u_bumpMatrixT", UN_BUMP_MATRIX_T);
			RenderProgram.appendUniform("u_diffuseMatrixS", UN_DIFFUSE_MATRIX_S);
			RenderProgram.appendUniform("u_diffuseMatrixT", UN_DIFFUSE_MATRIX_T);
			RenderProgram.appendUniform("u_specularMatrixS", UN_SPECULAR_MATRIX_S);
			RenderProgram.appendUniform("u_specularMatrixT", UN_SPECULAR_MATRIX_T);

			RenderProgram.appendUniform("u_colorModulate", UN_COLOR_MODULATE);
			RenderProgram.appendUniform("u_colorAdd", UN_COLOR_ADD);
			RenderProgram.appendUniform("u_diffuseColor", UN_DIFFUSE_COLOR);
			RenderProgram.appendUniform("u_specularColor", UN_SPECULAR_COLOR);

			RenderProgram.appendUniform("u_rimlightAmout", UN_RIM_LIGHT_AMOUNT);
			RenderProgram.appendUniform("u_rimlightPower", UN_RIM_LIGHT_POWER);

			// set texture locations
			RenderProgram.UseProgram();// use current program
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_normalTexture"),			TEXUNIT_NORMAL);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_lightFalloffTexture"),	TEXUNIT_LIGHTFALLOF);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_lightProjectionTexture"), TEXUNIT_LIGHTPOJECTION);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_diffuseTexture"), TEXUNIT_DIFUSE);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_specularTexture"), TEXUNIT_SPECULAR);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_materialMask"), TEXUNIT_MATERIAL);

			RenderProgram::UseNull();//Return to null program
		}
		else if (RenderProgram.ident == BUILTIN_INTERACTION_AMBIENT)
		{
			//Append  uniforms to program
			RenderProgram.appendUniform("u_modelMatrix", UN_MODEL_MATRIX);

			RenderProgram.appendUniform("u_lightOrigin", UN_LIGHT_ORIGIN);
			RenderProgram.appendUniform("u_viewOrigin", UN_VIEW_ORIGIN);
			RenderProgram.appendUniform("u_lightProjectionS", UN_LIGHT_PROJECT_S);
			RenderProgram.appendUniform("u_lightProjectionT", UN_LIGHT_PROJECT_T);
			RenderProgram.appendUniform("u_lightProjectionQ", UN_LIGHT_PROJECT_Q);
			RenderProgram.appendUniform("u_lightFalloff", UN_LIGHT_FALLOFF_S);

			RenderProgram.appendUniform("u_bumpMatrixS", UN_BUMP_MATRIX_S);
			RenderProgram.appendUniform("u_bumpMatrixT", UN_BUMP_MATRIX_T);
			RenderProgram.appendUniform("u_diffuseMatrixS", UN_DIFFUSE_MATRIX_S);
			RenderProgram.appendUniform("u_diffuseMatrixT", UN_DIFFUSE_MATRIX_T);

			RenderProgram.appendUniform("u_colorModulate", UN_COLOR_MODULATE);
			RenderProgram.appendUniform("u_colorAdd", UN_COLOR_ADD);
			RenderProgram.appendUniform("u_diffuseColor", UN_DIFFUSE_COLOR);

			// set texture locations
			RenderProgram.UseProgram();// use current program

			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_normalTexture"), TEXUNIT_NORMAL);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_lightFalloffTexture"), TEXUNIT_LIGHTFALLOF);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_lightProjectionTexture"), TEXUNIT_LIGHTPOJECTION);
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "u_diffuseTexture"), TEXUNIT_DIFUSE);
			RenderProgram::UseNull();//Return to null program
		}
		else if (RenderProgram.ident == BUILTIN_SHADOW)
		{
			RenderProgram.appendUniform("u_lightOrigin", UN_LIGHT_ORIGIN);//shadons have only this uniform, wel, add them
		}
		else if (RenderProgram.ident == BUILTIN_ENVIRONMENT)
		{
			RenderProgram.appendUniform("u_EnvCubemap", UN_ENV_CUBEMAP);//shadons have only this uniform, wel, add them

			RenderProgram.UseProgram();// use current program
			glUniform1i(RenderProgram.GetUniform(UN_ENV_CUBEMAP), 0);
			RenderProgram::UseNull();
		}
		else if (RenderProgram.ident == BUILTIN_BUMP_ENVIRONMENT)
		{
			RenderProgram.appendUniform("u_EnvCubemap", UN_ENV_CUBEMAP);
			RenderProgram.appendUniform("u_normalTexture", UN_NORMAL_TEXTURE);
			RenderProgram.appendUniform("u_viewOrigin", UN_VIEW_ORIGIN);

			RenderProgram.UseProgram();// use current program
			glUniform1i(RenderProgram.GetUniform(UN_NORMAL_TEXTURE), TEXUNIT_NORMAL);
			glUniform1i(RenderProgram.GetUniform(UN_ENV_CUBEMAP), 1);
			RenderProgram::UseNull();
		}
		else if (RenderProgram.ident == BUILTIN_FILMGRAIN)
		{
			RenderProgram.appendUniform("RenderedTextureWidth", UN_SCREENSIZE_WIDH);
			RenderProgram.appendUniform("RenderedTextureHeight", UN_SCREENSIZE_HEIG);
			RenderProgram.appendUniform("timer", UN_TIMER);

			RenderProgram.UseProgram();// use current program
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "RenderedTexture"), 0);
			RenderProgram::UseNull();
		}
		else if (RenderProgram.ident == BUILTIN_VIGNETE)
		{
			RenderProgram.UseProgram();// use current program
			glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "RenderTexture0"), 0);
			//glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "RenderTexture0"), TEXUNIT_LIGHTFALLOF);
			//glUniform1i(glGetUniformLocation(RenderProgram.GetProgram(), "RenderSpecular0"), TEXUNIT_LIGHTPOJECTION);
			RenderProgram::UseNull();
		}
	}

}

void nmDrawGLSL::R_DrawShader(BuilthinIdent_t programIdent)
{
	for (int i = 0; i < glslPrograms.Num(); i++)
	{
		if (glslPrograms[i].ident == programIdent)
		{
			glslPrograms[i].UseProgram();
		}
	}
}

void nmDrawGLSL::R_DeleteShader(BuilthinIdent_t programIdent)
{
	for (int i = 0; i < glslPrograms.Num(); i++)
	{
		if (glslPrograms[i].ident == programIdent)
		{
			glslPrograms[i].DeleteProgram();
		}
	}
}

GLuint nmDrawGLSL::R_GetShaderProgram(BuilthinIdent_t programIdent)
{
	//search program
	for (int i = 0; i < glslPrograms.Num(); i++)
	{
		if (glslPrograms[i].ident == programIdent)
		{
			return glslPrograms[i].GetProgram();
		}
	}

	//if Program not found return a null
	return INVALID_PROGID;
}

RenderProgram	nmDrawGLSL::R_GetRenderProgram(BuilthinIdent_t programIdent)
{ 
	for (int i = 0; i < glslPrograms.Num(); i++)
	{
		if (glslPrograms[i].ident == programIdent)
		{
			return glslPrograms[i];
		}
	}
	return RenderProgram(0); //return a null program
}

int nmDrawGLSL::R_FindGLSLShader(const char *name, GLenum target)
{
	int index = 0;

	//if not are GL_VERTEX_SHADER or GL_GEOMETRY_SHADER or GL_FRAGMENT_SHADER whe have a  error
	if (target != GL_VERTEX_SHADER && target != GL_GEOMETRY_SHADER && target != GL_FRAGMENT_SHADER)
		common->Error("on find shader: %s, No Vertex, Geometry or Fragment sader set\n", name);

	//search in the list, if shader already loaded
	for (int i = 0; i < Shaders.Num(); i++)
	{
		if (Shaders[i]->name.Icmp(name) == 0)
			return i;
	}

	//create a vertex shader
	Shader* shader = new Shader(idStr(name), target);
	shader->name = name;
	shader->LoadShader();

	//append to shader list 
	index = Shaders.Append(shader);

	return index;
}

int nmDrawGLSL::R_FindGLSLProgram(const char* name, int vIndex, int fIndex, int gIndex)
{
	//if program is loaded find it
	for (int i = 0; i < glslPrograms.Num(); ++i)
	{
		if ((glslPrograms[i].FSIndex == vIndex) && (glslPrograms[i].VSIndex == fIndex))
		{
			//Reload shaders
			Shaders[vIndex]->LoadShader();
			Shaders[fIndex]->LoadShader();

			//reload program
			glslPrograms[i].LinkProgram(Shaders[vIndex], Shaders[fIndex]);

			//clear shader but keep the name and the source dir to reload
			Shaders[vIndex]->ClearShader();
			Shaders[fIndex]->ClearShader();

			return i;
		}
	}

	//create a new program and apendo to the list
	RenderProgram program = RenderProgram(name);

	//we naver gona have an Geometry shader whit index 0, so dont use it
	if (gIndex != 0)
		program.LinkProgram(Shaders[vIndex], Shaders[fIndex], Shaders[gIndex]);
	else
		program.LinkProgram(Shaders[vIndex], Shaders[fIndex]);

	//apend program to the list an get the index
	int index = glslPrograms.Append(program);

	//clear shader but keep data to reload
	Shaders[vIndex]->ClearShader();
	Shaders[fIndex]->ClearShader();
	if (gIndex != 0)
		Shaders[gIndex]->ClearShader();

	return index;
}

void nmDrawGLSL::R_ReloadGLSLPrograms_f() 
{
	//TODO: FIX SHADER RELOAD
	DrawGLSL.KillAllShaders();

	common->Printf("----- R_ReloadGLSLPrograms -----\n");
	
	for (uint i = 0; glslPrograms.Num(); i++)
	{
		common->Printf("Reloading Shader program: s% \n", glslPrograms[i].ProgramName);
		if (glslPrograms[i].GSIndex != 0 && r_useGeometryShader.GetBool() == true)
		{
			//reload shaders
			Shaders[glslPrograms[i].VSIndex]->LoadShader();
			Shaders[glslPrograms[i].FSIndex]->LoadShader();
			Shaders[glslPrograms[i].GSIndex]->LoadShader();

			//linkin shaders
			glslPrograms[i].LinkProgram(Shaders[glslPrograms[i].VSIndex],
										Shaders[glslPrograms[i].FSIndex],
										Shaders[glslPrograms[i].GSIndex]);

			//Clear shaders
			Shaders[glslPrograms[i].VSIndex]->ClearShader();
			Shaders[glslPrograms[i].FSIndex]->ClearShader();
			Shaders[glslPrograms[i].GSIndex]->ClearShader();
		}
		else
		{
			//reload shaders
			Shaders[glslPrograms[i].VSIndex]->LoadShader();
			Shaders[glslPrograms[i].FSIndex]->LoadShader();

			//linking shaders
			glslPrograms[i].LinkProgram(Shaders[glslPrograms[i].VSIndex],
										Shaders[glslPrograms[i].FSIndex]);

			//Clear shaders
			Shaders[glslPrograms[i].VSIndex]->ClearShader();
			Shaders[glslPrograms[i].FSIndex]->ClearShader();
		}
	}
}

void nmDrawGLSL::R_listProgram(const idCmdArgs &args)
{
	common->Printf("----- R_listProgram -----\n");
	for (int i = 0; i < glslPrograms.Num(); ++i)
	{
		common->Printf("Program: %s, Index: %d\n", glslPrograms[i].ProgramName.c_str(), i);
		glslPrograms[i].ListUniforms();
		common->Printf("\n");
	}
}

void nmDrawGLSL::KillAllShaders()
{
	//Clear shader, but keep names
	for (int i = 0; i < Shaders.Num(); ++i)
	{
		Shaders[i]->ClearShader();
	}

	//clear shader  programs but we keep, the shader index to reload
	for (int i = 0; i < glslPrograms.Num(); ++i)
	{
		glslPrograms[i].DeleteProgram();
	}
	glslPrograms.Clear();
}

static void R_ReloadShaders(const idCmdArgs& args)
{
	DrawGLSL.R_ReloadGLSLPrograms_f();
}

static void RelistProgram(const idCmdArgs& args)
{
	DrawGLSL.R_listProgram(args);
}

void nmDrawGLSL::R_GLSL_Init(void) 
{
	glConfig.allowGLSLPath = false;
	common->Printf( "---------- R_GLSL_Init -----------\n" );

	if ( !glConfig.GLSLAvailable ) 
	{
		common->Printf( "Not available.\n" );
		return;
	} 

	//load all programs light an post render programs
	this->R_LoadBuiltinsPrograms();

	common->Printf( "Available.\n" );
	cmdSystem->AddCommand("R_listProgram", RelistProgram, CMD_FL_RENDERER, "List GLSL programs");
	


	//and reload them when typed on console
	cmdSystem->AddCommand("reloadGLSLprograms", R_ReloadShaders, CMD_FL_RENDERER, "reloads GLSL programs");
	common->Printf( "---------------------------------\n" );

	glConfig.allowGLSLPath = true;
}