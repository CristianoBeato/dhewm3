in vec4						var_Color;  
in vec3						var_position;
in vec2						var_TexCoord;
in mat3						var_mat_os2ts;

uniform samplerCube			u_EnvCubemap;
uniform sampler2D			u_normalTexture;     

out vec4 color;

void main( void ) 
{
	vec3 I = normalize(Position - cameraPos);
	vec3 R = reflect(I, normalize(Normal));
	color = texture(u_EnvCubemap, R);
}