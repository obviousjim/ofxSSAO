//
//  ofxSSAO.h
//  ofxSSAO
//
//  Created by lars on 10/22/13.
//
//

#include "ofMain.h"


class ofxSSAO{
public:
	ofxSSAO();
	~ofxSSAO();
	
	void setup(int w=ofGetWidth(), int h=ofGetHeight(), int format = GL_RGBA16F );
	
	void draw(int x=0, int y=0, int w=ofGetWidth(), int h=ofGetHeight());
	
	void begin( float nearClip, float farClip);
	
	void end();
	
	void loadShaders();
	
	void setColorTexture( ofTexture* _colorTexture );
	ofTexture& getSSAOTexture();
	ofTexture& getColorTexture();
	
    void setWeight(float _weight);
    void setRadius(float _radius);
    void setMaxThreshold(float _maxThreshold);
    void setMinThreshold(float _minThreshold);
    void setExponent(float _exponent);
    void setNumSamples(int _numSamples);
    void setRayReflection( bool bUseRayReflection );
	
	float& getWeight();
	float& getRadius();
    float& getMaxThreshold();
    float& getMinThreshold();
    float& getExponent();
    float& getNumSamples();
    bool& getRayReflection();
	
	
	ofFbo depthNormalPass, ssaoFbo;
	ofShader depthNormalShader, ssaoShader;
	
	float radius, maxThreshold , minThreshold , exponent, numSamples, weight;
	bool reflectRays;
	
	bool bIsSetup;
	
	ofTexture* colorTexture;
	
	vector <ofVec3f> sampleVectors;
};