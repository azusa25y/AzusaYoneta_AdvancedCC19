#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        //echo part---------------------------------------------------
        void echo1();
    
        ofVideoPlayer kennedyVideo;
    
        ofVideoPlayer cam1;
        ofVideoPlayer cam2;
        ofVideoPlayer cam3;
        ofVideoPlayer cam4;
    
        //a variable that represents the vector of mouse movement
        //if the mouse is not dragged the value is automatically negative
        float mMove = -1;
        //speed of video
        float vSpeed;
        //timestamp of everytime mousedrag start
        int dragStart;
    
        //initialize an array for storing all the videos
        vector<ofVideoPlayer> cams;
        vector<Boolean> camSwitch = {false, false, false, false};
        //a boolean to enable the echo2 function
        Boolean cam = false;
    
    // azusa part ----------------
    bool bIsAllPaused = true;
    bool frameByframe;
    ofImage mImage1;
    
    //Lucas part-----------------------------------
    float frequencyTarget;
    float volume;
    
    void updateWaveform(int waveformResolution);
    void audioOut(float * output, int bufferSize, int nChannels);
    
    std::vector<float> waveform; // this is the lookup table
    double phase;
    float frequency;
    
    ofMutex waveformMutex;
    ofPolyline waveLine;
    ofPolyline outLine;
    
    ofVideoPlayer bgVideo;
    
    ofColor col = ofColor(255,255,255);
    
};
