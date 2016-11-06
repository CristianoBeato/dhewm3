//The layou location begin from 0, for, the gl_Vertex already use 0

layout (location = 1) in vec3          	attr_Normal; 
layout (location = 2) in vec4          	attr_TexCoord; 
layout (location = 3) in vec3          	attr_Tangent;   
layout (location = 4) in vec3          	attr_Bitangent;    

out VS_OUT 
{
	vec3								var_Position;
	vec2								var_TexDiffuse;      
	vec2								var_TexNormal;      
	vec2								var_TexSpecular;      
	vec4								var_TexLight;
	mat3								var_mat_os2ts;
	vec4								var_Color;   
	vec2								var_TexCoord;
} vs_out;

uniform vec4 							u_lightOrigin;   
uniform mat4							u_modelMatrix;   
uniform vec4							u_lightProjectionS;   
uniform vec4							u_lightProjectionT;   
uniform vec4							u_lightFalloff;   
uniform vec4							u_lightProjectionQ;   
uniform vec4							u_bumpMatrixS;   
uniform vec4							u_bumpMatrixT;   
uniform vec4							u_diffuseMatrixS;   
uniform vec4							u_diffuseMatrixT;   
uniform vec4							u_specularMatrixS;   
uniform vec4							u_specularMatrixT;   
uniform vec4							u_colorModulate;   
uniform vec4							u_colorAdd;  

void main()
{
    // transform vertex position into homogenous clip-space   
	gl_Position = ftransform();
	vs_out.var_Position = gl_Vertex.xyz;
	
	// normal map texgen   
	vs_out.var_TexNormal.x = dot( u_bumpMatrixS, attr_TexCoord );
	vs_out.var_TexNormal.y = dot( u_bumpMatrixT, attr_TexCoord ); 

	// diffuse map texgen   
	vs_out.var_TexDiffuse.x = dot( u_diffuseMatrixS, attr_TexCoord );
	vs_out.var_TexDiffuse.y = dot( u_diffuseMatrixT, attr_TexCoord );
   
	// specular map texgen   
	vs_out.var_TexSpecular.x = dot( u_specularMatrixS, attr_TexCoord );
	vs_out.var_TexSpecular.y = dot( u_specularMatrixT, attr_TexCoord );

	// light texgen
	vs_out.var_TexLight.x = dot( u_lightProjectionS, gl_Vertex );   
	vs_out.var_TexLight.y = dot( u_lightProjectionT, gl_Vertex );   
	vs_out.var_TexLight.z = dot( u_lightFalloff, gl_Vertex );   
	vs_out.var_TexLight.w = dot( u_lightProjectionQ, gl_Vertex );   
 
	// construct object-space-to-tangent-space 3x3 matrix
	vs_out.var_mat_os2ts = mat3(
         attr_Tangent.x, attr_Bitangent.x, attr_Normal.x,
         attr_Tangent.y, attr_Bitangent.y, attr_Normal.y,
         attr_Tangent.z, attr_Bitangent.z, attr_Normal.z
    );
 
	// primary color   
	vs_out.var_Color = gl_FrontColor * u_colorModulate + u_colorAdd;  

	vs_out.var_TexCoord = attr_TexCoord.xy;
}