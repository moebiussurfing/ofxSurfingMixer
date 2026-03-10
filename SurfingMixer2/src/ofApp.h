#pragma once
#include "ofMain.h"


#include "ofxSurfingImGui.h"
#include "ofxSurfingMixer.h"
#include "ofxWindowApp.h"

#define USE_ofxNDI
#ifdef USE_ofxNDI
	#include "NDI_HelperReceiver.h"
#endif

class ofApp : public ofBaseApp
{
public:
	void setup();
	void setupParameters();
	void update();
	void draw();
	void drawGui();
	void keyPressed(int key);
	void windowResized(int w, int h);
	void exit();

	ofxSurfingMixer mixer;

	//--

	// Gui manager
	ofxSurfingGui ui;

	void setupImGui();
	void drawImGui();
	void drawImGuiApp();
	void drawImGuiSpecialWindows();
	void drawImGuiSpecialWindow0();
	void drawImGuiSpecialWindow1();

	ofParameter<bool> bGui{ "ofApp", true };
	
	//----

	// Manager to Save/Load Layout manually

	//--

	// Scene functions

	void updateScene(); // Generates random messages to test the Log window.
	void drawScene();// Draws a Central Rectangle getting from the docking layout.

	ofEasyCam cam;

	//----

#ifdef USE_ofxNDI
	NDI_HelperReceiver ndi_CH1;
	NDI_HelperReceiver ndi_CH2;
#endif

	ofxWindowApp w;
};
