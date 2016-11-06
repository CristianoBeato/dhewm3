
#define PI 3.1415926

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
	mat3								var_mat_os2ts;
	vec4								var_Color;   
	vec2								var_TexCoord;
} fs_in;

//Material Textures
uniform sampler2D   	    u_diffuseTexture;      
uniform sampler2D   	    u_specularTexture;  //Alpha (.a) is the glos mask
//uniform sampler2D			u_materialMask; //Red(.r), Green (.g) is the Rim Light Intencity, Blue(.b) is metalness, Alpha (.a)  
 
//Light Textures
uniform sampler2D   	    u_lightFalloffTexture;      
uniform sampler2D   	    u_lightProjectionTexture;

uniform sampler2D   	    u_normalTexture;      

uniform vec4 				u_lightOrigin;   
uniform vec4 				u_viewOrigin;

uniform vec4 				u_diffuseColor;     
uniform vec4 				u_specularColor;    

uniform float				u_rimlightAmout;
uniform float				u_rimlightPower;

#if 0//#ifdef MULTIPASS

layout (location = 0) out vec4 color;      
layout (location = 1) out vec3 NormalOut;
layout (location = 2) out vec3 GlosOut;
//layout (location = 3) out vec3 WordOut;
//layout (location = 4) out vec3 wordFresnel;

#else
out vec4 color; 
#endif

//the Fragment output


//Math Utils
float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
float dot3 (vec3 a , vec3 b ) {return dot ( a , b ) ; }
const vec4 LUMINANCE_SRGB = vec4( 0.2125, 0.7154, 0.0721, 0.0 );
vec3 sRGBToLinearRGB (vec3 rgb ) {
	return max ( pow ( rgb , vec3 ( 2.2 ) ) , vec3 ( 0.0 ) ) ;
}
vec4 sRGBAToLinearRGBA (vec4 rgba ) {
	return vec4 ( max ( pow ( rgba. rgb , vec3 ( 2.2 ) ) , vec3 ( 0.0 ) ) , rgba. a ) ;
}

void main()
{
	//Normal Map Texture
	vec4 bumpMap = texture2D ( u_normalTexture , fs_in.var_TexNormal );
	
	//specular texture
	vec4 specularMapSRGBA = texture2D( u_specularTexture, fs_in.var_TexSpecular );
	vec4 specularMap = sRGBAToLinearRGBA( specularMapSRGBA );
	
	//difuse texture
	vec4 diffuseMap = sRGBAToLinearRGBA(texture2D( u_diffuseTexture, fs_in.var_TexDiffuse ));
	
	//lightFalloff and Light Projection
	vec4 lightFalloff = texture2D ( u_lightFalloffTexture , vec2( fs_in.var_TexLight.z, 0.5 ) ) ;
	vec4 lightProj = texture2DProj ( u_lightProjectionTexture , fs_in.var_TexLight.xyw ) ;
	
	//Texture conteining varios colors Masks
	//vec4 ColorsMasks = texture2D(u_materialMask, fs_in.var_TexCoord);

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

#if 1 //defined(USE_METALNESS)
	float metallic = specularMapSRGBA.g ;
	float roughness = specularMapSRGBA.r ;
	float glossiness = 1.0 - roughness;

	// the vast majority of real-world materials (anything not metal or gems) have F(0°)
	// values in a very narrow range (~0.02 - 0.08)
	
	// approximate non-metals with linear RGB 0.04 which is 0.08 * 0.5 (default in UE4)
	vec3 dielectricColor = vec3( 0.04 );
	
	// derive diffuse and specular from albedo(m) base color
	vec3 baseColor = diffuseMap.rgb;
	
	vec3 diffuseColor = baseColor * ( 1.0 - metallic );
	vec3 specularColor = mix( dielectricColor, baseColor, metallic );
#else
	// HACK calculate roughness from D3 gloss maps
	float Y = dot( LUMINANCE_SRGB.rgb, specularMapSRGBA.rgb );
	
	//const float glossiness = clamp( 1.0 - specularMap.r, 0.0, 0.98 );
	float glossiness = clamp( pow( Y, 1.0 / 2.0 ), 0.0, 0.98 );
	
	float roughness = 1.0 - glossiness;
	
	vec3 diffuseColor = diffuseMap.rgb;
	vec3 specularColor = specularMap.rgb;
#endif
	
	// RB: compensate r_lightScale 3 and the division of Pi
	//lambert *= 1.3;
	
	// rpDiffuseModifier contains light color multiplier
	vec3 lightColor = sRGBToLinearRGB( lightProj.xyz * lightFalloff.xyz );// * rpDiffuseModifier.xyz;
	//vec3 lightColor = lightProj.rgb * lightFalloff.rgb ;// * rpDiffuseModifier.xyz;
	
	float VdotN = clamp( dot3( viewVector, localNormal ), 0.0, 1.0 );
	float VdotH = clamp( dot3( viewVector, HalfAngleVector ), 0.0, 1.0 );
	float LdotH = clamp( dot3( lightVector, HalfAngleVector ), 0.0, 1.0 );
	
	// compensate r_lightScale 3 * 2
	vec3 reflectColor = specularMap.rgb *  u_specularColor.rgb * 1.0;// * 0.5;
	
	float rr = roughness * roughness;
	float rrrr = rr * rr;
	
	// disney GGX
	float D = ( HdotN * HdotN ) * ( rrrr - 1.0 ) + 1.0;
	float VFapprox = ( LdotH * LdotH ) * ( roughness + 0.5 );
	vec3 specularBRDF = ( rrrr / ( 4.0 * PI * D * D * VFapprox ) ) * LdotN * reflectColor;
	//specularBRDF = half3( 0.0 );
	
	// see http://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	//lambert /= PI;
	
	//half3 diffuseColor = mix( diffuseMap, F0, metal ) * rpDiffuseModifier.xyz;
	vec3 diffuseBRDF = diffuseColor * lambert * sRGBToLinearRGB( u_diffuseColor.rgb );
	//vec3 diffuseBRDF = diffuseColor * lambert * u_diffuseColor.rgb ;
#ifdef USE_RIM_LIGHT
	//float rim = (u_rimlightAmout * ColorsMasks.g) - saturate( HdotN );
	float rim = (u_rimlightAmout * specularMap.a) - saturate( HdotN );
	vec3 rimColor = (diffuseColor.rgb * lightProj.rgb * lightFalloff.rgb * 1.0f * pow( rim, u_rimlightPower ) * u_diffuseColor.rgb) * roughness;// * halfLdotN;
	
	color.rgb = ( diffuseBRDF + specularBRDF ) * lightColor * fs_in.var_Color.rgb + rimColor;
#else
	//color.rgb = ( diffuseBRDF + specularBRDF ) * lightColor * fs_in.var_Color.rgb * shadow;
	color.rgb = ( diffuseBRDF + specularBRDF ) * lightColor * fs_in.var_Color.rgb;
#endif

#if 0//#ifdef MULTIPASS
	//output Normal Pass
	NormalOut = localNormal;
	GlosOut = vec3(roughness, roughness, roughness);
	wordFresnel = vec3(HalfLdotN);
#endif

	color.a = 1.0;
}