uniform sampler2DRect tex;

uniform float nearClip;
uniform float farClip;

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}

varying vec3 norm;
void main(void)
{
    //I think we get more freedom if we pass a texture with our colors rather then being tied to gl_Color
    gl_FragColor = vec4( norm, linearizeDepth(gl_FragCoord.z) );
    
//    gl_FragData[0] = vec4( norm, linearizeDepth(gl_FragCoord.z) );//texture2DRect( tex, gl_TexCoord[0].st) * gl_Color;
//    gl_FragData[1] = vec4( norm, 1.0 ) ;
}