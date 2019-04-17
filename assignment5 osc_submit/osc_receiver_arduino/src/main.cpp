#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1280,720,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
//    ofGLWindowSettings settings;
//    settings.setSize(640, 480);
//    //settings.setGLVersion(3,2); // GL3
//    settings.setGLVersion(2,1); // GL2
//    ofCreateWindow(settings);
    ofRunApp(new ofApp());

}
