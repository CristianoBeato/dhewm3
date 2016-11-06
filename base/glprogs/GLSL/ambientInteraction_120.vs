varying vec3          		var_Position;  
varying vec2            	var_TexDiffuse;  
varying vec2            	var_TexNormal; 
varying vec4            	var_TexLight; 
varying mat3     			var_TangentBinormalNormalMatrix;  
varying vec4            	var_Color; 
varying mat3				var_mat_os2ts; 
 
attribute vec4          	attr_TexCoord;      
attribute vec3          	attr_Tangent;      
attribute vec3          	attr_Binormal;      
attribute vec3          	attr_Normal;   
  
uniform mat4				u_modelMatrix; 
uniform vec4 				u_lightProjectionS;  
uniform vec4 				u_lightProjectionT;  
uniform vec4 				u_lightProjectionQ;  
uniform vec4 				u_lightFalloff;  
uniform vec4 				u_bumpMatrixS;   
uniform vec4 				u_bumpMatrixT;   
uniform vec4 				u_diffuseMatrixS;   
uniform vec4 				u_diffuseMatrixT;   
uniform vec4 				u_colorModulate;  
uniform vec4				u_colorAdd;  
      
void main( void ) {      
    // mvp transform into clip space      
	gl_Position = ftransform( );  
	var_Position = gl_Vertex.xyz; 
 
	// normal map texgen   
	var_TexNormal.x = dot( u_bumpMatrixS, attr_TexCoord );
	var_TexNormal.y = dot( u_bumpMatrixT, attr_TexCoord ); 

	// diffuse map texgen   
	var_TexDiffuse.x = dot( u_diffuseMatrixS, attr_TexCoord );
	var_TexDiffuse.y = dot( u_diffuseMatrixT, attr_TexCoord );
 
	// light projection texgen 
	var_TexLight.x = dot( u_lightProjectionS, gl_Vertex ); 
	var_TexLight.y = dot( u_lightProjectionT, gl_Vertex ); 
	var_TexLight.z = dot( u_lightFalloff, gl_Vertex ); 
	var_TexLight.w = dot( u_lightProjectionQ, gl_Vertex ); 
 
	// construct tangent-binormal-normal 3x3 matrix    
	var_TangentBinormalNormalMatrix = mat3( attr_Tangent, attr_Binormal, gl_Normal ); 
	
		// construct object-space-to-tangent-space 3x3 matrix
	var_mat_os2ts = mat3(
         attr_Tangent.x, attr_Binormal.x, attr_Normal.x,
         attr_Tangent.y, attr_Binormal.y, attr_Normal.y,
         attr_Tangent.z, attr_Binormal.z, attr_Normal.z
    );
 
	// primary color  
	var_Color = gl_FrontColor * u_colorModulate + u_colorAdd;  
}