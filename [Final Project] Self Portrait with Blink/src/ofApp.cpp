#include "ofApp.h"

//===================================================================
// Used to extract eye rectangles. from ProCamToolkit
GLdouble modelviewMatrix[16], projectionMatrix[16];
GLint viewport[4];
void updateProjectionState() {
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

glm::vec3 ofWorldToScreen(glm::vec3 world) {
    updateProjectionState();
    glm::vec3 screen = glm::project(glm::dvec3(world),
                                    glm::make_mat4(modelviewMatrix),
                                    glm::make_mat4(projectionMatrix),
                                    glm::make_vec4(viewport));
    screen.y = ofGetHeight() - screen.y;
    return screen;
}

ofMesh getProjectedMesh(const ofMesh& mesh) {
    ofMesh projected = mesh;
    for(std::size_t i = 0; i < mesh.getNumVertices(); i++) {
        glm::vec3 cur = ofWorldToScreen(mesh.getVerticesPointer()[i]);
        cur.z = 0;
        projected.setVertex(i, cur);
    }
    return projected;
}

template <class T>
void addTexCoords(ofMesh& to, const std::vector<T>& from) {
    for(std::size_t i = 0; i < from.size(); i++) {
        to.addTexCoord(glm::vec2(from[i].x, from[i].y));
    }
}


//===================================================================
using namespace ofxCv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    cam.initGrabber(640, 480);
    
    // Set up images we'll use for frame-differencing
    camImage.allocate(640, 480, OF_IMAGE_COLOR);
    eyeImageColor.allocate(128, 48);
    eyeImageGray.allocate(128,48);
    eyeImageGrayPrev.allocate(128,48);
    eyeImageGrayDif.allocate(128,48);
    eyeFbo.allocate(128, 48, GL_RGB);
    
    // Initialize our images to black.
    eyeImageColor.set(0);
    eyeImageGray.set(0);
    eyeImageGrayPrev.set(0);
    eyeImageGrayDif.set(0);
    
    // Set up other objects.
    tracker.setup();
    osc.setup("localhost", 8338);
    
    // This GUI slider adjusts the sensitivity of the blink detector.
    gui.setup();
    gui.add(percentileThreshold.setup("Threshold", 0.92, 0.75, 1.0));
    
    //Portrait
    ofSetVerticalSync(true);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    cam.setup(ofGetWidth(), ofGetHeight());
    
    tracker.setup();
    
    // Countour Finder
    contourFinder.setMinAreaRadius(400); // 輪郭検出の最小範囲
    contourFinder.setMaxAreaRadius(800);// 輪郭検出の最大範囲
    // 背景差分
    background.setLearningTime(900); // 差分の学習時間を900msに
    background.setThresholdValue(90); // 背景の閾値を20に
    
    // GUI Settings
    resetBackgroundButton.addListener(this,&ofApp::resetBackgroundPressed);
    gui.setup();
    gui.add(bgThresh.setup("background thresh",50,0,100));
    gui.add(contourThresh.setup("contour finder thresh",127,0,255));
    gui.add(resetBackgroundButton.setup("reset background"));
    
    //Optical Flow
    gui.add(pyrScale.setup("pyrScale", .5, 0, 1));
    gui.add(levels.setup("levels", 4, 1, 8));
    gui.add(winsize.setup("levels", 8, 4, 64));
    gui.add(iterations.setup("iterations", 6, 1, 8));
    gui.add(polyN.setup("polyN", 6, 5, 10));
    gui.add(polySigma.setup("polySigma", 1.9, 1.1, 2));
    gui.add(OPTFLOW_FARNEBACK_GAUSSIAN.setup("OPTFLOW_FARNEBACK_GAUSSIAN", true));
    gui.add(winSize.setup("winSize", 20, 4, 64));
    gui.add(maxLevel.setup("maxLevel", 3, 0, 8));
    gui.add(levels.setup("maxFeatures", 200, 1, 1000));
    gui.add(qualityLevel.setup("levels", 0.01, 0.001, .02));
    gui.add(minDistance.setup("minDistance", 4, 1, 16));
    curFlow = &farneback; // &はポインタという意味（後述）
    
}

void ofApp::update() {
    cam.update();
    if(cam.isFrameNew()) {
        camImage.setFromPixels(cam.getPixels());
        
        tracker.update(toCv(cam));
        position = tracker.getPosition();
        scale = tracker.getScale();
        rotationMatrix = tracker.getRotationMatrix();
        
        if(tracker.getFound()) {
            glm::vec2
            leftOuter = tracker.getImagePoint(36),
            leftInner = tracker.getImagePoint(39),
            rightInner = tracker.getImagePoint(42),
            rightOuter = tracker.getImagePoint(45);
            
            ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
            ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
            
            glm::vec2 leftCenter = leftEye.getBoundingBox().getCenter();
            glm::vec2 rightCenter = rightEye.getBoundingBox().getCenter();
            
            float leftRadius = (glm::distance(leftCenter, leftInner) + glm::distance(leftCenter, leftOuter)) / 2;
            float rightRadius = (glm::distance(rightCenter, rightInner) + glm::distance(rightCenter, rightOuter)) / 2;
            
            glm::vec3
            leftOuterObj = tracker.getObjectPoint(36),
            leftInnerObj = tracker.getObjectPoint(39),
            rightInnerObj = tracker.getObjectPoint(42),
            rightOuterObj = tracker.getObjectPoint(45);
            
            glm::vec3 upperBorder(0, -3.5, 0), lowerBorder(0, 2.5, 0);
            glm::vec3 leftDirection(-1, 0, 0), rightDirection(+1, 0, 0);
            float innerBorder = 1.5, outerBorder = 2.5;
            
            ofMesh leftRect, rightRect;
            leftRect.setMode(OF_PRIMITIVE_LINE_LOOP);
            leftRect.addVertex(leftOuterObj + upperBorder + leftDirection * outerBorder);
            leftRect.addVertex(leftInnerObj + upperBorder + rightDirection * innerBorder);
            leftRect.addVertex(leftInnerObj + lowerBorder + rightDirection * innerBorder);
            leftRect.addVertex(leftOuterObj + lowerBorder + leftDirection * outerBorder);
            rightRect.setMode(OF_PRIMITIVE_LINE_LOOP);
            rightRect.addVertex(rightInnerObj+ upperBorder + leftDirection * innerBorder);
            rightRect.addVertex(rightOuterObj + upperBorder + rightDirection * outerBorder);
            rightRect.addVertex(rightOuterObj + lowerBorder + rightDirection * outerBorder);
            rightRect.addVertex(rightInnerObj + lowerBorder + leftDirection * innerBorder);
            
            ofPushMatrix();
            ofSetupScreenOrtho(640, 480, -1000, 1000);
            ofScale(1, 1, -1);
            ofTranslate(position);
            applyMatrix(rotationMatrix);
            ofScale(scale, scale, scale);
            leftRectImg = getProjectedMesh(leftRect);
            rightRectImg = getProjectedMesh(rightRect);
            ofPopMatrix();
            
            ofMesh normRect, normLeft, normRight;
            normRect.addVertex(glm::vec3(0, 0, 0));
            normRect.addVertex(glm::vec3(64, 0, 0));
            normRect.addVertex(glm::vec3(64, 48, 0));
            normRect.addVertex(glm::vec3(0, 48, 0));
            normLeft.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            normRight.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            normLeft.addVertices(normRect.getVertices());
            normRight.addVertices(normRect.getVertices());
            addTexCoords(normLeft, leftRectImg.getVertices());
            addTexCoords(normRight, rightRectImg.getVertices());
            
            // Copy the extracted quadrilaterals into the eyeFbo
            eyeFbo.begin();
            ofSetColor(255);
            ofFill();
            cam.getTexture().bind();
            normLeft.draw();
            ofTranslate(64, 0);
            normRight.draw();
            cam.getTexture().unbind();
            eyeFbo.end();
            eyeFbo.readToPixels(eyePixels);
            
            // Convert the combined eye-image to grayscale,
            // and put its data into a frame-differencer.
            eyeImageColor.setFromPixels(eyePixels);
            eyeImageGrayPrev.setFromPixels(eyeImageGray.getPixels());
            eyeImageGray.setFromColorImage(eyeImageColor);
            eyeImageGray.contrastStretch();
            eyeImageGrayDif.absDiff(eyeImageGrayPrev, eyeImageGray);
            
            // Compute the average brightness of the difference image.
            unsigned char* difPixels = eyeImageGrayDif.getPixels().getData();
            int nPixels = 128*48;
            float sum = 0;
            for (int i=0; i<nPixels; i++){
                if (difPixels[i] > 4){ // don't consider diffs less than 4 gray levels;
                    sum += difPixels[i]; // reduces noise
                }
            }
            // Store the current average in the row graph
            float avg = sum / (float) nPixels;
            rowGraph.addSample(avg, percentileThreshold);
            
            // Send out an OSC message,
            // With the value 1 if the current average is above threshold
            ofxOscMessage msg;
            msg.setAddress("/gesture/blink");
            int oscMsgInt = (rowGraph.getState() ? 1 : 0);
            msg.addIntArg(oscMsgInt);
            osc.sendMessage(msg);
            
// Print out a message to the console if there was a blink.
            if (oscMsgInt > 0){
                printf("Blink happened at frame #:    %d\n", (int)ofGetFrameNum());
//Change Colors if theres a blink.
                backCol = ofColor(204, ofRandom(50, 180), ofRandom(50, 180));
                color = ofColor(255,255,255);
            }
        }
    }
    //Portrait
    if(cam.isFrameNew()) { // When frame is new
        
        //Facetracker
        tracker.update(toCv(cam));
        position = tracker.getPosition();
        scale = tracker.getScale();
        orientation = tracker.getOrientation();
        rotationMatrix = tracker.getRotationMatrix();
        // Contour（反応する境界を指定）
        float threshold = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
        contourFinder.setThreshold(threshold); // Set Threshold
        contourFinder.findContours(cam); //Get Contour from Camera
        
        //Optical Flow
        if(OPTFLOW_FARNEBACK_GAUSSIAN){
            curFlow = &pyrLk;
            pyrLk.setMaxFeatures(maxFeatures);
            pyrLk.setQualityLevel(qualityLevel);
            pyrLk.setMinDistance(minDistance);
            pyrLk.setWindowSize(winSize);
            pyrLk.setMaxLevel(maxLevel);
        }
        // Calculate Optical Flow
        curFlow->calcOpticalFlow(cam);
    }
}

void ofApp::draw() {
    ofSetColor(255);
    
    camImage.draw(0, 0);
    tracker.draw();
    
    float y = 58;
//  gui.draw();

    rowGraph.draw(10, y, 48);
    ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, ofGetHeight() - 20);
    
    //Portrait
    ofScale(0.8,0.8);
    ofTranslate(100, 100);
    ofSetColor(135, 206, 235);
    cam.draw(0, 0);
    ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
    ofBackground(backCol);
    
    ofPushMatrix();
    ofSetColor(255,255,255);
    thresholded.draw(0,0);
    ofFill();
    
    ofSetLineWidth(2);
    ofSetColor(color);
    contourFinder.draw();
    ofPopMatrix();
    
    if(tracker.getFound()) {
        ofPushMatrix();
        ofSetLineWidth(2);
        ofSetColor(color);
        tracker.draw();
        ofPopMatrix();
    }
    
    ofPushMatrix();
    ofTranslate(70, 0);
    ofSetColor(color);
    ofSetLineWidth(1);
    curFlow->draw(0,0,ofGetWidth(),ofGetHeight());
    ofPopMatrix();
    
}

void ofApp::keyPressed(int key) {
    if(key == 'r') {
        tracker.reset();
    }
    if(key == 'x'){
        img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
        img.save("screenshot"+ofGetTimestampString()+".png");
    }
    if(key == 'b'){
        backCol = ofColor(204, 255, 255);
        color = ofColor(255, 102, 240);
    }
    if(key == 'w'){
        backCol = ofColor(0, 0, 0);
        color = ofColor(255, 255, 255);
    }
}
// resetBackgroundPressedを関数ごと新規作成
void ofApp::resetBackgroundPressed(){
    background.reset();
}
