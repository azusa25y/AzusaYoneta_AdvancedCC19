#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#include "MotionAmplifier.h"

#include "opencv2/opencv.hpp"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key) {};
    void mouseMoved(int x, int y) {};
    void mouseDragged(int x, int y, int button) {};
    void mousePressed(int x, int y, int button) {};
    void mouseReleased(int x, int y, int button) {};
    void windowResized(int w, int h) {};
    void dragEvent(ofDragInfo dragInfo) {};
    void gotMessage(ofMessage msg) {};
    

    ofVideoPlayer video;
    MotionAmplifier motionAmplifier;
    
    ofxCvColorImage image;       //The current video frame
    
    //The current and the previous video frames as grayscale images
    ofxCvGrayscaleImage grayImage, grayImagePrev;
    
    ofxCvGrayscaleImage diff;        //Absolute difference of the frames
    ofxCvFloatImage diffFloat;        //Amplified difference images
    ofxCvFloatImage bufferFloat;    //Buffer image
    ofxCvColorImage imageDecimated;
    ofxCvGrayscaleImage blurred;
    ofxCvGrayscaleImage mask;
    ofxCvContourFinder     contourFinder;
    ofxCvGrayscaleImage background;
    
    vector<ofPoint> obj;    //object's centers
    
    int drawMode;    //Drawing mode,
    //0 - image processing
    //1 - additional rendering
    void generateImg( int w, int h );    //Additional rendering
    ofImage img;
    
    //azusa---------------------------------------
    ofxCvColorImage            colorImg;
    ofxCvGrayscaleImage     grayImage2;
    ofxCvGrayscaleImage     grayBg;
    ofxCvGrayscaleImage     grayDiff;
    
    int                 threshold;
    bool                bLearnBakground;
    bool                bShowVideo = true;
    template<typename T>
    class ofColor_;
    ofColor col = ofColor(255,165,0);

};


