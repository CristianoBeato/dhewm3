
#ifndef __SL_DEF_H__
#define __SL_DEF_H__

#include "renderer/tr_local.h"

static const GLuint INVALID_PROGID = 0xFFFFFFFF;

/*
struct glslUniformLocation_t
{
	int		parmIndex;
	GLint	uniformIndex;
};
*/
/*
struct attribInfo_t
{
	const char* 	type;
	const char* 	name;
	const char* 	semantic;
	const char* 	glsl;
	//int				bind;
	int				flags;
	//int				vertexMask;
};

static const int AT_VS_IN = BIT(1);
static const int AT_VS_OUT = BIT(2);
static const int AT_PS_IN = BIT(3);
static const int AT_PS_OUT = BIT(4);



attribInfo_t attribs[] =
{
	// vertex attributes
	{ "vec4", "position",		"POSITION",		"in_Position",		AT_VS_IN },
	{ "vec2", "texcoord",		"TEXCOORD0",	"in_TexCoord",		AT_VS_IN },
	{ "vec4", "normal",			"NORMAL",		"in_Normal",		AT_VS_IN },
	{ "vec4", "tangent",		"TANGENT",		"in_Tangent",		AT_VS_IN },
	{ "vec4", "color",			"COLOR0",		"in_Color",			AT_VS_IN },
	{ "vec4", "color2",			"COLOR1",		"in_Color2",		AT_VS_IN },

	// fragment program output
	{ "float4",		"color",	"COLOR",	"gl_FragColor", AT_PS_OUT},
	{ "half4",		"hcolor",	"COLOR",	"gl_FragColor", AT_PS_OUT},
	{ "float4",		"color0",	"COLOR0",	"gl_FragColor", AT_PS_OUT},
	{ "float4",		"color1",	"COLOR1",	"gl_FragColor", AT_PS_OUT},
	{ "float4",		"color2",	"COLOR2",	"gl_FragColor", AT_PS_OUT},
	{ "float4",		"color3",	"COLOR3",	"gl_FragColor", AT_PS_OUT},
	{ "float",		"depth",	"DEPTH",	"gl_FragDepth", AT_PS_OUT}
};*/

#endif // !__SL_DEF_H__