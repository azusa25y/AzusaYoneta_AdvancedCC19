#include "ofApp.h"
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLog() << "listening for osc messages on port " << PORT;
    receiver.setup(PORT);
    ofSetFrameRate(25);
    
    
    //char* video;
    for (int i = 0; i < 2; i++)
    {
        videos[i].load(ofToString(i) + ".mp4");
    }
    ofBackground(255,255,255);

    lastTime = ofGetElapsedTimef();
    
    //blur function
    blur.setup(1280, 720, 10, .2, 2);
}

//--------------------------------------------------------------
void ofApp::update(){
    videos[0].update();
    videos[1].update();

    //receive OSC message
    while (receiver.hasWaitingMessages()){
        ofxOscMessage m;
        //ofxOscMessage duration;
        receiver.getNextMessage(m);
        //receiver2.getNextMessage(duration);
        if(m.getAddress() == "/distance/0"){
            //sensorValue = m.getArgAsInt32(0);
            sensorValue = m.getArgAsFloat(0);
            ofLog(OF_LOG_NOTICE) << "sensorValue=" << sensorValue;
        }
        if (m.getAddress() == "/duration/0"){
            signalDuration = m.getArgAsFloat(0);
            ofLog(OF_LOG_NOTICE) << "signalDuration=" << signalDuration;
        }
    }

    if (sensorValue < 200 && sensorValue > 0){
        blurScale_close = ofMap(sensorValue,1,200,1,0);
        blur.setScale(blurScale_close);
       
        videos[0].setPaused(true);
        videos[1].play();
        videoNumb = 1;
        totalFrame1 = videos[1].getTotalNumFrames();
        if (ofGetElapsedTimef() - lastTime > signalDuration ){
            videos[1].setFrame(ofRandom(totalFrame1-1));
            lastTime = ofGetElapsedTimef();
            //videos[1].update();
        }
    } else if (sensorValue > 200){
        blur.setScale(0);
        videoNumb = 0;
        videos[1].setPaused(true);
        videos[0].setPaused(false);
           //videos[0].update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(0);
    blur.begin();
    videos[videoNumb].draw(0,0);
    blur.end();
    blur.draw();
        //video[1].update();
        //ofLogNotice() << playFrame0;
        
        //set the randomframe and its play duration
        //    if (sensorValue < 200) {
        //        currentFrame = video[0].getCurrentFrame();
        //        signalStart = ofGetElapsedTimef();
        //        counter++;
        //        ofLogNotice() << counter;
        //        //counter/25
        //        playFrame0 = ofRandom(0,totalFrame0-1);
        //        video[0].setFrame(playFrame0);
        //        if (ofGetElapsedTimef() - signalStart > floor(counter/25)){
        //            playFrame0 = ofRandom(0,totalFrame0-1);
        //            video[0].setFrame(playFrame0);
        //            //lastTime = ofGetElapsedTimef();
        //        }
        //    } else if (!signal){
        //        signalEnd = ofGetElapsedTimef();
        //        signalDuration = signalEnd - signalStart;
        //        counter = 0;
        //    }
        //
}

//--------------------------------------------------------------
//void ofApp::vertigo(){
//    ofTranslate(300,300);
//    ofSetLineWidth(15);
//    ofEnableSmoothing();
//
//    for (int i = 0; i < 20; i++){
//        //ofRotateRad(sin(a*0.001)*10);
//        ofRotateRad(1);
//        ofScale(0.8);
//        ofFill();
//        ofSetColor(10,15);
//        ofDrawEllipse(0,0,1200,1200+sin(c*0.01)*120);
//        ofNoFill();
//        ofSetColor(245);
//    ofDrawEllipse(0,0,1200+sin(c*0.005)*120,1200+sin(c*0.015)*120);
//    }
//    c++;
//    a++;
//}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key){
        case '1' :
            sensorValue++;
            break;
        case '2' :
            sensorValue--;
    }
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
