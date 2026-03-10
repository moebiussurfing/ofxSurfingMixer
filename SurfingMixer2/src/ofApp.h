#pragma once
#include "ofMain.h"

#include "ofAppGuiViews.h"
#include "ofxSurfingImGui.h"
#include "ofxSurfingMixer.h"
#include "ofxWindowApp.h"

#define USE_ofxNDI
#ifdef USE_ofxNDI
	#include "NDI_HelperReceiver.h"
	#include "NDI_HelperSender.h"
#endif

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void drawGui();
	void keyPressed(int key);
	void windowResized(int w, int h);
	void exit();

	ofxSurfingMixer mixer;

	//--

	ofxSurfingGui ui;
	void setupImGui();
	ofParameter<bool> bGui { "ofApp", true };

	//----

	void updateScene();
	void drawScene();

	ofEasyCam cam;

	//----

#ifdef USE_ofxNDI
	NDI_HelperReceiver ndi_CH1;
	NDI_HelperReceiver ndi_CH2;
	NDI_HelperSender ndiOut;
#endif

	ofxWindowApp w;
	ofAppGuiViews guiViews;
};
