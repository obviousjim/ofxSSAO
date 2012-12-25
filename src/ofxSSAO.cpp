/*
 *  ofxSSAO.h
 *  blt_motionStudy
 *
 *  Created by lars berg on 2/10/12.
 *  Copyright 2012 lablab. All rights reserved.
 *
 */


#include "ofxSSAO.h"

ofxSSAO::ofxSSAO(){
    nearClip = 1;
    farClip = 1000;
    
    weight = 1.;
    radius = 50;
    maxThreshold = .1;
    minThreshold = .01;
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
    //deferredPass.allocate(w, h, format, 0);//msaa gives us weird edges...
    makeDeferredShader();
    
    // depth blur
    fbo1.allocate(w, h, GL_RGB, 0);
    makeDaoShader();
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

void ofxSSAO::setClipPlanes( float near, float far ){
    nearClip = near;
    farClip = far;
}

void ofxSSAO::begin(){
    vector<int> drawBuffers;
    drawBuffers.push_back(0);
    drawBuffers.push_back(1);
    
    deferredPass.begin();
    deferredPass.activateAllDrawBuffers();
    glClearColor(1,1,1,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    deferredShader.begin();
}

void ofxSSAO::end( bool drawToFbo, float elapsedTime){
    deferredShader.end();
    deferredPass.end();
    
    glColor3f(1, 1, 1);
    glDisable( GL_DEPTH_TEST );
    
    
    if(drawToFbo)   fbo1.begin();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    daoShader.begin();
    daoShader.setUniformTexture("colortex", deferredPass.getTextureReference(0), 0);
    daoShader.setUniformTexture("normaltex", deferredPass.getTextureReference(1), 1);
    daoShader.setUniformTexture("depthtex", deferredPass.getDepthTexture(), 2);
    daoShader.setUniform1f("nearClip", nearClip);
    daoShader.setUniform1f("farClip", farClip);
    
    daoShader.setUniform1f("maxThreshold", maxThreshold );
    daoShader.setUniform1f("minThreshold", minThreshold );
    daoShader.setUniform1f("radius", radius );
    daoShader.setUniform1f("weight", weight );
    daoShader.setUniform2f("randSeed", elapsedTime * ofRandom(.1, 1.), elapsedTime * ofRandom(.1, 1.));
    deferredPass.draw(0, 0, fbo1.getWidth(), fbo1.getHeight() );
    daoShader.end();
    if(drawToFbo)   fbo1.end();
}


void ofxSSAO::draw(int x, int y, int w, int h){
    glColor4f(1,1,1,1);
    fbo1.draw( x, y+h, w, -h);
}

void ofxSSAO::makeDaoShader(){
    
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
    
    
    if(!daoShader.load("shaders/ssao_render")){
        ofLogError("ofxSSAO render shader failed to load");
    }
    
    daoShader.begin();
    daoShader.setUniform3fv("samples", halfSphereSamps, 23*3);
    daoShader.end();
}

void ofxSSAO::makeDeferredShader(){
    
    if(!deferredShader.load("shaders/ssao_deferred")){
        ofLogError("ofxSSAO deferred shader failed to load");
    }
    
    setClipPlanes( nearClip, farClip );
}


void ofxSSAO::gauss1D( vector <float>& k, int numSamples ){
    k.resize( numSamples );
    float sum = 0;
    int edge = floor( float( numSamples/2 ));
    for(int i=-edge; i<=edge; i++){
        k[i+edge] = pow(sqrt(2.*PI), -(float(i*i)/4.) );
        sum += k[i+edge];
    }
    
    for(int i=0; i<k.size();i++)	k[i] /= sum;
}

void ofxSSAO::makeOffsets( vector <float>& s, int numSamples ){
    s.resize( numSamples );
    int half = floor( numSamples/2 );
    for(int i=0; i<numSamples; i++){
        s[i] = float(-half + i)/float(half);
    }
}
