uniform sampler2DRect tex;
uniform float nearClip;
uniform float farClip;

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}

varying vec3 norm;
void main(void)
{
    gl_FragData[0] = gl_Color;
    gl_FragData[1] = vec4( normalize( norm ) * .5 + .5, linearizeDepth( gl_FragCoord.z ) );
}