layout (location = 1) in vec3          	attr_Normal; 

out vec4            					var_Color; 
out vec3 								var_TexCoord1;

void main( void ) {

	gl_Position = ftransform(); 

	vec3 vNormal = attr_Normal * 2.0 - 1.0 ;
	var_TexCoord1 = vNormal.xyz ;
	
	// primary color  
	var_Color = gl_FrontColor;  
}