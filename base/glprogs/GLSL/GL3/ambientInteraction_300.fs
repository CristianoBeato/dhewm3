
#ifdef USE_GEOMETRY_SHADER
in GS_OUT
#else	
in VS_OUT
#endif
{
	vec3								var_Position;
	vec2								var_TexDiffuse;      
	vec2								var_TexNormal;         
	vec4								var_TexLight; 
	mat3								var_mat_os2ts;
	vec4								var_Color;   
} fs_in;   

uniform sampler2D   		u_lightFalloffTexture;         
uniform sampler2D   		u_lightProjectionTexture;

uniform sampler2D   		u_diffuseTexture;   
uniform sampler2D   		u_normalTexture;         

uniform vec4 				u_lightOrigin;   
uniform vec4 				u_viewOrigin;
uniform vec4 				u_diffuseColor;    

out vec4 color;

//Math Utils
float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
float dot3 (vec3 a , vec3 b ) {return dot ( a , b ) ; }

void main( void ) 
{
	//Normal Map Texture
	vec4 bumpMap = texture2D ( u_normalTexture , fs_in.var_TexNormal ) ;

	//difuse texture
	vec4 diffuseMap = texture2D( u_diffuseTexture, fs_in.var_TexDiffuse );
	
	//lightFalloff and Light Projection
	vec4 lightFalloff = texture2D ( u_lightFalloffTexture , vec2( fs_in.var_TexLight.z, 0.5 ) ) ;
	vec4 lightProj = texture2DProj ( u_lightProjectionTexture , fs_in.var_TexLight.xyw ) ;
	
	vec3 lightVector = normalize(fs_in.var_mat_os2ts * (u_lightOrigin.xyz - fs_in.var_Position));
	vec3 viewVector = normalize (fs_in.var_mat_os2ts * (u_viewOrigin.xyz - fs_in.var_Position));
	
	vec3 localNormal ;
	localNormal. xy = bumpMap. wy - 0.5 ;
	localNormal. z = sqrt ( abs ( dot ( localNormal. xy , localNormal. xy ) - 0.25 ) ) ;
	localNormal = normalize ( localNormal ) ;
	
	float LdotN = saturate ( dot3 ( localNormal , lightVector ) ) ;
	
#ifdef USE_HALF_LAMBERT
	//float HalfLdotN = dot3 ( localNormal , lightVector ) * 0.5 + 0.5 ;
	float HalfLdotN = dot3 ( localNormal , lightVector ) * 0.5 + 0.25 ;
	HalfLdotN *= HalfLdotN ;	
	float lambert = mix ( LdotN , HalfLdotN , 0.5 ) ;
#else
	float lambert = LdotN;
#endif

	vec3 HalfAngleVector = normalize ( lightVector + viewVector ) ;
	float HdotN = clamp ( dot3 ( HalfAngleVector , localNormal ) , 0.0 , 1.0 ) ;
	
	// compute the diffuse term 
	vec3 diffuseColor = diffuseMap.rgb;// * u_diffuseColor.rgb ;
	vec3 lightColor = lightProj.rgb * lightFalloff.rgb;
	
	color.rgb = diffuseColor * lambert * lightColor * fs_in.var_Color.rgb ;

	color.a = 1.00f;
   
}