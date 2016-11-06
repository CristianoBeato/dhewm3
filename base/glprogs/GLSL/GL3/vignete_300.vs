
layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;

uniform float	rectSize;

out vec2 var_TexCoord;
out vec3 var_vertColor;

void main( void ) 
{
	gl_Position = vec4(Position, 1.0f); 
	
	var_TexCoord = TexCoord.st;
}