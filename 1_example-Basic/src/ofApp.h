#pragma once
#include "ofMain.h"

//----

#include "ofxSurfingMixer.h"

#include "ofxSceneTEST.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void windowResized(int w, int h);

	// scene
	ofxSceneTEST scene;

	// mixer
	ofxSurfingMixer mixer;
};
