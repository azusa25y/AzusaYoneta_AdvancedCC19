#include "ofApp.h"
float radius;
float bg, fg;
ofPoint pos;
const int numPiano = 8;
const int numDrum = 8;
const int numGuitar = 6;
ofSoundPlayer  piano[numPiano];
ofSoundPlayer  drum[numDrum];
ofSoundPlayer guitar[numGuitar];
float volPiano[numPiano];
float volDrum[numDrum];
float volGuitar[numGuitar];
//--------------------------------------------------------------
void ofApp::setup(){
    mGui.setup("oscillator settings");
    mFreqSlider.setup("frequency", 200.0f, 60.0f, 2500.0f);
    mVolumeSlider.setup("volume(amplitude)", 0.2f,0.0f,0.89f);
    
    //ampersand means we pass in the variable location in memory
    //because inside the add function we create a pointer
    // 2 output channels (stereo), 0 input channels
    // 512 samples per buffer, 2 buffers
    
    // Wave
    ofSetVerticalSync(true);
    wavePhase = 0;
    pulsePhase = 0;
    
    // start the sound stream with a sample rate of 44100 Hz, and a buffer
    // size of 512 samples per audioOut() call
    ofSoundStreamSettings settings;
    settings.numOutputChannels = 2;
    settings.sampleRate = 44100;
    settings.bufferSize = 512;
    settings.numBuffers = 4;
    settings.setOutListener(this);
    soundStream.setup(settings);
    
    //Circle
    ofBackground(0,0,0); //background color
    ofEnableAlphaBlending(); //opacity
    ofSetCircleResolution(120); //circlr resolution
    ofSetFrameRate(10); //frame rate per second
    ofSetVerticalSync(true);
    ofSetBackgroundAuto(false);
    
    for ( int i=0; i<numPiano; i++) {
        piano[i].load(
                      "piano" + ofToString( i + 1 ) + ".mp3" );
        piano[i].setLoop( true );
        //Do some stereo panoraming of the sounds
        piano[i].setPan( ofMap( i, 0, numPiano-1, -0.5, 0.5 ) );
        
        piano[i].setVolume( 0 );
        piano[i].play();        //Start a sample to play
    }
    for ( int i=0; i<numDrum; i++) {
        drum[i].load(
                     "drum" + ofToString( i + 1 ) + ".mp3" );
        drum[i].setLoop( true );
        //Do some stereo panoraming of the sounds
        drum[i].setPan( ofMap( i, 0, numDrum-1, -0.5, 0.5 ) );
        
        drum[i].setVolume( 0 );
        drum[i].play();        //Start a sample to play
    }
    for ( int i=0; i<numGuitar; i++) {
        guitar[i].load(
                       "guitar" + ofToString( i + 1 ) + ".mp3" );
        guitar[i].setLoop( true );
        guitar[i].setPan( ofMap( i, 0, numGuitar-1, -0.5, 0.5 ) );
        
        guitar[i].setVolume( 0 );
        guitar[i].play();        //Start a sample to play
    }
    
    //(audio clipping)
    ofSoundSetVolume( 0.2 );
    
}
//--------------------------------------------------------------
void ofApp::update(){
    rotation++;

    unique_lock<mutex> lock(audioMutex);
    
    waveform.clear();
    for(size_t i = 0; i < lastBuffer.getNumFrames(); i++) {
        float sample = lastBuffer.getSample(i, 0);
        float x = ofMap(i, 0, lastBuffer.getNumFrames(), 0, ofGetWidth());
        float y = ofMap(sample, -1, 1, 0, ofGetHeight());
        waveform.addVertex(x, y);
    }
    
    
    rms = lastBuffer.getRMSAmplitude();
    
    
   //Circle
    radius = sin(2.0 * (float)mVolumeSlider) * 250.0f; //Change size with amplitude
    bg = sin (ofGetElapsedTimef() * 2.0) * 127 + 127;
    fg = sin (ofGetElapsedTimef() * 3.0) * 127 + 127;
    pos.x = ofRandom(ofGetWidth());
    pos.y = ofRandom(ofGetHeight());
    
    
    float time = ofGetElapsedTimef();
    float tx = time*0.1 + 50;
    
    for (int i=0; i<numGuitar; i++) {
        float ty = i * 0.2;
        
        volGuitar[i] = ofNoise( tx, ty );
        guitar[i].setVolume( volGuitar[i] );
    }
    for (int i=0; i<numPiano; i++) {
        float ty = i * 0.2;
        
        volPiano[i] = ofNoise( tx, ty );
        piano[i].setVolume( volPiano[i] );
    }
  
    ofSoundUpdate();
}
//--------------------------------------------------------------
void ofApp::draw(){
    //Circle
    ofBackground(bg, bg, bg); //set color for background
    ofSetColor(fg, fg, fg); // set color for circle
    
    if (ofDist(ofGetWidth()/2, ofGetHeight()/2, pos.x, pos.y)<200){
        ofSetColor(255, 63, 63, 127);
        //ofSetColor(118,198,197, 127); mint green
    } else {
        ofSetColor(63, 63, 255, 127);
    }
    
    ofSetLineWidth(1 + (rms * 10.));
    
    waveform.draw();
    for (int i=0; i < ofGetHeight(); i+= 20){
        ofTranslate(0, i);
        waveform.draw();
        ofTranslate(0, -i);
        waveform.draw();
    }
    
    for (int i=0; i<numPiano; i++) {
        int width = ofGetWidth();
        int height = ofGetHeight();
       
        int fx = ofMap(piano[i].getVolume(), 0.0, 1.0, 0.0, ofGetWidth());
        int fy = ofMap(piano[i].getVolume(), 0.0, 1.0, 0.0, ofGetHeight());
        int radius = ofMap(piano[i].getVolume(), 0.0, 1.0, 0.0, 100.0);
        ofRotateDeg(rotation, ofGetWidth()/2, ofGetHeight()/2, 1);
        ofDrawCircle(width/7 * i, height/7, radius);
    }

}
    
void ofApp::audioOut(ofSoundBuffer &outBuffer){
        
    for(int i = 0; i < outBuffer.size(); i += 2) {
        float sample = sin(mPhase*TWO_PI) * (float)mVolumeSlider; // generating a sine wave sample
            outBuffer[i] = sample; // writing to the left channel
            outBuffer[i+1] = sample;
        float phaseOffset = ((float)mFreqSlider / (float)sampleRate);
            mPhase += phaseOffset;
            
        }

    float frequency = 172.5;
    float wavePhaseStep = (frequency / outBuffer.getSampleRate()) * TWO_PI;
    float pulsePhaseStep = (0.5 / outBuffer.getSampleRate()) * TWO_PI;
    
    unique_lock<mutex> lock(audioMutex);
    lastBuffer = outBuffer;
}

    void ofApp::mousePressed(int x, int y, int button){
    float widthStep = ofGetWidth() ;
    for (int i=0; i<numGuitar; i++) {
        guitar[i].setPan(ofMap(x, 0, widthStep, -1, 1, true));
        guitar[i].setSpeed( 0.1f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*10);;
    }
    for (int i=0; i<numPiano; i++) {
        piano[i].setPan(ofMap(x, 0, widthStep, -1, 1, true));
        piano[i].setSpeed( 0.1f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*10);;
    }
        
    
        
}
    void ofApp:: keyPressed(int key) {
    switch (key) {
        case('q'):
            for (int i=0; i<numGuitar; i++) {
                if (guitar[i].isPlaying()) {
                    guitar[i].stop();
                }
                else {
                    guitar[i].play();
                }
            }
            break;
        case('w'):
            for (int i=0; i<numPiano; i++) {
                if (piano[i].isPlaying()) {
                    piano[i].stop();
                }
                else {
                    piano[i].play();
                }
            }
            break;
            
    }
    
}
    void ofApp::mouseMoved(int x, int y ){
    

}
