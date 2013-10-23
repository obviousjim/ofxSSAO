
uniform sampler2DRect colorTex;
uniform sampler2DRect normalDepthTex;
uniform sampler2DRect normalDepthTexHalf;
uniform sampler2DRect normalDepthTexQuarter;

uniform int reflectRays;
uniform float minThreshold;
uniform float maxThreshold;
uniform float numSamples;
uniform float radius;
uniform vec2 randSeed;
uniform float nearClip;
uniform float farClip;
uniform float exponent;
uniform float weight;


uniform vec3 samples[24];

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}

float rand(vec2 n){
    return 0.6 + 0.4 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main(void)
{
	vec4 normDepth = texture2DRect( normalDepthTex, gl_TexCoord[0].st );
    float depth = normDepth.a;// linearizeDepth( texture2DRect( depthtex, gl_TexCoord[0].st ).r );
    if( depth == 1. ){
        gl_FragColor = vec4(1.);
        return;
    }
    bool refRay = reflectRays == 1;
    
    vec3 norm = (refRay)? normDepth.xyz*2.-1. : vec3(0,0,1);

    float ao = 0.;
    float delta, rnd;
    float dpthScl = pow(1.-depth, 3.);
    float rad = radius * dpthScl;
    float mn = minThreshold * dpthScl;
    float mx = maxThreshold * dpthScl;
    vec3 ray;

	if(refRay)
	{
		for(int i=0; i<int(numSamples); i++)
		{
			ray = reflect(-norm, samples[i].xyz) * rad * rand(gl_FragCoord.xy+randSeed);
			
			delta = ( depth - mn - texture2DRect( normalDepthTex, gl_TexCoord[0].st + ray.xy).a );
			ao += min( 1., ( delta > 0. ) ? delta/max(delta,mx) : (mx-delta)/mx );
		}
	}
	else
	{
		for(int i=0; i<int(numSamples); i++)
		{
			rnd = rad * rand(gl_FragCoord.xy+randSeed);
			ray = samples[i] * rnd;
			
			delta = ( depth - mn - texture2DRect( normalDepthTex, gl_TexCoord[0].st + ray.xy).a );
			ao += min( 1., ( delta > 0. ) ? delta/max(delta,mx) : (mx-delta)/mx );
		}
	}
	
    
    ao = pow( ao/numSamples, exponent);

	gl_FragColor = texture2DRect( colorTex, gl_TexCoord[0].st ) * vec4( vec3(ao * weight + (1. - weight)), 1. );
}
