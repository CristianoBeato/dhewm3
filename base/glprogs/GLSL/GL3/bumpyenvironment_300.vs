layout (location = 1) in vec3          	attr_Normal; 
layout (location = 2) in vec4          	attr_TexCoord; 
layout (location = 3) in vec3          	attr_Tangent;   
layout (location = 4) in vec3          	attr_Bitangent;  


out VS_OUT
{
	vec2				var_TexCoord;
	vec4            	var_Color;
	vec3				var_position;
	mat3				var_mat_os2ts;
} vs_out;

void main( void ) {

	gl_Position = ftransform(); 

	// primary color
	vs_out.var_Color = gl_Color; 
	vs_out.var_TexCoord = attr_TexCoord.st;
	
	// construct object-space-to-tangent-space 3x3 matrix
	vs_out.var_mat_os2ts = mat3(
         attr_Tangent.x, attr_Bitangent.x, attr_Normal.x,
         attr_Tangent.y, attr_Bitangent.y, attr_Normal.y,
         attr_Tangent.z, attr_Bitangent.z, attr_Normal.z
    );	
}