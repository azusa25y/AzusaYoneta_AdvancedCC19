#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void audioOut( ofSoundBuffer &outBuffer );
    double mPhase = 0;
    ofxFloatSlider mFreqSlider;
    ofxFloatSlider mVolumeSlider;
    ofxPanel mGui;
    int sampleRate = 44100;
    void mouseMoved(int x, int y );
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    double wavePhase;
    double pulsePhase;
    
    std::mutex audioMutex;
    ofSoundStream soundStream;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    ofPolyline waveform2;
    float rms;
    
    float rotation = 0.0;
    int rotationAmount = 0.0;
    float rotateSpeed = 1;

    
};
