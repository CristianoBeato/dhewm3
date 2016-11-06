#version 330 compatibility     

out	vec2				var_TexCoord;

void main( void ) 
{
	gl_Position = ftransform(); 
	
	var_TexCoord = attr_TexCoord;
}