#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    centers.resize( 100 );
    for (int i=0; i<centers.size(); i++) {
        centers[i].set( ofRandom(-150, 150), ofRandom(-150, 150), ofRandom(-150, 150) );
    }
    
    ssao.setup( ofGetWidth(), ofGetHeight(), GL_RGBA16F );
    ssao.setRadius( 70 );
    ssao.setMaxThreshold( .05 );
    ssao.setMinThreshold( .01 );
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    float elapsedTime = ofGetElapsedTimef();
    ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
    
    ssao.begin();
    camera.begin();
    float step = PI * 2./float(centers.size() );
    for(int i=0; i<centers.size(); i++){
        ofSphere(centers[i].x, centers[i].y, centers[i].z, 40 + sin( elapsedTime + float(i) * step)*20 );
    }
    camera.end();
    ssao.end();

    ssao.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}