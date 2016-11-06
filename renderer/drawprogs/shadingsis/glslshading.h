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

#ifndef __GLSLSHADING_H__
#define __GLSLSHADING_H__

#include "renderer/tr_local.h"
static const GLuint INVALID_PROGID = 0xFFFFFFFF;

typedef enum 
{
	//TODO
	DEF_NO = 0,
	DEF_CORESHADER,
	DEF_USE_FILMIC,
	DEF_USE_REINHARD,
	DEF_USE_TESSELATION,
	DEF_USE_GEOMETRY_SHADER
}ShaderDefines_t;

typedef enum
{
	BUILTIN_NULL = -1,
	BUILTIN_INTERACTION = 0,			//Interation Shader
	BUILTIN_INTERACTION_AMBIENT,		//Ambient Interation Shader
	BUILTIN_SHADOW,						//
	BUILTIN_BUMP_ENVIRONMENT,
	BUILTIN_ENVIRONMENT,
	//BUILTIN_FRAMEBUFFER,			//Shader to FrameBuffer
	BUILTIN_FILMGRAIN,				//Post FX Film Granulation
	BUILTIN_VIGNETE,				//Post FX Screen Edge Darkening
	//BUILTIN_SSAO,					//Screen Space Ambient Oclusion
	//BUILTIN_HDR,					//HDR Post FX Shaders
	//BUILTIN_MATERIAL,				//Custom Material program
	BUILTIN_NUM						//number of built ins shaders
}BuilthinIdent_t;

typedef enum {
	//Textures
	UN_UNIFORM_INVALID = -1,
	UN_NORMAL_TEXTURE = 0,
	UN_DIFFUSE_TEXTURE,
	UN_SPECULAR_TEXTURE,
	UN_AMBIENT_TEXTURE,
	UN_LIGHT_FALLOFF_TEXTURE,
	UN_LIGHT_PROJECTION_TEXTURE,
	UN_ENV_CUBEMAP,

	//Colors
	UN_DIFFUSE_COLOR,
	UN_SPECULAR_COLOR,
	UN_COLOR_MODULATE,
	UN_COLOR_ADD,

	//poits
	UN_LIGHT_ORIGIN,
	UN_VIEW_ORIGIN,
	UN_LIGHT_PROJECT_S,
	UN_LIGHT_PROJECT_T,
	UN_LIGHT_PROJECT_Q,
	UN_LIGHT_FALLOFF_S,

	//Matrixes
	UN_MODEL_MATRIX,
	UN_BUMP_MATRIX_S,
	UN_BUMP_MATRIX_T,
	UN_DIFFUSE_MATRIX_S,
	UN_DIFFUSE_MATRIX_T,
	UN_SPECULAR_MATRIX_S,
	UN_SPECULAR_MATRIX_T,

	//
	UN_SCREENSIZE_WIDH,
	UN_SCREENSIZE_HEIG,
	UN_RANDOM,
	UN_TIMER,
	UN_AMOUNT,

	//imput Values
	UN_RIM_LIGHT_AMOUNT,
	UN_RIM_LIGHT_POWER,

	//user material defined
	UN_UNIFORM_0,
	UN_UNIFORM_1,
	UN_UNIFORM_2,
	UN_UNIFORM_3,
	UN_UNIFORM_4,
	UN_UNIFORM_5,
	UN_UNIFORM_6,
	UN_UNIFORM_7,
	//the limit of users parms are 8
	//but you can add more if change the limit in MAX_GLSL_USER_PARMS, in shader program

	UN_UNIFORM_NUMBER //NEVER use this this
					  //this is set the maximum number of unifors in list by shader
					  //and do not add a uniform iden after this point
	

} programUniform_t;

//Program uniform structure
typedef struct Uniforms_s
{
	GLint				uniform;
	idStr				Name;
	programUniform_t	Ident;

	Uniforms_s()
	{
		this->uniform = INVALID_PROGID;
		this->Name = idStr();
		this->Ident = programUniform_t(-1);
	};

	Uniforms_s(idStr _Name, programUniform_t _Ident)
	{
		this->Name = _Name;
		this->Ident = _Ident;
	};

}Uniforms_t;

//Shader Constructor
class Shader
{
public:
	Shader();//Defalt constructor
	~Shader();

	Shader(const idStr	File, const GLenum type)
	{
		this->FileDir = File;
		this->type = GLenum(type);
	};

	//load  the shader sourceand create the shader
	bool					LoadShader(bool	HasGS = false);
	//get the shader
	GLuint					GetShader()		{ return this->ShaderID; };
	//Get the type of shader program (Vertex, Geometry etc...)
	GLenum					GetShaderType()	{ return this->type; };
	//destroy the shader interation 
	void					ClearShader()	{ glDeleteShader(this->ShaderID); };
	//name the shader
	idStr					name;	

	const GLchar			*SourcePointer;
private:

	GLuint					ShaderID;	//the shader
	GLenum					type;		//store the type of the shader

	idStr					FileDir;	//the shader location

};

//render program constructor
class RenderProgram
{
public:
	RenderProgram(void);
	~RenderProgram();

	//create and name program, and check if are esential render program;
	RenderProgram(const idStr name, BuilthinIdent_t Ident = BUILTIN_NULL, bool Esential = false);

	//Attach Shaders to render program
	void					LinkProgram(Shader* VertexShader, Shader* Fragment);
	//Attach Shaders including geometr shader
	void					LinkProgram(Shader* VertexShader, Shader* Fragment, Shader* Geometry);
	//set the name of program
	void					NameProgram(const char* name){ this->ProgramName = name; };
	//made a check of the program
	bool					ValidateProgram();
	//delete current shader program
	void					DeleteProgram(){ glDeleteProgram(this->programID); };
	//disable the shader program
	void					DisableProgram(){ glDisable(this->programID); }
	//use the shader program
	void					UseProgram(){ glUseProgram(this->programID); };
	//return the program
	GLuint					GetProgram(){ return this->programID; };
	//apend an unifor to program uniform list
	void						appendUniform(const char* name, programUniform_t ident);
	//get the unifor from program list
	GLint					GetUniform(programUniform_t ident);
	//Bind Attrib Locations
	void					SetAttrbLocation(idStr Name, int index){ glBindAttribLocation(this->programID, index, Name.c_str()); }
	//Get a Atrib Location
	GLuint					GetAttrbLocation(idStr Name){ return glGetAttribLocation(this->programID, Name.c_str()); }
	//set a null shader program
	static void				UseNull(){ glUseProgram(0); };//set a null program
	//For Debug List All unifors 
	void					ListUniforms();
	//store the render program name
	idStr					ProgramName;
	//the Vertex, Geometry and vertex shaders indices
	int						VSIndex, GSIndex, FSIndex;
	//the program identation
	BuilthinIdent_t			ident;

	//max number of user defined parms by material
	static const int	MAX_GLSL_USER_PARMS = 8;

private:
	GLuint					programID;//the render program 
	GLuint					VShaderID, FShaderID, GShaderID;

	//Program Uniform List
#if 0
	idStaticList<Uniforms_t*, UN_UNIFORM_NUMBER>	Uniforms;
#else
	idList<Uniforms_t> Uniforms;
#endif

	bool					GeometryShader;
	bool					EsentialProgram;
};
#endif /* !__GLSLSHADING_H__ */
