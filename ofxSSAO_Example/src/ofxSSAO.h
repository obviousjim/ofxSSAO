/*
 *  ofxSSAO.h
 *  blt_motionStudy
 *
 *  Created by lars berg on 2/10/12.
 *  Copyright 2012 lablab. All rights reserved.
 *
 */

#pragma once

class ofxSSAO{
public:
	
	ofxSSAO(){
		nearClip = 1;
		farClip = 1000;
		
		weight = 1.;
		radius = 50;
		maxThreshold = .1;
		minThreshold = .01;
	}
	~ofxSSAO(){};
	
	void setup( int w = (int)ofGetWidth(), int h = (int)ofGetHeight(), int format = GL_RGBA32F ){
		width = w;
		height = h;
		
		//deferred
		deferredPass.allocate(w, h, format, 0);//msaa gives us weird edges...
		makeDeferredShader();
		
		// depth blur
		fbo1.allocate(w, h, GL_RGB, 0);
		
		makeDaoShader();
	}
    
    void setWeight(float _weight){
        weight = _weight;
    }
    void setRadius(float _radius){
        radius = _radius;
    }
    void setMaxThreshold(float _maxThreshold){
        maxThreshold = _maxThreshold;
    }
    void setMinThreshold(float _minThreshold){
        minThreshold = _minThreshold;
    }
	
	void setClipPlanes( float near, float far ){
		nearClip = near;
		farClip = far;
		
		deferredShader.begin();
		deferredShader.setUniform1f("nearClip", nearClip);
		deferredShader.setUniform1f("farClip", farClip);
		deferredShader.end();
	}
	
	
	void begin(){
		
		deferredPass.begin();
		glClearColor(1,1,1,0);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_DEPTH_TEST );
		deferredShader.begin();
	}
	
	void end( bool drawToFbo=true, float elapsedTime = ofGetElapsedTimef() ){
		deferredShader.end();
		deferredPass.end();
		
		glColor3f(1, 1, 1);
		glDisable( GL_DEPTH_TEST );
		

		if(drawToFbo)   fbo1.begin();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_DEPTH_TEST );
		daoShader.begin();
		daoShader.setUniformTexture("depthtex", deferredPass.getTextureReference(), 0);
		daoShader.setUniform1f("maxThreshold", maxThreshold );
		daoShader.setUniform1f("minThreshold", minThreshold );
		daoShader.setUniform1f("radius", radius );
		daoShader.setUniform1f("weight", weight );
		daoShader.setUniform2f("randSeed", elapsedTime * ofRandom(.1, 1.), elapsedTime * ofRandom(.1, 1.));
		deferredPass.draw(0, 0, fbo1.getWidth(), fbo1.getHeight() );
		daoShader.end();
		if(drawToFbo)   fbo1.end();
	}
	
	
	void draw(int x=0, int y=ofGetHeight(), int w=ofGetWidth(), int h = -ofGetHeight()){
		glColor4f(1,1,1,1);
		fbo1.draw( x, y+h, w, -h);
	}
	
	void makeDaoShader(){
		
		float halfSphereSamps[] = {
			-0.717643236477, 0.273249441045, -0.688175618649, 
			-0.305361618869, 0.101219129631, -0.95300924778, 
			0.305361649948, 0.101219129631, -0.95300924778, 
			0.717643236477, 0.273249441045, -0.688175618649, 
			-0.448564588874, 0.753300700721, -0.507219731808, 
			-0.0284153218579, 0.753300700721, -0.664495944977, 
			-0.692949481281, 0.753300633352, 0.0272486507893, 
			-0.345248291778, 0.999093570452, -0.271523624659, 
			0.283150254848, 0.999093570452, -0.33107188344, 
			0.528938805438, 0.753300633352, -0.430145829916, 
			-0.528938805438, 0.753300633352, 0.430145919323, 
			-0.283150192691, 0.999093570452, 0.331071943045, 
			0.345248322856, 0.999093570452, 0.271523594856, 
			0.692949481281, 0.753300633352, -0.0272486060858, 
			-0.717643112163, 0.273249407361, 0.688175678253, 
			0.028415399554, 0.753300700721, 0.664495944977, 
			0.448564588874, 0.753300633352, 0.507219791412, 
			0.71764317432, 0.688175618649, 0.273249373677, 
			-0.305361525634, 0.101219028578, 0.953009307384, 
			0.305361649948, 0.101219062263, 0.95300924778,
			0.993816845531, 0.101219028578, 0.292822957039, 
			0.993816845531, 0.101219028578, -0.292822986841, 
			-0.993816845531, 0.101219028578, 0.292823016644, 
			-0.993816907688, 0.101219062263, -0.292822927237
		};
		
		string vert;
		
		vert = "void main()\n";
		vert += "{\n";
		vert += "gl_TexCoord[0] = gl_MultiTexCoord0;\n";
		vert += "gl_Position = ftransform();\n";
		vert += "gl_Position.y *= -1.;\n";
		vert += "}\n";
		
		string frag;
		
		frag += "uniform sampler2DRect depthtex;\n";
		frag += "uniform float minThreshold;\n";
		frag += "uniform float maxThreshold;\n";
		frag += "uniform float radius;\n";
		frag += "uniform vec2 randSeed;\n";
		frag += "uniform vec3 samples[23];\n";
		
		frag += "float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;\n";
		frag += "	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);\n";
		frag += "}\n";
		
		frag += "float rand(vec2 n){\n";
		frag += "return 0.6 + 0.5 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);\n";
		frag += "}\n";
			
		frag += "void main(void)\n";
		frag += "{\n";
        frag += "vec4 xyzd = texture2DRect( depthtex, gl_TexCoord[0].st );\n";
		frag += "float depth = xyzd.w;//texture2DRect( depthtex, gl_TexCoord[0].st ).w;\n";
		frag += "if( depth == 0. ){	gl_FragColor = vec4(1.); return;}\n";
		
		frag += "float delta, mx, mn;\n";
		frag += "float ao = 0.;\n";
		frag += "float dscl = (1.-depth);\n";
		frag += "dscl *= dscl;\n";
		frag += "mx = maxThreshold * dscl;\n";
		frag += "mn = minThreshold * dscl;\n";
		frag += "float rad = radius * dscl;\n";
		frag += "float rnd = 0.;\n";
		frag += "vec3 ray;\n";
		
		frag += "for(int i=0; i<16; i++){\n";
		frag += "	rnd = rand( gl_FragCoord.xy+randSeed+vec2(i*i));\n";
		frag += "	ray = samples[i].zxy * rad * rnd;\n";
//		frag += "	ray = reflect( -samples[i], xyzd.xyz) * rad * rnd;\n";
		frag += "	delta = ( depth - mn - texture2DRect( depthtex, gl_TexCoord[0].st + ray.xy ).w);\n";
		frag += "	ao += min( 1., ( delta > 0. ) ? delta/max( delta, mx) : (mx - delta)/mx );\n";
		frag += "}\n";
		
		frag += "gl_FragColor = vec4( ao/16. );\n";
		frag += "\n";
    
		frag += "}\n";
		
		daoShader.setupShaderFromSource( GL_VERTEX_SHADER, vert );
		daoShader.setupShaderFromSource( GL_FRAGMENT_SHADER, frag );
		if(!daoShader.linkProgram()) cout<<"SSAO SHADER DIDN'T COMPILE."<<endl;
		
		daoShader.begin();
		daoShader.setUniform3fv("samples", halfSphereSamps, 23*3);
		daoShader.end();
	}
	
	void makeDeferredShader(){
		
		//deferredShader
		string vert;
		
		vert = "varying vec3 norm;\n";
		vert += "void main()\n";
		vert += "{\n";
		vert += "norm = normalize(gl_NormalMatrix * gl_Normal);\n";
		vert += "gl_TexCoord[0] = gl_MultiTexCoord0;\n";
		vert += "gl_FrontColor = gl_Color;\n";
		vert += "gl_Position = ftransform();\n";
		vert += "}\n";
		
		string frag;
		
		frag = "uniform float nearClip;\n";
		frag += "uniform float farClip;\n";
		
		frag += "float getDepth( in float d ) {\n";
		frag += "return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));	\n";
		frag += "}\n";
		
		frag += "varying vec3 norm;\n";
		frag += "void main(void)\n";
		frag += "{\n";
		frag += "gl_FragColor = vec4( norm, getDepth(  gl_FragCoord.z ) );\n";
		frag += "}\n";
		
		deferredShader.setupShaderFromSource( GL_VERTEX_SHADER, vert );
		deferredShader.setupShaderFromSource( GL_FRAGMENT_SHADER, frag );
		if(!deferredShader.linkProgram()) cout<<"THE SSAO.deferredShader SHADER DIDN'T COMPILE."<<endl;
		
		setClipPlanes( nearClip, farClip );
	}
	
	
	void gauss1D( vector <float>& k, int numSamples ){
		k.resize( numSamples );
		float sum = 0;
		int edge = floor( float( numSamples/2 ));
		for(int i=-edge; i<=edge; i++){
			k[i+edge] = pow(sqrt(2.*PI), -(float(i*i)/4.) );
			sum += k[i+edge];
		}
		
		for(int i=0; i<k.size();i++)	k[i] /= sum;
	}
	
	void makeOffsets( vector <float>& s, int numSamples ){
		s.resize( numSamples );
		int half = floor( numSamples/2 );
		for(int i=0; i<numSamples; i++){
			s[i] = float(-half + i)/float(half);
		}
	}
	
	ofShader daoShader, deferredShader, depthBlurShader;
	int width, height;
	float nearClip, farClip;
	ofFbo deferredPass;
	
	ofFbo fbo1, fbo2;
	ofFbo* fboOne;

	vector <float> depthBlurSamples;
	vector <float> depthBlurWeights;
	
    //private:
	float weight, radius, maxThreshold, minThreshold;
	
};