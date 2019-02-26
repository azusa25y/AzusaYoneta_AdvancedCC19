#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //echo part--------------------------------------------------------
    kennedyVideo.load("kennedy1.mp4");
    ofSetRectMode(OF_RECTMODE_CENTER);
    kennedyVideo.setLoopState(OF_LOOP_NORMAL);
    kennedyVideo.play();
    cam1.load("1.mp4");
    cam2.load("2.mp4");
    cam3.load("3.mp4");
    cam4.load("4.mp4");
    cams.push_back(cam1);
    cams.push_back(cam2);
    cams.push_back(cam3);
    cams.push_back(cam4);
    for (int i = 0; i < 4; i++){
        cams[i].setLoopState(OF_LOOP_NORMAL);
        cams[i].play();
        
    }
    vSpeed = 1.0;
     //azusa part--------------------------------------------------------
    mImage1.load("kennedy.jpg");
    //Lucas
    phase = 0;
    updateWaveform(32);
    ofSoundStreamSetup(1, 0); // mono output
    frequency = 0;
    frequencyTarget = frequency;
    volume = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){
     //echo part--------------------------------------------------------
    kennedyVideo.update();
    for (int i = 0; i < 4; i++){
        cams[i].update();
    }
    //Lucas
    ofScopedLock waveformLock(waveformMutex);
    updateWaveform(ofMap(ofGetMouseX(), 0, ofGetWidth(), 3, 64, true));
    frequency = ofLerp(frequency, frequencyTarget, 0.4);
    if(ofGetKeyPressed()) {
        volume = ofLerp(volume, 1, 0.8); // jump quickly to 1
    } else {
        volume = ofLerp(volume, 0, 0.1); // fade slowly to 0
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
     //echo part--------------------------------------------------------
    echo1();
    kennedyVideo.setSpeed(vSpeed);
    kennedyVideo.draw(ofGetWidth()/2, ofGetHeight()/2);
    for (int i = 0; i < 4; i++){
        
        if (camSwitch[i] == true){
            if (i == 0){
                cams[i].draw(ofGetWidth()/2 - kennedyVideo.getWidth()/4, ofGetHeight()/2 - kennedyVideo.getHeight()/4);
                //std::cout << "worked" << endl;
            } else if (i == 1){
                cams[i].draw(ofGetWidth()/2 + kennedyVideo.getWidth()/4, ofGetHeight()/2 - kennedyVideo.getHeight()/4);
            } else if (i ==2){
                cams[i].draw(ofGetWidth()/2 - kennedyVideo.getWidth()/4, ofGetHeight()/2 + kennedyVideo.getHeight()/4);
            } else if (i ==3){
                cams[i].draw(ofGetWidth()/2 + kennedyVideo.getWidth()/4, ofGetHeight()/2 + kennedyVideo.getHeight()/4);
            }
            
        }
    }
    //Lucas part---------------------------------------------------
    ofSetColor(col);
    
    // azusa part ----------------------------------------
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotateY(ofGetElapsedTimeMillis() *ofGetMouseY()/1000);
    ofRotateX(15.0f);
    mImage1.draw(ofGetMouseX()-200, ofGetMouseY()-200);
    ofPopMatrix();
    
    
    ofSetColor(col);
    
    
}
//--------------------------------------------------------------
//function echo is implemented by Echo
//the function enables speed change of the video according to the speed of the mouse move
void ofApp::echo1(){
    //video speed only change with the mosue when mouse is being dragged
    if (mMove >= 0){
        
        if (mMove >= 80){
            mMove = 80;
        }
        //the only apply new vspeed 4 times per second so that the video is smoother
        int dragNow = ofGetElapsedTimeMillis();
        if ((dragNow - dragStart)%250 == 0){
            vSpeed = ofMap(mMove, 0, 80.1, 0.0, 4.0);
            //round up vspeed leaving only one digit after decimal point to make it look smoother
            vSpeed = roundf(vSpeed*10)/10;
            
            
        }
        
    }
    else {
        vSpeed = 1.0;
        
    }
}
//--------------------------------------------------------------

//Lucas
void ofApp::updateWaveform(int waveformResolution) {
    waveform.resize(waveformResolution);
    waveLine.clear();
    
    // "waveformStep" maps a full oscillation of sin() to the size
    // of the waveform lookup table
    float waveformStep = (M_PI * 2.) / (float) waveform.size();
    
    for(int i = 0; i < waveform.size(); i++) {
        waveform[i] = sin(i * waveformStep);
        
        waveLine.addVertex(ofMap(i, 0, waveform.size() - 1, 0, ofGetWidth()),
                           ofMap(waveform[i], -1, 1, 0, ofGetHeight()));
    }
}

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    ofScopedLock waveformLock(waveformMutex);
    
    float sampleRate = 44100;
    float phaseStep = frequency / sampleRate;
    
    outLine.clear();
    
    for(int i = 0; i < bufferSize * nChannels; i += nChannels) {
        phase += phaseStep;
        int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
        output[i] = waveform[waveformIndex] * volume;
        
        outLine.addVertex(ofMap(i, 0, bufferSize - 1, 0, ofGetWidth()),
                          ofMap(output[i], -1, 1, 0, ofGetHeight()));
    }
    
}


void ofApp::keyPressed(int key){
     //echo part--------------------------------------------------------
    if (key == '1' || key == '2' || key == '3' || key == '4'){
        cam = true;
        if (key == '1'){
            cams[0].setSpeed(vSpeed);
            camSwitch[0] = true;
        } else if (key == '2'){
            cams[1].setSpeed(vSpeed);
            camSwitch[1] = true;
        } else if (key == '3'){
            cams[2].setSpeed(vSpeed);
            camSwitch[2] = true;
        }else if (key == '4'){
            cams[3].setSpeed(vSpeed);
            camSwitch[3] = true;
        }
        
        
    }
    //azusa part --------------------------
    switch(key){
        case OF_KEY_LEFT:
            kennedyVideo.previousFrame();
            break;
        case OF_KEY_RIGHT:
            kennedyVideo.nextFrame();
            break;
            
            //Lucas Part
        case 'a':
            col = ofColor(255,0,0);
            frequencyTarget = 261.63; //C
            break;
        case 's':
            col = ofColor(255,125,0);
            frequencyTarget = 293.67; // D
            break;
        case 'd':
            col = ofColor(255,255,0);
            frequencyTarget = 329.63; // E
            break;
        case 'f':
            col = ofColor(0,255,0);
            frequencyTarget = 349.23; // F
            break;
        case 'g':
            col = ofColor(0,255,255);
            frequencyTarget = 392.00; // G
            break;
        case 'h':
            col = ofColor(0,0,255);
            frequencyTarget = 440.00; // A
            break;
        case 'j':
            col = ofColor(125,0,255);
            frequencyTarget = 493.88; // B
            break;
        case 'k':
            col = ofColor(255,0,255);
            frequencyTarget = 523.25;
            break;
        case 'l':
            col = ofColor(125,125,125);
            frequencyTarget = 587.33;
            break;
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
     //echo part--------------------------------------------------------
    dragStart = ofGetElapsedTimeMillis();
    ofVec2f m1(mouseX, mouseY);
    ofVec2f m2(ofGetPreviousMouseX(), ofGetPreviousMouseY());
    mMove = m1.squareDistance(m2);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
 // azusa part ------------
//    if(!frameByframe){
//        kennedyVideo.setPaused(true);
//    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
     //echo part--------------------------------------------------------
    mMove = -1;
    //azusa part ---------
//    if(!frameByframe){
//        kennedyVideo.setPaused(false);
//    }
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
