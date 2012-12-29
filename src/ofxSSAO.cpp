/*
 *  ofxSSAO.h
 *  Created by lars berg on 2/10/12.
 */


#include "ofxSSAO.h"

ofxSSAO::ofxSSAO(){
    nearClip = 1;
    farClip = 1000;
    
    weight = 1.;
    radius = 75;
    maxThreshold = .1;
    minThreshold = .01;
    exponent = 1.;
    numSamples = 24;
    reflectRays = 1;
}

ofxSSAO::~ofxSSAO(){};

void ofxSSAO::setup( int w, int h, int format){
    width = w;
    height = h;
    
    //deferred
    ofFbo::Settings deferredBuffersSettings;
    deferredBuffersSettings.width = w;
    deferredBuffersSettings.height = h;
    deferredBuffersSettings.internalformat = format;
    deferredBuffersSettings.numColorbuffers = 2;
    deferredBuffersSettings.useDepth = true;
    deferredBuffersSettings.useStencil = true;
    deferredBuffersSettings.depthStencilAsTexture = true;
    deferredBuffersSettings.textureTarget = ofGetUsingArbTex() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
    
    deferredPass.allocate(deferredBuffersSettings);
    makeDeferredShader();
    
    ssaoFbo.allocate(w, h, GL_RGB, 0);
    makeSsaoShader();
	
	composite.load("shaders/ssao_composite");
}

void ofxSSAO::setWeight(float _weight){
    weight = _weight;
}
void ofxSSAO::setRadius(float _radius){
    radius = _radius;
}
void ofxSSAO::setMaxThreshold(float _maxThreshold){
    maxThreshold = _maxThreshold;
}
void ofxSSAO::setMinThreshold(float _minThreshold){
    minThreshold = _minThreshold;
}
void ofxSSAO::setNumSamples( int _numSamples ){
    numSamples = min( 24, _numSamples );
}
void ofxSSAO::setExponent( float _exponent ){
    exponent = (_exponent == .5)? _exponent : .501;
}

void ofxSSAO::setClipPlanes( float near, float far ){
    nearClip = near;
    farClip = far;
}

void ofxSSAO::setRayReflection( bool bUseRayReflection ){
    reflectRays = ( bUseRayReflection == true );
}

void ofxSSAO::begin(){
    
    deferredPass.begin();
    deferredPass.activateAllDrawBuffers();
    //TODO:: store previous DEPTH_TEST state
    glClearColor(1,1,1,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    deferredShader.begin();
}


void ofxSSAO::end( float elapsedTime ){
    deferredShader.end();
    deferredPass.end();
    
//    ssaoFromDepthTexture( deferredPass.getDepthTexture(), elapsedTime );
    ssaoFromDepthAndNormaTextures( deferredPass.getDepthTexture(), deferredPass.getTextureReference(1), elapsedTime );

    
    //TODO:: re-store previous DEPTH_TEST state
}

void ofxSSAO::ssaoFromDepthTexture( ofTexture& depthTex, float elapsedTime ){
    setRayReflection( false );
    
    glColor3f(1, 1, 1);
    ssaoFbo.begin();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    ssaoShader.begin();
    
    ssaoShader.setUniformTexture("depthtex", depthTex, 1);
    setSsaoUniforms( elapsedTime );
    
    deferredPass.draw(0, 0, ssaoFbo.getWidth(), ssaoFbo.getHeight());
    
    ssaoShader.end();
    ssaoFbo.end();
    
}


void ofxSSAO::ssaoFromDepthAndNormaTextures( ofTexture& depthTex, ofTexture& normTex, float elapsedTime ){
    setRayReflection( true );
    
    glColor3f(1, 1, 1);
    ssaoFbo.begin();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    ssaoShader.begin();
    
    ssaoShader.setUniformTexture("normaltex", normTex, 0);
    ssaoShader.setUniformTexture("depthtex", depthTex, 1);
    
    setSsaoUniforms( elapsedTime );
    
	//TODO:: use a rect without texture associated with it
    deferredPass.draw(0, 0, ssaoFbo.getWidth(), ssaoFbo.getHeight() );
    
    ssaoShader.end();
    ssaoFbo.end();
    
}

void ofxSSAO::setSsaoUniforms( float elapsedTime ){
    ssaoShader.setUniform1f("nearClip", nearClip);
    ssaoShader.setUniform1f("farClip", farClip);
    ssaoShader.setUniform1f("numSamples", numSamples);
    ssaoShader.setUniform1i("reflectRays", reflectRays );
    ssaoShader.setUniform1f("maxThreshold", maxThreshold );
    ssaoShader.setUniform1f("minThreshold", minThreshold );
    ssaoShader.setUniform1f("radius", radius );
    ssaoShader.setUniform1f("exponent", exponent );
    ssaoShader.setUniform2f("randSeed", elapsedTime, elapsedTime);
}

void ofxSSAO::draw(int x, int y, int w, int h){
    glColor4f(1,1,1,1);
//    ssaoFbo.draw( x, y+h, w, -h);
	
	composite.begin();
	composite.setUniformTexture("ssaoTex", ssaoFbo.getTextureReference(), 0 );
	composite.setUniformTexture("colorTex", deferredPass.getTextureReference(0), 1);
	
	ssaoFbo.draw( x, y+h, w, -h);
	
	composite.end();
	
}

void ofxSSAO::makeSsaoShader(){
    
    //some points distributed on half a sphere
    float halfSphereSamps[] = {
        -0.717643236477, -0.688175618649, 0.273249441045,
        -0.305361618869, -0.95300924778, 0.101219129631,
        0.305361649948, -0.95300924778, 0.101219129631,
        0.717643236477, -0.688175618649, 0.273249441045,
        -0.448564588874, -0.507219731808, 0.753300700721,
        -0.0284153218579, -0.664495944977, 0.753300700721,
        -0.692949481281, 0.0272486507893, 0.753300633352,
        -0.345248291778, -0.271523624659, 0.999093570452,
        0.283150254848, -0.33107188344, 0.999093570452,
        0.528938805438, -0.430145829916, 0.753300633352,
        -0.528938805438, 0.430145919323, 0.753300633352,
        -0.283150192691, 0.331071943045, 0.999093570452,
        0.345248322856, 0.271523594856, 0.999093570452,
        0.692949481281, -0.0272486060858, 0.753300633352,
        -0.717643112163, 0.688175678253, 0.273249407361,
        0.028415399554, 0.664495944977, 0.753300700721,
        0.448564588874, 0.507219791412, 0.753300633352,
        0.71764317432, 0.273249373677, 0.688175618649,
        -0.305361525634, 0.953009307384, 0.101219028578,
        0.305361649948, 0.95300924778, 0.101219062263,
        0.993816845531, 0.292822957039, 0.101219028578,
        0.993816845531, -0.292822986841, 0.101219028578,
        -0.993816845531, 0.292823016644, 0.101219028578,
        -0.993816907688, -0.29282292723, 0.101219062263
    };

    if(!ssaoShader.load("shaders/ssao_render")){
        ofLogError("ofxSSAO render shader failed to load");
    }
    
    ssaoShader.begin();
    ssaoShader.setUniform3fv("samples", halfSphereSamps, 24*3);
    ssaoShader.end();
}

void ofxSSAO::makeDeferredShader(){
    
    if(!deferredShader.load("shaders/ssao_deferred")){
        ofLogError("ofxSSAO deferred shader failed to load");
    }
    
    setClipPlanes( nearClip, farClip );
}
