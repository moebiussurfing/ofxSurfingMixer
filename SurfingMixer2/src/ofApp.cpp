#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	setupParameters();

	//--

	setupImGui();

#ifdef USE_ofxNDI
	ndi_CH1.setup();
	ndi_CH1.ndiReceiver.SetSenderName("surfingVideoSkip");
	ndi_CH1.ndiReceiver.SetSenderIndex(1);

	ndi_CH2.setup();
	ndi_CH2.ndiReceiver.SetSenderName("surfingTitles");
	ndi_CH2.ndiReceiver.SetSenderIndex(0);
#endif

	mixer.setLogLevel(OF_LOG_SILENT);
	mixer.setup();
	mixer.resizeFbos(1080,1920);
	mixer.reallocate(1080,1920);

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

	//#ifdef USE_ofxNDI
	//	ui.addWindowSpecial(ndi.bGui); // index 0
	//#endif
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
void ofApp::updateScene() {
	// channel 1
	mixer.begin_CH1();
	{
#ifdef USE_ofxNDI
		ndi_CH1.draw(0, 0, 1080, 1920);
#endif
	}
	mixer.end_CH1();

	// channel 2
	mixer.begin_CH2();
	{
#ifdef USE_ofxNDI
		ndi_CH2.draw(0, 0, 1080, 1920);
#endif
	}
	mixer.end_CH2();

	mixer.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	drawScene();

	//----

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	// Mixer OUT
	mixer.draw();

	//auto & r = ui.getRectangleCentralViewport();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	if (!bGui) return;

	// Gui
	mixer.drawGui();

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

		ui.Add(mixer.bGui, OFX_IM_TOGGLE_BIG);

		ui.Add(w.params, SurfingGuiGroupStyle_Collapsed);

		ui.EndWindow();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGuiSpecialWindow0() {

	if (ui.BeginWindowSpecial(0)) {
		ui.AddLabelHuge("Mixer", false);

		ui.AddGroup(mixer.getParamsPreset(), SurfingGuiGroupStyle_Collapsed);

		//--

		ui.EndWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofApp::drawImGuiSpecialWindow1() {

	if (ui.BeginWindowSpecial(1)) {
		ui.AddLabelHuge("Window 1", false);
		//ui.AddGroup(mixer.getParamsPreset());

		//--

		ui.EndWindowSpecial();
	}
}

//----

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	//mixer.windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//ofLogNotice(__FUNCTION__) << " " << char(key);

	if (key == 'g') bGui = !bGui;

	if (key == OF_KEY_SPACE) {
		ndi_CH1.ndiReceiver.SetSenderName("surfingVideoSkip");
		ndi_CH2.ndiReceiver.SetSenderName("surfingTitles");
	}

#ifdef USE_ofxNDI
	ndi_CH1.keyPressed(key);
	//ndi_CH2.keyPressed(key);
#endif
}

//--------------------------------------------------------------
void ofApp::exit() {
#ifdef USE_ofxNDI
	ndi_CH1.exit();
	ndi_CH2.exit();
#endif
}

//----
