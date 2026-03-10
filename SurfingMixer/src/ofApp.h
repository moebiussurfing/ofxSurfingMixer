#pragma once
#include "ofMain.h"


#include "ofxSurfingImGui.h"
#include "ofxSurfingMixer.h"
#include "ofxWindowApp.h"

#define USE_ofxNDI
#ifdef USE_ofxNDI
	#include "SurfingNDIManager.h"
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

	// Docking Helpers Stuff

	// Mode/workflow 
	
#if 0
	// Can be configured:
	bool bModeDockingResetAtStartup = true;
	// false: the layout will be persistent and auto saved on exit and loaded on setup.
	// true: the layout will be reseted by code on each startup.
#endif

	ofParameter<bool> bGui_DockingHelp{ "DOCKING HELP", true };


	// Flags
	bool bFlagDockingReset = false;
	bool bFlagDockingRandom = false;

	//----

	// Manager to Save/Load Layout manually

	string path = "myLayout.ini";
	
	//--

	// Scene functions

	void updateScene(); // Generates random messages to test the Log window.
	void drawScene();// Draws a Central Rectangle getting from the docking layout.

	ofEasyCam cam;

	//----

#ifdef USE_ofxNDI
	// NDI
	SurfingNDIManager ndi;
#endif

	ofxWindowApp w;
};
