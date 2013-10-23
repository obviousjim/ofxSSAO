
/*
 //TODO:
 
 -getters & setters for depthNormalTexture, colorTexture. If NULL default to
 
 -get blend state and blendfunctions to revert to
 
 -generate hemisphere vectors more randomly
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	randomPositions.resize(100);
	randomColors.resize(randomPositions.size() );
	
	for (int i=0; i<randomPositions.size(); i++) {
		randomPositions[i].set( ofRandom(-200, 200), ofRandom(-200, 200), ofRandom(-200, 200) );
		randomColors[i].set( ofRandom(0, 1.5), ofRandom(0, 1.5), ofRandom(0, 1.5));
	}
	
	//ssao
	ssao.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
	ofSetWindowTitle( ofToString(ofGetFrameRate() ) );
}

//--------------------------------------------------------------
void ofApp::draw()
{

	ssao.begin( camera.getNearClip(), camera.getFarClip() );
	camera.begin();
	
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ZERO);
	
	float elapsedTime = ofGetElapsedTimef();
	float step = 1. / float(randomPositions.size());
	for (int i=0; i<randomPositions.size(); i++) {
		ofSetColor(randomColors[i]);
		
//        if(i%2){
			ofDrawSphere(randomPositions[i].x, randomPositions[i].y, randomPositions[i].z, 40 + sin( elapsedTime + float(i) * step)*20 );
//        }
//        else{
//			glPushMatrix();
//			glTranslatef( randomPositions[i].x, randomPositions[i].y, randomPositions[i].z );
//			glRotatef( (elapsedTime*10 +i)*10, sin(i), 0, cos(i) );
//			ofDrawBox(0,0,0, 40 + sin( elapsedTime + float(i) * step)*20 );
//			glPopMatrix();
//        }
	}
	
	camera.end();
	
	ssao.end();
	
	ofSetColor(255);
	ssao.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
