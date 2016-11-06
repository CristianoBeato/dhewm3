
//store shaders definitions and headers
#ifndef __SHADER_HEADER_H__
#define __SHADER_HEADER_H__

//Set the Version 1.20 shader header
static const char* GLSL_1_2 = "#version 120 core\n";

//set the shader version to 3.30 in compatibility mode 
static const char* GLSL_3_3 = "#version 330 compatibility\n";

//use shader version 3.30 in core mode 
static const char* GLSL_3_3_CORE = "#version 330 core\n";

//set the shader version to 3.30 in compatibility mode 
static const char* GLSL_4_0 = "#version 400 compatibility\n";

//use shader version 4.00 in core mode 
static const char* GLSL_4_0_CORE = "#version 400 core\n";

//define to use the type of Tonemapping Filmic or Reinhard 
//FILMIC DEFINE
static const char* DEFINE_USE_FILMIC = "#define USE_FILMIC\n";
static const char* DEFINE_USE_REINHARD = "#define USE_REINHARD\n";

//define use of Tesselation
static const char* DEFINE_USE_TESSELATE = "#define USE_TESSELATION\n";

//define debug Tesselation 
static const char* DEFINE_USE_GS = "#define USE_GEOMETRY_SHADER\n";

//define Use Half Lambert Shader
static const char* DEFINE_USE_HALFLAMBERT = "#define USE_HALF_LAMBERT\n";

//Suport For rim Light Term
static const char* DEFINE_USE_RIMLIGHT = "#define USE_RIM_LIGHT\n";

#endif // !__SHADER_HEADER_H__