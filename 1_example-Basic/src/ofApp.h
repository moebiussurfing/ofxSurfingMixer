#pragma once
#include "ofMain.h"

//-----------------------------
//							
//	OPTIONAL DEFINES					
//		
//							
//-----------------------------

//	NOTE 
//	ABOUT THIS EXAMPLE:
//	1_example-Basic
//
//	To run out-of-the-box,
//	At ofxSurfingMixer.h,
//	This example requires to comment (disable) the lines:
//
//	#define INCLUDE_FX_HELPER
//	#define INCLUDE_ofxPresetsManager

///----


#include "ofxSurfingMixer.h"

#include "ofxSceneTEST.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void windowResized(int w, int h);

	//scene
	ofxSceneTEST scene;
    
	//mixer
	ofxSurfingMixer mixer;
};
