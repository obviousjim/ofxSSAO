//
//  ofxSSAO.cpp
//  ofxSSAO
//
//  Created by lars on 10/22/13.
//
//

#include "ofxSSAO.h"

ofxSSAO::ofxSSAO()
{
	radius = 50;
	maxThreshold = .1;
	minThreshold = .01;
	exponent = .6;
	numSamples = 24;
	reflectRays = true;
	weight = .9;
	
	bIsSetup = false;
	
	colorTexture = NULL;
};
ofxSSAO::~ofxSSAO(){};

void ofxSSAO::setup(int w, int h, int format )
{
	depthNormalPass.allocate( w, h, GL_RGBA);//color
	depthNormalPass.createAndAttachTexture(format, 1);//normal & depth == rgb & a
	ssaoFbo.allocate( w, h, GL_RGBA);//ssao output
	
	//shaders
	loadShaders();
	
	bIsSetup = true;
}
void ofxSSAO::draw(int x, int y, int w, int h)
{
	ssaoFbo.draw(x,y,w,h);
}

void ofxSSAO::begin( float nearClip, float farClip		)
{
	if(!bIsSetup){
		setup();
	}
	depthNormalPass.begin();
	depthNormalPass.activateAllDrawBuffers();
	ofClear(255, 255, 255,0);
	
	depthNormalShader.begin();
	depthNormalShader.setUniform1f("nearClip", nearClip );
	depthNormalShader.setUniform1f("farClip", farClip );
}
void ofxSSAO::end()
{
	depthNormalShader.end();
	depthNormalPass.end();
	
	ssaoFbo.begin();
	ofClear(0,0,0,0);
	float elapsedTime = ofGetElapsedTimef();
	ssaoShader.begin();
	ssaoShader.setUniform1f("numSamples", numSamples);
	ssaoShader.setUniform1i("reflectRays", reflectRays );
	ssaoShader.setUniform1f("maxThreshold", maxThreshold );
	ssaoShader.setUniform1f("minThreshold", minThreshold );
	ssaoShader.setUniform1f("radius", radius );
	ssaoShader.setUniform1f("exponent", exponent );
	ssaoShader.setUniform2f("randSeed", elapsedTime, elapsedTime);
	ssaoShader.setUniform1f("weight", weight);
	
	if(colorTexture == NULL)	ssaoShader.setUniformTexture("colorTex", depthNormalPass.getTextureReference(0), 0);
	else	ssaoShader.setUniformTexture("colorTex", *colorTexture, 0);
	
	ssaoShader.setUniformTexture("normalDepthTex", depthNormalPass.getTextureReference(1), 1);
	
	depthNormalPass.draw(0, 0, ofGetWidth(), ofGetHeight() );
	
	ssaoShader.end();
	ssaoFbo.end();
}

void ofxSSAO::loadShaders(){
	//depthNormal shader
	depthNormalShader.load("shaders/ssao_depthNormalColor");
	
	//ssao shader
	ssaoShader.load("shaders/ssao_render");
	
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


/**
 *use this to set the color texture used in the ssao_render shader to another texture.
 if your doing multiple passes this is a way to pass in a more interesting lighting or whatever
 **/
void ofxSSAO::setColorTexture( ofTexture* _colorTexture )
{
	colorTexture = _colorTexture;
}

ofTexture& ofxSSAO::getSSAOTexture(){
	return ssaoFbo.getTextureReference();
}
ofTexture& ofxSSAO::getColorTexture(){
	return depthNormalPass.getTextureReference(0);
}

void ofxSSAO::setRadius(float _radius){
	radius = _radius;
}
void ofxSSAO::setMaxThreshold(float _maxThreshold)
{
	maxThreshold = _maxThreshold;
}
void ofxSSAO::setMinThreshold(float _minThreshold)
{
	minThreshold = _minThreshold;
}
void ofxSSAO::setExponent(float _exponent)
{
	exponent = _exponent;
}
void ofxSSAO::setNumSamples(int _numSamples)
{
	numSamples = _numSamples;
}
void ofxSSAO::setRayReflection( bool bUseRayReflection )
{
	reflectRays = bUseRayReflection;
}

float& ofxSSAO::getRadius()
{
	return radius;
}

float& ofxSSAO::getMinThreshold()
{
	return minThreshold;
}

float& ofxSSAO::getMaxThreshold()
{
	return maxThreshold;
}

float& ofxSSAO::getExponent()
{
	return exponent;
}
float& ofxSSAO::getNumSamples()
{
	return numSamples;
}
bool& ofxSSAO::getRayReflection()
{
	return reflectRays;
}