uniform sampler2D RenderTexture0;
uniform vec2 scale;

out vec4 color;

void main()
{
    vec4 ResultColor = vec4(vec3(0.0), 0.0);

    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( -3.0 * scale.x, -3.0 * scale.y ) ) * 0.015625;
    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( -2.0 * scale.x, -2.0 * scale.y ) )*0.09375;
    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( -1.0 * scale.x, -1.0 * scale.y) )*0.234375;
    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( 0.0 , 0.0) )*0.3125;
    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( 1.0 * scale.x,  1.0 * scale.y ) )*0.234375;
    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( 2.0 * scale.x,  2.0 * scale.y ) )*0.09375;
    ResultColor += texture2D( RenderTexture0, gl_TexCoord[0].st + vec2( 3.0 * scale.x, 3.0 * scale.y ) ) * 0.015625;

    if(ResultColor.a < 0.5)
      discard;
    if(scale.y == 0.0)
      color = ResultColor;//vec4(ResultColor.xyz, 0.5);
    else
		color = color = vec4(vec3(0.0), (1.0 - color.x));
      //color = vec4(ResultColor.xyz, 0.5);//texture2D(RenderTexture0, gl_TexCoord[0].st);

}