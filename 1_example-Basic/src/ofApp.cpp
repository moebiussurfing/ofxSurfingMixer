#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);

	mixer.setup();
}

//--------------------------------------------------------------
void ofApp::update() {
	// channel 1
	mixer.begin_PRE_Channel_1();
	{
		scene.drawChannel1();
	}
	mixer.end_PRE_Channel_1();

	// channel 2
	mixer.begin_PRE_Channel_2();
	{
		scene.drawChannel2();
	}
	mixer.end_PRE_Channel_2();

	mixer.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	// mixer
	mixer.drawMixer();

	// gui
	mixer.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// scene
	if (key == 'S') {
		scene.setToggleGuiVisible();
	}

	// mixer
	if (key == 'M') {
		mixer.setToggleGuiVisible();
	}
	if (key == 'A') {
		mixer.setToggleActive();
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	mixer.windowResized(w, h);
}
