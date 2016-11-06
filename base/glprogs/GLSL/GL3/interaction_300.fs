
//THX Robert Backbans for the RBDoom3 Shaders
#ifdef USE_GEOMETRY_SHADER
in GS_OUT
#else	
in VS_OUT
#endif
{
	vec3								var_Position;
	vec2								var_TexDiffuse;      
	vec2								var_TexNormal;      
	vec2								var_TexSpecular;      
	vec4								var_TexLight; 
	mat3								var_TangentBitangentNormalMatrix; 
	mat3								var_mat_os2ts;
	vec4								var_Color;   
	vec2								var_TexCoord; 
	float 								var_distance;
} fs_in;

//Material Textures
uniform sampler2D   	    u_diffuseTexture;      
uniform sampler2D   	    u_specularTexture;  //Alpha (.a) is the glos mask
uniform sampler2D			u_materialMask; //Red(.r), Green (.g) is the Rim Light Intencity, Blue(.b) is metalness, Alpha (.a)  
 
//Light Textures
uniform sampler2D   	    u_lightFalloffTexture;      
uniform sampler2D   	    u_lightProjectionTexture;

uniform sampler2D   	    u_normalTexture;      

uniform vec4 				u_lightOrigin;   
uniform vec4 				u_viewOrigin;

uniform vec4 				u_diffuseColor;     
uniform vec4 				u_specularColor;    

//In Render Time SRGB Suport
uniform int					u_useSRGB;

uniform float				u_rimlightAmout;
uniform float				u_rimlightPower;

//the Fragment output
out vec4 color;

//Math Utils
float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
float dot3 (vec3 a , vec3 b ) {return dot ( a , b ) ; }

vec3 sRGBToLinearRGB (vec3 rgb )
{
	return max ( pow ( rgb , vec3 ( 2.2 ) ) , vec3 ( 0.0 ) ) ;
}
vec4 sRGBAToLinearRGBA (vec4 rgba )
{
	return vec4 ( max ( pow ( rgba. rgb , vec3 ( 2.2 ) ) , vec3 ( 0.0 ) ) , rgba. a ) ;
}



void main()
{
	//Normal Map Texture
	vec4 bumpMap = texture2D ( u_normalTexture , fs_in.var_TexNormal ) ;
	//specular texture
	vec4 specularMap;
	
	if(u_useSRGB == 1)
	{
		specularMap = sRGBAToLinearRGBA(texture2D( u_specularTexture, fs_in.var_TexSpecular ));
	}
	else
	{
		specularMap = texture2D( u_specularTexture, fs_in.var_TexSpecular );   	
	}
	
	//difuse texture
	vec4 diffuseMap;
	if(u_useSRGB == 1)
	{
		diffuseMap = sRGBAToLinearRGBA(texture2D( u_diffuseTexture, fs_in.var_TexDiffuse ));
	}
	else
	{
		diffuseMap = texture2D( u_diffuseTexture, fs_in.var_TexDiffuse );
	}
	
	//lightFalloff and Light Projection
	vec4 lightFalloff = texture2D ( u_lightFalloffTexture , vec2( fs_in.var_TexLight.z, 0.5 ) ) ;
	vec4 lightProj = texture2DProj ( u_lightProjectionTexture , fs_in.var_TexLight.xyw ) ;
	
#if 0
	vec3 lightVector = normalize(u_lightOrigin.xyz - fs_in.var_Position);
	vec3 viewVector = normalize (u_viewOrigin.xyz - fs_in.var_Position);
#else
	vec3 lightVector = normalize(fs_in.var_mat_os2ts * (u_lightOrigin.xyz - fs_in.var_Position));
	vec3 viewVector = normalize (fs_in.var_mat_os2ts * (u_viewOrigin.xyz - fs_in.var_Position));
#endif
	
		vec3 localNormal ;
#if 0 //#ifdef USE_NORMAL_FMT_RGB8
	localNormal.xy = bumpMap.rg - 0.5;
#else
	localNormal.xy = bumpMap.wy - 0.5;
#endif
	localNormal.z = sqrt ( abs ( dot ( localNormal. xy , localNormal. xy ) - 0.25 ) ) ;
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
	
	//float specularPower = specularMap.a;
	const float specularPower = 10.0f;
	
	// compute the specular term
	vec3 specularContribution = vec3 ( pow ( HdotN , specularPower ) ) ;
	vec3 specularColor = specularMap.rgb * specularContribution * u_specularColor.xyz ;
	
	// compute the diffuse term 
	vec3 diffuseColor = diffuseMap.rgb * u_diffuseColor.rgb ;
	
	vec3 lightColor;
	if(u_useSRGB == 1)
	{
		lightColor = sRGBToLinearRGB( lightProj.rgb * lightFalloff.rgb );	
	}
	else
	{
		lightProj.rgb * lightFalloff.rgb;
	}
	
	
#ifdef USE_RIM_LIGHT_NOT_IN_BUILD
	float rim = (u_rimlightAmout * ColorsMasks.g) - saturate( HdotN );
	vec3 rimColor = diffuseColor.rgb * lightProj.rgb * lightFalloff.rgb * 1.0f * pow( rim, u_rimlightPower ) * u_diffuseColor.rgb;// * halfLdotN;
	
	color.rgb = ( diffuseColor + specularColor ) * lambert * lightColor * fs_in.var_Color.rgb + (rimColor * specularMap.a);//Glos Map 
#else
	color.rgb = ( diffuseColor + specularColor ) * lambert * lightColor * fs_in.var_Color.rgb;
#endif
	
	color.a = 1.00f;
}