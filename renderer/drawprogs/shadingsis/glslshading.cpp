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

#include "glslshading.h"
#include "shader_Headers.h"
#include "renderer/tr_local.h"

//#define EASY_SHADER_DEBUG

void ShaderErrorHadler(GLuint Shader, idStr ShaderSource, GLint &compiled)
{
	//Error Handler
#if 0
	GLint compiled = GL_FALSE;
#endif

	int infologLength = 0;

	glGetShaderiv(Shader, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &infologLength);

#ifdef EASY_SHADER_DEBUG
	if (compiled == GL_FALSE)
	{
		glDeleteShader(Shader);
		common->Warning("Compile Shader ERROR:\n");
	}

	if (infologLength > 0){
		idTempArray<char> ShaderErrorMessage(infologLength + 1);
		glGetShaderInfoLog(Shader, infologLength, NULL, &ShaderErrorMessage[0]);
		common->Warning("%s\n", &ShaderErrorMessage[0]);
}

#else
	if (infologLength > 1)
	{

		idTempArray<char> infoLog(infologLength);
		int charsWritten = 0;
		glGetShaderInfoLog(Shader, infologLength, &charsWritten, infoLog.Ptr());
		if (r_displayGLSLCompilerMessages.GetBool())
		{
			const char separator = '\n';
			idList<idStr> lines;
			lines.Clear();
			idStr source(ShaderSource);
			lines.Append(source);
			for (int index = 0, ofs = lines[index].Find(separator); ofs != -1; index++, ofs = lines[index].Find(separator))
			{
				lines.Append(lines[index].c_str() + ofs + 1);
				lines[index].CapLength(ofs);
			}
			common->Printf("-----------------\n");
			for (int i = 0; i < lines.Num(); i++)
			{
				common->Printf("%3d: %s\n", i + 1, lines[i].c_str());
			}
			common->Printf("-----------------\n");
			common->Printf("%s\n", infoLog.Ptr());
		}
	}
#endif
}

void ShaderProgramErrorHandler(GLuint Shader, const char* ShaderName)
{
	GLint Result = GL_FALSE;
	int InfoLogLength = 0;

	// Check the program
	glGetProgramiv(Shader, GL_LINK_STATUS, &Result);
	glGetProgramiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);

#ifdef EASY_SHADER_DEBUG
	if (InfoLogLength > 0){
		idTempArray<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(Shader, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		common->Printf("\n%s\n", &ProgramErrorMessage[0]);
	}
#else
	glGetProgramiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 1)
	{
		char* infoLog = (char*)malloc(InfoLogLength);
		int charsWritten = 0;
		glGetProgramInfoLog(Shader, InfoLogLength, &charsWritten, infoLog);
		// catch the strings the ATI and Intel drivers output on success
		if (strstr(infoLog, "Vertex shader(s) linked, fragment shader(s) linked.") != NULL || strstr(infoLog, "No errors.") != NULL)
		{
			//common->Printf("render prog %s from %s linked\n", GetName(), GetFileName());
		}
		else
		{
			common->Printf("While linking GLSL program %s\n", ShaderName);
			common->Printf("%s\n", infoLog);
		}

		free(infoLog);
	}
#endif
}

idStr SetShaderHeader(const idStr shaderSource, GLenum shaderType, ShaderDefines_t HaderDefines = DEF_NO)
{
	idStr SOURCE;
	SOURCE.ReAllocate(shaderSource.Length() * 2, false);

	//Set the shader source Header
	if (idStr::Icmp(r_GLSLVer.GetString(), "GLSL4") == 0)
	{
		//use GLSL 4.0 CORE shaders
		if (HaderDefines == DEF_CORESHADER)
			SOURCE += GLSL_4_0_CORE;
		else
			SOURCE += GLSL_4_0;//use GLSL 4.0 compatibilty mode
	}
	else if (idStr::Icmp(r_GLSLVer.GetString(), "GLSL3") == 0)
	{
		//use GLSL 3.3 CORE shaders
		if (HaderDefines == DEF_CORESHADER)
			SOURCE += GLSL_3_3_CORE;
		else
			SOURCE += GLSL_3_3;//use GLSL 3.3 compatibilty mode
	}
	else //GLSL 1.20
	{
		SOURCE += GLSL_1_2;
	}

	switch (HaderDefines)
	{
	//Use filmic tonemaping on postFX shaders
	case DEF_USE_FILMIC:
		if (shaderType == GL_FRAGMENT_SHADER)//set only in fragment shader
			SOURCE += DEFINE_USE_FILMIC;
		break;

	//Use reinnhard  tonemaping on post FX shader
	case DEF_USE_REINHARD:
		if (shaderType == GL_FRAGMENT_SHADER)//set only in Fragment Shader
			SOURCE += DEFINE_USE_FILMIC;
		break;

	//Enable use of mesh Tesselation
	case DEF_USE_TESSELATION:
		if (shaderType == GL_VERTEX_SHADER || shaderType == GL_GEOMETRY_SHADER)//set only vertex and geometry shaders
			SOURCE += DEFINE_USE_TESSELATE;
		break;

	default:
		break;
	}

	//Half Lambert Shading
	if (r_useHalfLambert.GetBool())
		if (shaderType == GL_FRAGMENT_SHADER)//works only in Fragment Shaders 
			SOURCE += DEFINE_USE_HALFLAMBERT;

	//Rim Light suport
	if (r_useRimLight.GetBool())
		if (shaderType == GL_FRAGMENT_SHADER)
			SOURCE += DEFINE_USE_RIMLIGHT;

	if (HaderDefines == DEF_USE_GEOMETRY_SHADER)
		if (shaderType == GL_VERTEX_SHADER || GL_FRAGMENT_SHADER)
			SOURCE += DEFINE_USE_GS;

	//now set the rest of the source fom file
	SOURCE += shaderSource;

	return SOURCE;
}

bool LoadShaderSource(const char *name, idStr &buffer)
{
	idStr	fullPath;

	//GLSL 4 Shaders Have your onw dir
	if (idStr::Icmp(r_GLSLVer.GetString(), "GLSL4") == 0)
	{
		fullPath = "glprogs/GLSL/GL4/";
	}
	else if (idStr::Icmp(r_GLSLVer.GetString(), "GLSL3") == 0)
	{
		fullPath = "glprogs/GLSL/GL3/";
	}
	else
	{
		fullPath = "glprogs/GLSL/GL2/";
	}

	fullPath += name;
	char	*fileBuffer;
	//GLchar	*Buffer;

	common->Printf("%s\n", fullPath.c_str());

	// load the program even if we don't support it, so
	// fs_copyfiles can generate cross-platform data dumps
	fileSystem->ReadFile(fullPath.c_str(), (void **)&fileBuffer, NULL);
	if (!fileBuffer) {
		common->Printf(": File not found\n");
		return false;
	}
	buffer.Append(fileBuffer);

	fileSystem->FreeFile(fileBuffer);

	//all good whe done
	return true;
}

/*
===========================================================================
Create Shaders
===========================================================================
*/
/*Render Shader*/
Shader::Shader()
{
	this->ShaderID = INVALID_PROGID;
}

Shader::~Shader()
{
	this->type = 0;
	this->name.Clear();
	this->FileDir = nullptr;
	glDeleteShader(this->ShaderID);
}

bool Shader::LoadShader(bool HasGS)
{
	//store shader compilation status
	GLint compiled = GL_FALSE;
	idStr	ShaderSource;
	GLuint TemShaderID = INVALID_PROGID;
	ShaderDefines_t Options;

#if 0
	//set if Have GS
	if (HasGS)
	{
		Options = |DEF_USE_GEOMETRY_SHADER;
	}
#endif

	//try Load Vertex Shader Source
	if (!LoadShaderSource(this->FileDir.c_str(), ShaderSource))
	{
		common->Warning("Shader File not found\n");
		return false;
	}

	//gerate the shader
	TemShaderID = glCreateShader(type);

	//Add the shader Headers
	idStr ShSource = SetShaderHeader(ShaderSource, type, Options);

	//source pointer
	this->SourcePointer = ShSource.c_str();

	//set the shader source to buffer
	glShaderSource(TemShaderID, 1, &SourcePointer, NULL);

	//compile the loaded shader source
	glCompileShader(TemShaderID);
	glGetShaderiv(TemShaderID, GL_COMPILE_STATUS, &compiled);

	//Check shader
	ShaderErrorHadler(TemShaderID, ShaderSource, compiled);

	// shader is not compiled, clear
	if (compiled == GL_FALSE)
	{
		common->Warning("Can't compile shader program %s", this->name.c_str());

		glDeleteShader(TemShaderID);
		TemShaderID = INVALID_PROGID;
		return false;
	}

	this->ShaderID = TemShaderID;

	return true;
}

/*
===========================================================================
Render Program
===========================================================================
*/
RenderProgram::RenderProgram(void)
{
	this->programID = INVALID_PROGID;
	this->VShaderID = INVALID_PROGID;
	this->GShaderID = INVALID_PROGID;
	this->FShaderID = INVALID_PROGID;

	//set the the number of elements in the list and create a defalt
	//this->Uniforms.SetNum(UN_UNIFORM_NUMBER, new Uniforms_t());
}

/*
=================
RenderProgram
=================
*/
RenderProgram::RenderProgram(const idStr name, BuilthinIdent_t Ident, bool Esential)
{
	this->programID = INVALID_PROGID;
	this->VShaderID = INVALID_PROGID;
	this->GShaderID = INVALID_PROGID;
	this->FShaderID = INVALID_PROGID;

	//set the the number of elements in the list and create a defalt
	//this->Uniforms.SetNum(UN_UNIFORM_NUMBER, new Uniforms_t());

	// Set the program name
	this->ProgramName = name;
	
	//set the program identtion
	this->ident = Ident;

	//set to stop engine if have any error
	this->EsentialProgram = Esential;
}

/*
=================
~RenderProgram
Destroy shader Program
=================
*/
RenderProgram::~RenderProgram()
{
	glDeleteProgram(this->programID);

	this->FSIndex = NULL;
	this->GSIndex = NULL;
	this->VSIndex = NULL;
	this->ident = BUILTIN_NULL;
	this->ProgramName.Clear();
	this->Uniforms.Clear();
}

/*
=================
RenderProgram::LinkProgram
link all the shaders to the render program
=================
*/
void RenderProgram::LinkProgram(Shader* VertexShader, Shader* FragmentShader)
{
	this->programID = glCreateProgram();

	//Attach vertex shader
	if (VertexShader->GetShader() != INVALID_PROGID)
	{
		glAttachShader(this->programID, VertexShader->GetShader());
	}
	else
	{
		if (this->EsentialProgram)
			common->Error(" Attach Vertex Shader");
		else
			common->Warning(" Attach Vertex Shader");

		return;
	}

	//Attach Fragment shader
	if (FragmentShader->GetShader() != INVALID_PROGID)
	{
		glAttachShader(this->programID, FragmentShader->GetShader());
	}
	else
	{
		if (this->EsentialProgram)
			common->Error("error on Attach Fragment Shader");
		else
			common->Warning("error on Attach Fragment Shader");

		return;
	}

	// Link the program
	glLinkProgram(this->programID);

#if 0
	int linked = GL_FALSE;
	//check the program
	glGetProgramiv(this->programID, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		glDeleteProgram(this->programID);
		if (this->EsentialProgram)
			common->Error("while linking GLSL program %s\n", ProgramName.c_str());
		else
			common->Warning("while linking GLSL program %s\n", ProgramName.c_str());

		return;
	}

#else
	ShaderProgramErrorHandler(this->programID, this->ProgramName);
#endif
}

/*
=================
RenderProgram::LinkProgram
link all the shaders to the render program
=================
*/
void RenderProgram::LinkProgram(Shader* VertexShader, Shader* FragmentShader, Shader* GeometryShader)
{
	int linked = GL_FALSE;

	this->programID = glCreateProgram();

	glProgramParameteri(this->programID, GL_PROGRAM_SEPARABLE, TRUE);

	//Attach vertex shader
	if (VertexShader->GetShader() != INVALID_PROGID)
		glAttachShader(this->programID, VertexShader->GetShader());
	else
	{
		if (this->EsentialProgram)
			common->Error(" on Attach Vertex Shader");
		else
			common->Warning(" on Attach Vertex Shader");
		return;
	}

	//Attach Geometry shader
	if (GeometryShader->GetShader() != INVALID_PROGID)
		glAttachShader(this->programID, GeometryShader->GetShader());
	else
		if (this->EsentialProgram)
			common->Error("on Attach Geometry Shader");
		else
			common->Warning("on Attach Geometry Shader");

	//Attach Fragment shader
	if (FragmentShader->GetShader() != INVALID_PROGID)
		glAttachShader(this->programID, FragmentShader->GetShader());
	else
	{
		if (this->EsentialProgram)
			common->Error(" on Attach Fragment Shader");
		else
			common->Warning(" on Attach Fragment Shader");

		return;
	}

	// Link the program
	common->Printf("Linking program: %s\n", this->ProgramName.c_str());
	glLinkProgram(this->programID);

#if 0
	glGetProgramiv(this->programID, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		if (this->EsentialProgram)
			common->Error("Error while linking GLSL program %s\n", ProgramName);
		else
			common->Warning("Error while linking GLSL program %s\n", ProgramName);

		glDeleteProgram(this->programID);
		return;
	}
#else
	ShaderProgramErrorHandler(this->programID, this->ProgramName);
#endif
	common->Printf("\n");
}

/*
=================
R_ValidateGLSLProgram
makes sure GLSL program is valid
=================
*/
bool RenderProgram::ValidateProgram()
{
	GLint validProgram;
	glValidateProgram(this->programID);
	glGetShaderiv(this->programID, GL_VALIDATE_STATUS, &validProgram);
	if (!validProgram)
	{
		common->Warning("RenderProgram::ValidateProgram: program invalid\n");
		return false;
	}
	return true;
}

/*
=================
appendUniform
add uniform to the uniforms lists
=================
*/
void RenderProgram::appendUniform(const char* name, programUniform_t ident)
{ 
	const char* Program = ProgramName.c_str();
	GLint Location = GL_FALSE;

	//create a new uniform whit the givem ident
	int idIdx = Uniforms.Append(Uniforms_t(idStr(name), ident));

	//get uniform location
	Uniforms[idIdx].uniform = glGetUniformLocation(this->programID, name);

	//check uniform Location
	glGetUniformiv(this->programID, Uniforms[idIdx].uniform, &Location);
	if (Location == GL_INVALID_OPERATION)
	{
		common->Warning("Can't get uniform location %s, from program %s", name, Program);
	}
}

/*
==================
GetUniform
Find and retur unifor by ident type
==================
*/
GLint RenderProgram::GetUniform(programUniform_t ident)
{
	//find in the uniform list the givem
	for (unsigned int i = 0; i < Uniforms.Size(); i++)
	{
		if (Uniforms[i].Ident == ident)
			return Uniforms[i].uniform;
	}

	//inf not uniform find in list return invalid location
	return INVALID_PROGID;
}

void RenderProgram::ListUniforms()
{
	for (int i = 0; i < Uniforms.Num(); i++)
	{
		//don't list invalid uniforms
		if (Uniforms[i].Ident != UN_UNIFORM_INVALID)
		{
			common->Printf("uniform: name: %s, location: %d \n", Uniforms[i].Name.c_str(), Uniforms[i].uniform);
		}
	}
}

