
layout (location = 0) in vec2          	attr_Position; 
layout (location = 1) in vec4          	attr_TexCoord; 

out	vec2				var_TexCoord;

void main( void ) 
{
	//gl_Position = ftransform(); 
	gl_Position = vec4(attr_Position, 0.0, 1.0);
	
	var_TexCoord = attr_TexCoord.st;
}