#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	setupParameters();

	//--

	setupImGui();

#ifdef USE_ofxNDI
	// NDI
	ndi.setup();
	ndi.setMode_ImGui();

	//ndi.setGuiInternalVisible(true);//debug
#endif

	mixer.setLogLevel(OF_LOG_VERBOSE);
	mixer.setup();

	w.setup(&w);
	w.setFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::setupParameters() {
	
}

//--------------------------------------------------------------
void ofApp::setupImGui() {
	ui.setImGuiViewPort(true);
	//ui.setup(IM_GUI_MODE_INSTANTIATED_DOCKING_RAW);
	ui.setup();


	ui.addWindowSpecial(ndi.bGui); // index 0
	ui.addWindowSpecial(mixer.bGui); // index 1


	//// Extra windows to be auto included in windows menu
	//ui.addWindowExtra(bGui);

	//--

	ui.startup();
}

//--------------------------------------------------------------
void ofApp::update() {
	updateScene();
}

//--------------------------------------------------------------
void ofApp::draw() {
	drawScene();

#ifdef USE_ofxNDI
	// NDI
	ndi.begin_NDI_OUT();
	{
		//ndi.drawSignals();
		// Mixer OUT
		mixer.draw();
	}
	ndi.end_NDI_OUT();
#endif	

	//----

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
#ifdef USE_ofxNDI
	// NDI Preview
	ndi.draw();
	// Gui
	ndi.drawGui();
#endif

	// Gui
	//mixer.drawGui();

	drawImGui();
}

//--------------------------------------------------------------
void ofApp::drawImGui() {
	// Gui Manager with Docking features:
	// In between here (Begin/End)
	// we can render ImGui windows and widgets,
	// and all the docking magic.

	//--

	ui.Begin();
	{
		//--

		// 1. Docking magic
		#if 0
		ui.BeginDocking();
		{
		}
		ui.EndDocking();
		#endif


		//--

		// 2.2 Populate the visible toggles
		drawImGuiApp();

		//--

		// 3. The Special Windows

		// The windows previously queued to the manager on setup(),
		// that are controlled by the Layout Presets Engine.
		// Render ImGui Windows and Widgets now!
		{
			drawImGuiSpecialWindows();
		}

		//--
	}
	ui.End();
}

//----

//--------------------------------------------------------------
void ofApp::drawImGuiSpecialWindows() {

	drawImGuiSpecialWindow0();

	//--

	drawImGuiSpecialWindow1();

}

//--------------------------------------------------------------
void ofApp::drawImGuiApp() {
	// for all the queued especial windows in setup()!
	if (ui.BeginWindow(bGui)) {
		ui.drawWidgetsSpecialWindowsManager();
		ui.AddSpacingSeparated();

		ui.Add(ndi.bGui, OFX_IM_TOGGLE_BIG);
		ui.Add(mixer.bGui, OFX_IM_TOGGLE_BIG);
		ui.EndWindow();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGuiSpecialWindow0() {

	if (ui.BeginWindowSpecial(0)) {
		//ui.AddLabelHuge("Window 0", false);

		//ui.AddGroup(mixer.getParamsPreset());

#ifdef USE_ofxNDI
		ui.Add(ndi.bGui, OFX_IM_TOGGLE_BIG);
#endif

		//--

		ui.EndWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGuiSpecialWindow1() {

	if (ui.BeginWindowSpecial(1)) {
		//ui.AddLabelHuge("Window 1", false);
		ui.AddGroup(mixer.getParamsPreset());

		//--

		ui.EndWindowSpecial();
	}
}

//----

// Scene

//--------------------------------------------------------------
void ofApp::updateScene() {
	// channel 1
	mixer.begin_CH1();
	{
#ifdef USE_ofxNDI
		ndi.draw_NDI_IN_1();
#endif
	}
	mixer.end_CH1();

	// channel 2
	mixer.begin_CH2();
	{
#ifdef USE_ofxNDI
		ndi.draw_NDI_IN_2();
#endif
	}
	mixer.end_CH2();

	mixer.update();
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	//// Mixer OUT
	//mixer.draw();


	//auto & r = ui.getRectangleCentralViewport();

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	mixer.windowResized(w, h);

#ifdef USE_ofxNDI
	ndi.windowResized(w, h);
#endif
}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//ofLogNotice(__FUNCTION__) << " " << char(key);

	if (key == 'g') bGui = !bGui;
	if (key == ' ') bFlagDockingRandom = true;
	if (key == OF_KEY_BACKSPACE) bFlagDockingReset = true;
}

//----




