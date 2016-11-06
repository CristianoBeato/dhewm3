#version 330 compatibility   
   
uniform sampler2D   RenderTexture0;     
  
out vec4 color;
  
void main( void ) 
{
	vec4 RenderImage = texture2D(RenderTexture0, var_TexCoord);

	color = RenderImage* 2;
}