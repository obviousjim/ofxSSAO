uniform sampler2DRect colortex;
uniform sampler2DRect normDepthTex;

uniform int reflectRays;
uniform float minThreshold;
uniform float maxThreshold;
uniform float numSamples;
uniform float radius;
uniform vec2 randSeed;
uniform float nearClip;
uniform float farClip;
uniform float exponent;

uniform vec3 samples[24];

float rand(vec2 n){
    return 0.5 + 0.5 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main(void)
{

    vec4 nrmDpth = texture2DRect( normDepthTex, gl_TexCoord[0].st );
    float depth = nrmDpth.a;
    vec3 norm = nrmDpth.rgb;
    if( depth == 1. ){
        gl_FragColor = vec4(1.);//basecolor;
        return;
    }
    float ao = 0.;
    float rnd, delta;
    float dpthScl = pow((1.-depth), 2.);
    float rad = radius * dpthScl;
    float mn = minThreshold * dpthScl;
    float mx = maxThreshold * dpthScl;
    vec3 ray;
    bool refRay = (reflectRays == 1);

    for(int i=0; i<int(numSamples); i++){
        rnd = rand( gl_FragCoord.xy+randSeed+vec2(i));
        ray = (refRay)? reflect( -samples[i], norm) * rad * rnd : samples[i] * rad * rnd;
        delta = depth - mn - texture2DRect( normDepthTex, gl_TexCoord[0].st + ray.xy).a;
        ao += min( 1., ( delta > 0. ) ? delta/max(delta,mx) : (mx-delta)/mx );
    }
    
    ao = pow( ao/numSamples, exponent);
    
    
//    vec4 basecolor = texture2DRect( colortex, gl_TexCoord[0].st );

    gl_FragColor = vec4( vec3(ao), 1. );
}
