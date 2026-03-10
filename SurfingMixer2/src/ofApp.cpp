#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	setupParameters();
	guiViews.setApp(this);

	//--

	setupImGui();

	//mixer.setLogLevel(OF_LOG_SILENT);
	mixer.setup();
	//mixer.resizeFbos(1080,1920);
	ofEnableArbTex();
	mixer.reallocate(1080, 1920);

	w.setup(&w);
	w.setFrameRate(60);

#ifdef USE_ofxNDI
	ndi_CH1.setup();
	ndi_CH2.setup();
#endif
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
	//ui.addWindowSpecial(mixer.bGui); // index 1

	//// Extra windows to be auto included in windows menu
	//ui.addWindowExtra(bGui);

	//--

	ui.startup();
}

//--------------------------------------------------------------
void ofApp::update() {
	static bool bDone = false;
	if (!bDone && ofGetElapsedTimef()>2) {
		bDone = true;
		ndi_CH1.ndiReceiver.SetSenderName("W11-AERO (surfingTitles)");
		ndi_CH2.ndiReceiver.SetSenderName("W11-AERO (surfingVideoSkip)");
	}

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

	guiViews.drawImGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//ofLogNotice(__FUNCTION__) << " " << char(key);

	if (key == 'g') bGui = !bGui;

	if (key == OF_KEY_SPACE) {
		////ndi_CH1.ndiReceiver.SetSenderName("surfingVideoSkip");
		////ndi_CH2.ndiReceiver.SetSenderName("surfingTitles");
		//ndi_CH1.ndiReceiver.SetSenderName("Test Pattern 2");
		//ndi_CH2.ndiReceiver.SetSenderName("Test Pattern");
	}

#ifdef USE_ofxNDI
	//ndi_CH1.keyPressed(key);
	////ndi_CH2.keyPressed(key);
#endif
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	mixer.windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::exit() {
	mixer.exit();

#ifdef USE_ofxNDI
	ndi_CH1.exit();
	ndi_CH2.exit();
#endif
}

//----
