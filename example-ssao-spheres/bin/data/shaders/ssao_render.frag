uniform sampler2DRect colortex;
uniform sampler2DRect normaltex;
uniform sampler2DRect depthtex;

uniform int reflectRays;
uniform float minThreshold;
uniform float maxThreshold;
uniform float numSamples;
uniform float radius;
uniform vec2 randSeed;
uniform float nearClip;
uniform float farClip;
uniform float exponent;

uniform vec3 samples[23];

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}

float rand(vec2 n){
    return 0.5 + 0.5 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main(void)
{
    vec4 basecolor = texture2DRect( colortex, gl_TexCoord[0].st );

    float depth = linearizeDepth( texture2DRect( depthtex, gl_TexCoord[0].st ).r );
    if( depth == 1. ){
        gl_FragColor = vec4(1.);//basecolor;
        return;
    }
    vec3 norm = texture2DRect( normaltex, gl_TexCoord[0].st ).xyz;
    float ao = 0.;
    float delta;
    float rad = radius * (1.-depth);
    float rnd = 0.;
    vec3 ray;
    bool refRay = (reflectRays == 1);

    for(int i=0; i<int(numSamples); i++){
        rnd = rand( gl_FragCoord.xy+randSeed+vec2(i*i));
        ray = (refRay)? reflect( -samples[i], norm) * rad * rnd : samples[i] * rad * rnd;
        delta = ( depth - minThreshold - linearizeDepth( texture2DRect( depthtex, gl_TexCoord[0].st + ray.xy).r ));
        ao += min( 1., ( delta > 0. ) ? delta/max(delta, maxThreshold) : (maxThreshold - delta)/maxThreshold );
    }
    
    ao = pow( ao/numSamples, exponent);
    gl_FragColor = vec4( vec3(ao), 1. ) * basecolor;
}
