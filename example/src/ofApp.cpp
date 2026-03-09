#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetWindowPosition(-1920, 30);
	ofSetWindowTitle("ofxSurfingMixer");

	mixer.setLogLevel(OF_LOG_VERBOSE);
	mixer.setup();
}

//--------------------------------------------------------------
void ofApp::update() {
	// channel 1
	mixer.begin_CH1();
	{
		scene.drawChannel1();
	}
	mixer.end_CH1();

	// channel 2
	mixer.begin_CH2();
	{
		scene.drawChannel2();
	}
	mixer.end_CH2();

	mixer.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Mixer OUT
	mixer.draw();

	// Gui
	mixer.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// Scene
	if (key == 'S') {
		scene.setToggleGuiVisible();
	}

	// Mixer
	if (key == 'G') {
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
