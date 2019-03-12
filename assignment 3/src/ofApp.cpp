#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    video.loadMovie("dance.mp4");
    video.play();
    drawMode = 0;
    motionAmplifier.setup(ofGetWindowWidth()/2, ofGetWindowHeight()/2, 2, .25);
    //azusa
    glm::ivec2 videoSize = glm::ivec2(video.getWidth(),video.getHeight());
    
    colorImg.allocate(videoSize.x,videoSize.y);
    grayImage.allocate(videoSize.x,videoSize.y);
    grayBg.allocate(videoSize.x,videoSize.y);
    grayDiff.allocate(videoSize.x,videoSize.y);
    
    bLearnBakground = true;
    threshold = 40;
    
    
}



void ofApp::update() {
    motionAmplifier.setStrength(ofMap(mouseX, 0, ofGetWidth(), -50, 10));
    motionAmplifier.setLearningRate(ofMap(mouseY, 0, ofGetHeight(), 0,2, true));
    motionAmplifier.setBlurAmount(0);
    motionAmplifier.setWindowSize(15);
    
	video.update();
    
    //azusa------------------------------------------------------------------
    bool bNewFrame = false;
    bNewFrame = video.isFrameNew();
    ofSetColor(col);
    if (bNewFrame){
        colorImg.setFromPixels(video.getPixels());
        grayImage2 = colorImg;
        if (bLearnBakground == true){
            grayBg = grayImage2;        // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage2);
        grayDiff.threshold(threshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);    // find holes
    }
    
    
    
	if(video.isFrameNew()) {
        motionAmplifier.update(video);
	}

    if ( video.isFrameNew() ) {
        motionAmplifier.update(video);
        //Store the previous frame, if it exists till now
        if ( grayImage.bAllocated ) {
            grayImagePrev = grayImage;
        }
        
        //Getting a new frame
        image.setFromPixels( video.getPixelsRef() );
        grayImage = image;    //Convert to grayscale image
        
        //Do processing if grayImagePrev is inited
        if ( grayImagePrev.bAllocated ) {
            //Get absolute difference
            diff.absDiff( grayImage, grayImagePrev );
            
            //We want to amplify the difference to obtain
            //better visibility of motion
            //We do it by multiplication. But to do it, we
            //need to convert diff to float image first
            diffFloat = diff;    //Convert to float image
            diffFloat *= 7.0;    //Amplify the pixel values
            
            //Update the accumulation buffer
            if ( !bufferFloat.bAllocated ) {
                //If the buffer is not initialized, then
                //just set it equal to diffFloat
                bufferFloat = diffFloat;
            }
            else {
                //Slow damping the buffer to zero
                bufferFloat *= 0.85;
                //Add current difference image to the buffer
                bufferFloat += diffFloat;
            }
        }
    }
    
    video.update();    //Decode the new frame if needed
    
    //Do computing only if the new frame was obtained
    if ( video.isFrameNew() ) {
        //Getting a new frame
        image.setFromPixels( video.getPixelsRef() );
        
        //Decimate image for speed-up processing
        if ( !imageDecimated.bAllocated ) {
            imageDecimated.allocate( image.width * 1,
                                    image.height * 1 );
        }
        imageDecimated.scaleIntoMe( image, CV_INTER_NN );
        
        //Convert to grayscale image
        grayImage = imageDecimated;
        
        //Smoothing image
        blurred = grayImage;
        blurred.blurGaussian( 9 );
        
        //Store first frame to background image
        if ( !background.bAllocated ) {
            background = blurred;
        }
        
        //Find difference of the frame and background
        diff = blurred;
        diff -= background;
        
        //Thresholding for obtaining binary image
        mask = diff;
        mask.threshold( 40 );
        //Here value 40 is the threshold parameter.
        //It should be adjusted for good results when using another videos than in example.
        
        //Find contours
        contourFinder.findContours( mask, 10, 10000, 20, false );
        
        //Store objects' centers
        vector<ofxCvBlob>  &blobs = contourFinder.blobs;
        int n = blobs.size();    //Get number of blobs
        obj.resize( n );        //Resize obj array
        for (int i=0; i<n; i++) {
            obj[i] = blobs[i].centroid;    //Fill obj array
        }
        
        //Additional rendering - updates img if needed
        if ( drawMode == 1 ) {
            generateImg( mask.width, mask.height );
        }
    }
   
}

void ofApp::draw() {
    ofBackground(0);
    ofPushStyle();
    if ( drawMode == 0 ) {
        int w = imageDecimated.width;
        int h = imageDecimated.height;
        ofSetColor( 255, 255, 255 );    //Set color for images drawing
        
        //Thresholded
        mask.draw( 0, ofGetWindowHeight()/2, w, h );
        
        //Draw found contours
        contourFinder.draw( 0, h+10, w, h );
        ofSetLineWidth( 6 );
    }
    else {
        //Additional rendering - draw generated image over original video
        ofEnableAlphaBlending();
        ofSetColor( 255, 255, 255 );
        video.draw( 0, 0 );
        img.draw( 0, 0, 640, 360 );
    }
    
    if ( drawMode == 0 ) {
        ofBackground(0);    //Set the background color
        int w = imageDecimated.width;
        int h = imageDecimated.height;
    
        ofPushStyle();
        ofSetColor(50,0,0);
        colorImg.draw(0, 0, ofGetWindowWidth()/2, ofGetWindowHeight()/2 );
        ofPopStyle();
        
        ofSetColor( 255, 255, 255 );    //Set color for images drawing
        
        //Thresholded
        //mask.draw( 0, 200, w, h );
        
        //Draw found contours
        contourFinder.draw( 0, h+10, w, h );
        ofSetLineWidth( 3 );
    }
    else {
        //Additional rendering - draw generated image over original video
        ofEnableAlphaBlending();
        ofSetColor( 0,122, 255 );
        video.draw( 0, 0 );
        img.draw( 0, 370, 640, 360 );
    }
    ofPopStyle();
    //Draw only if diffFloat image is ready.
    //It happens when the second frame from the video is obtained
    if ( diffFloat.bAllocated ) {
        //Get image dimensions
        int w = grayImage.width;
        int h = grayImage.height;
        
        //Draw images grayImage,  diffFloat, bufferFloat
        ofPushStyle();
        ofSetColor( 0, 0, 255 );
        diffFloat.draw( ofGetWindowWidth()/2, ofGetWindowHeight()/2,ofGetWindowWidth()/2, ofGetWindowHeight()/2);
        ofPopStyle();
        
        ofPushStyle();
        ofSetColor( 0, 122, 255 );
    
        bufferFloat.draw(ofGetWindowWidth()/2, ofGetWindowHeight()/2, ofGetWindowWidth()/2, ofGetWindowHeight()/2 );
         ofPopStyle();
        //Draw the image motion areas
        
        ofPushStyle();
        // draw the incoming video image
        ofSetHexColor(0xffffff);
        ofPushMatrix();
        //        ofTranslate(ofGetWidth()/2 - colorImg.getWidth()/2,ofGetHeight()/2 - colorImg.getHeight()/2);
        
        
        //if we want to draw an outline around our blob path
        ofNoFill();
        ofSetColor(col);
        ofBeginShape();
        //we loop through each of the detected blobs
        //contourFinder.nBlobs gives us the number of detected blobs
        for (int i = 0; i < contourFinder.nBlobs; i++){
            //each of our blobs contains a vector<ofPoints> pts
            for(int j=0; j < contourFinder.blobs[i].pts.size(); j++){
                ofVertex(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y);
            }
        }
        ofEndShape();
        ofPopMatrix();
        ofPopStyle();
//        ofTranslate( ofGetWindowWidth()/2, ofGetWindowHeight()/2);
//        ofScale( 0.5, 0.5 );
        
        //Shift and scale the coordinate system
        //ofPushMatrix();
        ofTranslate(ofGetWidth(),ofGetHeight() );
        ofScale( 0.5, 0.5 );
        
       // Draw bounding rectangle
        ofSetColor(0,0,255);
        ofNoFill();
        ofRect( -1, -1, w+2, h+2 );
       
        
        //Get bufferFloat pixels
        float *pixels = bufferFloat.getPixelsAsFloats();
        //Scan all pixels
        for (int y=0; y<h; y++) {
            for (int x=0; x<w; x++) {
                //Get the pixel value
                float value = pixels[ x + w * y ];
                //If value exceed threshold, then draw pixel
                if ( value >= 0.9 ) {
                    ofRect( x, y, 1, 1 );
                    //Rectangle with size 1x1 means pixel
                    //Note, this is slow function,
                    //we use it here just for simplicity
                }
            }
        }
        ofPopMatrix();
//Restore the coordinate system
        //AZUSA
        ofPushStyle();
        // draw the incoming video image
        ofSetHexColor(0xffffff);
        ofPushMatrix();
        //        ofTranslate(ofGetWidth()/2 - colorImg.getWidth()/2,ofGetHeight()/2 - colorImg.getHeight()/2);
        
        
//        //if we want to draw an outline around our blob path
//        ofNoFill();
//        ofSetColor(ofColor::orange);
//
//        ofBeginShape();
//        //we loop through each of the detected blobs
//        //contourFinder.nBlobs gives us the number of detected blobs
//        for (int i = 0; i < contourFinder.nBlobs; i++){
//            //each of our blobs contains a vector<ofPoints> pts
//            for(int j=0; j < contourFinder.blobs[i].pts.size(); j++){
//                ofVertex(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y);
//            }
//        }
//        ofEndShape();
//        ofPopMatrix();
//        ofPopStyle();
//
//
//        ofTranslate( ofGetWindowWidth()/2, ofGetWindowHeight()/2);
//        ofScale( 0.5, 0.5 );
//
    
 //   ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), -100, +100);
    ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), -200, +300);
    ofEnableDepthTest();
    
    ofPushMatrix();
    ofTranslate( grayImage.width/2+320, 0);
    
    motionAmplifier.draw(video);
    ofDisableDepthTest();
    
    ofPopMatrix();
}
    
    
    
}
    void ofApp::generateImg( int w, int h ){
        if ( !img.bAllocated() ) {
            img.allocate( w, h, OF_IMAGE_COLOR_ALPHA );
        }
        
        ofPixels &pixels = img.getPixelsRef();
        
        //Generate image using object's centers
        for ( int y=0; y<h; y++) {
            for ( int x=0; x<w; x++ ) {
                float step = 15.0; //15;
                float f = 0.;
                for (int i=0; i<obj.size(); i++) {
                    ofPoint p = obj[i];
                    float rad = 60;
                    float val = 0;
                    float dist = ofDist( p.x, p.y, x, y );
                    
                    //Circle function
                    val = max( rad - abs( dist-rad ), 0.0f );         //2-peaks
                    
                    //Uncomment to try other shape:
                    //Square function
                    //val = max(abs( x - p.x ), abs( y - p.y ));
                    
                    f += val;
                }
                
                float val = fmodf( f, step * 2 ) / step;
                val = 1-abs( 1-val );
                val = ofMap( val, 0.9, 1.0, 0, 255, true );
                pixels.setColor( x, y, ofColor( 255, 255, 255, val ) );
            }
        }
        img.update();
    }
// kepress to draw contour lines

void ofApp::keyPressed(int key){
    if ( key == '1' ) {
        drawMode = 0;
    }
    if ( key == '2' ) {
        drawMode = 1;
    }
    if (key == 'p' ){
        col = ofColor(255,182,193);
    }
    if (key == 'y' ){
        col = ofColor(255,215,0);
    }
    if (key == 'b' ){
        col = ofColor(0,255,255);
    }
    if (key == 'w' ){
        col = ofColor(255,255,255);
    }
    if (key == 'r' ){
        col = ofColor(ofRandom( 0, 255 ), ofRandom( 0, 255 ), ofRandom( 128, 255 ));
    }
}
