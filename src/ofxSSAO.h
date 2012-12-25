/*
 *  ofxSSAO.h
 *  blt_motionStudy
 *
 *  Created by lars berg on 2/10/12.
 *  Copyright 2012 lablab. All rights reserved.
 *
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
	
	void setClipPlanes( float near, float far );
	
	void begin();
	
	void end( bool drawToFbo=true, float elapsedTime = ofGetElapsedTimef() );
	
	
	void draw(int x=0, int y=ofGetHeight(), int w=ofGetWidth(), int h = -ofGetHeight());
	
	void makeDaoShader();
	
	void makeDeferredShader();
	
	void gauss1D( vector <float>& k, int numSamples );
    
	void makeOffsets( vector <float>& s, int numSamples );
    
  private:
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