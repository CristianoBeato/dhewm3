in vec3						var_TexCoord1;
in vec4						var_Color;  

uniform samplerCube			u_EnvCubemap;
out vec4 color;
  
void main( void ) 
{	
	vec4 cube = textureCube(u_EnvCubemap, var_TexCoord1 );
	
	color = var_Color;
	color.rgb *= cube;	
}