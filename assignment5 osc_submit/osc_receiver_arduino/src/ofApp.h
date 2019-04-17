#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxBlur.h"
//#include "ofxBlurShader.h"

#define PORT 9001

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
    
//        void vertigo();
//        int c=0, a=0;
    
       ofxOscReceiver receiver;
       float sensorValue=210;
       float signalDuration=2;
    
       ofxBlur blur;
       float blurScale_close;
    
       ofVideoPlayer   videos[2];
       int videoNumb = 0;
    
       int  totalFrame0, totalFrame1, playFrame0, playFrame1;
       int  currentFrame;
       int  lastTime, signalStart, signalEnd;
       int  counter=0;
       bool signal;
    
       //ofVideoGrabber camera;
       //ofxBlurShader blur;
};
