#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    ofSetFullscreen(true);
    ofSetVerticalSync(true);
    
    //Load Video test
    armsVid.setPixelFormat(OF_PIXELS_RGBA);
    armsVid.loadMovie("videos/arms_final.mov");
    armsVid.setLoopState(OF_LOOP_PALINDROME);
    armsVid.play();
    
    
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.addUserGenerator();
    openNIDevice.setMaxNumUsers(1);
    openNIDevice.start();
    
    // set properties for all user masks and point clouds
    openNIDevice.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    
    dispMask.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
    justUser.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
    bg.allocate(900, 900, OF_IMAGE_COLOR);
    bg.loadImage("pattern.png");
    
    wasTracked = false;
}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();
    armsVid.update();
    
    
    // get number of current users
    int numUsers = openNIDevice.getNumTrackedUsers();
    
    // iterate through users
    if (numUsers > 0) {
        for (int i = 0; i < numUsers; i++){
            
            // USER REFERENCE
            ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
            
            //get neck joint and save to ofPoint
            neckPoint.set(user.getJoint(JOINT_NECK).getProjectivePosition());
            
            // PIXEL REFERENCE for mask
            pix = user.getMaskPixels();
            
            // Make sure the pixel reference is not empty
            if (pix.getNumChannels() > 0){
                dispMask.setFromPixels(pix);
                dispMask.update();
                
                // get the pixels from the RGB camera, store to later copy into the justUser image
                ofPixels inPix;
                inPix.allocate(640, 480, 3);
                inPix = openNIDevice.getImagePixels();
                inPix.setNumChannels(4);
            
                // Loop through the mask-image pixels (pix) and get the color of each one
                // If the alpha is 0, the user is there. Copy those pixels from the RGB array
                // to the temporary pixel array (tempPix)
                // Then set the justUser image's pixels to the ones from tempPix
                for (int x = 0; x < 640; x++){
                    for (int y = 0; y < 480; y++) {
        
                        // get the color (and transparency) or the given pixel from the user mask
                        ofColor color = pix.getColor(x,y);
                        ofPixels & tempUserPix = justUser.getPixelsRef();
        
                        // if the userMask pixel is clear (user is there), grab the pixel's color
                        if ((int) color.a == 0) {
                            
                            tempUserPix.setColor(x, y, inPix.getColor(x, y));
                            
                        }
                        else {
                            tempUserPix.setColor(x,y, ofColor(0,0,0,0));
                        }
                    }
                }
                justUser.update();
                wasTracked = true;
                
            }
        }

    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255, 255, 255);
    

    // draw debug (ie., image, depth, skeleton)
    openNIDevice.drawDebug();
    
    // use a blend mode so we can see 'through' the mask(s)
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    // get number of current users
    int numUsers = openNIDevice.getNumTrackedUsers();
        
    if (wasTracked) {
        // draw the mask texture for this user
        //user.drawMask();
        //dispMask.draw(640,0);
        
        //set arms to be pinned at the torso and play video when tracked
        if (neckPoint.x != 0){
            ofSetRectMode(OF_RECTMODE_CENTER);
            bg.draw(640/2, 480/2);
            armsVid.draw(neckPoint.x,neckPoint.y+100,armsVid.getWidth()*.5,armsVid.getHeight()*.5);
            ofSetRectMode(OF_RECTMODE_CORNER);
        }
        
        // Draw the user mask and just the user separately
        justUser.draw(0,0);
    }

    
    ofDisableBlendMode();
}

//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    // show user event messages in the console
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
      
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}
