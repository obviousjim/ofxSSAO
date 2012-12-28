/*
 *  ofxSSAO.h
 *  Created by lars berg on 2/10/12.
 */


#pragma once

#include "ofMain.h"

class ofxSSAO{
public:
	
	ofxSSAO();
	~ofxSSAO();
	
	void setup( int w = (int)ofGetWidth(), int h = (int)ofGetHeight(), int format = GL_RGBA32F );
    void setWeight(float _weight);
    void setRadius(float _radius);
    void setMaxThreshold(float _maxThreshold);
    void setMinThreshold(float _minThreshold);
    void setExponent(float _exponent);
    void setNumSamples(int _numSamples);
	
	void setClipPlanes( float near, float far );
	
    void setSaaoUniforms(float elapsedTime);
	void begin();
	
	void end( float elapsedTime = ofGetElapsedTimef(), bool drawToFbo=true );
	
	
	void draw(int x=0, int y=ofGetHeight(), int w=ofGetWidth(), int h = -ofGetHeight());
	
	void makeSsaoShader();
	
	void makeDeferredShader();
    
  private:
	ofShader ssaoShader, deferredShader;
    
	int width, height;
	float nearClip, farClip, numSamples, exponent;
	ofFbo deferredPass;
	
	ofFbo ssaoFbo, colorFbo;
	
    //private:
	float weight, radius, maxThreshold, minThreshold;
    int reflectRays;
	
};