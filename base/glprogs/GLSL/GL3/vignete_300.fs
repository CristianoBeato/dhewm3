
#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

in vec2						var_TexCoord; 
in vec3 					var_vertColor;

uniform sampler2D RenderTexture0;

//the Fragment output
out vec4 color;

void main()
{
	#if 1
	float vignette = 1.8 - 0.6 * (dot(var_TexCoord - 0.5, var_TexCoord - 0.5) * 2.0);
	vec3 rgb = texture2D(RenderTexture0, var_TexCoord).rgb;
	color.rgb = vignette * rgb;

	color.a = 1.0f;
	#else
	color = vec4(0.0, 0.5, 0.5, 1.0)
	#endif
}
