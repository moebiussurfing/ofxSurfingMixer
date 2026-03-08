#pragma once
#include "ofMain.h"

//-----------------------------

//	TODO:
//	+	allow one channel enable/preview without broke mixing

#define INCLUDE_ofxGui
#ifdef INCLUDE_ofxGui
	#include "ofxGui.h"
#endif

// 1. blend
#define INCLUDE_BLEND_MODE
#ifdef INCLUDE_BLEND_MODE
	#include "ofxPSBlend.h"
#endif
//#define BLEND_TOGGLES // too much toggles (24)... out of screen

// 2. mixer
#define INCLUDE_MIXER_MODE
#ifdef INCLUDE_MIXER_MODE
	#include "ofxGpuMixer.h"
#endif

// 3. mask
#define INCLUDE_MASK_MODE
#ifdef INCLUDE_MASK_MODE
	#include "ofxAlphaMask.h"
#endif

//app modes
#define NUM_MODES_APP 3
//we can handle many app modes to change behaviour

#include "ofxSurfingHelpers.h"

//-

class ofxSurfingMixer : public ofBaseApp {

public:
	ofxSurfingMixer();
	~ofxSurfingMixer();

	void setup();
	void update();
	void updateEngine();
	//void draw();
	void exit();
	void windowResized(int w, int h);

private:
	void startup();
	void drawPreviews(float x, float y, float _w);

	//--

	// parameters for presets

public:
	//--------------------------------------------------------------
	ofParameterGroup & getParams_Mixer() {
		return params_Preset;
	}

	//--

private:
	bool bGuiAdv = false;

	//--

	ofParameterGroup params_Control;
	ofParameterGroup params_Basic;

	ofParameterGroup params_UserGui;
	ofParameterGroup params_AppSession;

	ofParameterGroup params_Preset;

	//----

	// API

public:
	void setToggleFX(bool b);

	//-

private:
	void reallocate(int w, int h);
	void resizeFbos(int w, int h);

public:
	// TODO:
	// to route to the fx's?
	void begin_PRE_Channel_1();
	void end_PRE_Channel_1();
	void begin_PRE_Channel_2();
	void end_PRE_Channel_2();

	void begin_Channel_1();
	void end_Channel_1();
	void begin_Channel_2();
	void end_Channel_2();

	void begin_Mix();
	void end_Mix();

	//--

private:
	void setupParamsMixer();
	void setupParamsInternal();

	void guiSetup();
	void guiSetupStyles();
	void guiCustomize();

public:
	void updateMixer();
	void drawMixer();
	void drawGui();

	//--

	// TODO: customize channel names
	//std::string nameChannel1, nameChannel2;
	//void setNameChannel1(std::string s)
	//{
	//	nameChannel1 = s;
	//}
	//void setNameChannel2(std::string s)
	//{
	//	nameChannel2 = s;
	//}

	//--

private:
	ofFbo fbo_Input_1; // channel1: background ?
	ofFbo fbo_Input_2; // channel2: letters (will be the blend mix from channel 1 and channel 2 already ?
	ofFbo fbo_MixOut; // mixed. final mixed of the 2 channels. for drawing or to apply 'global fx'

	//--

	// 3. MASK
#ifdef INCLUDE_MASK_MODE
	ofxAlphaMask alphaMask;
#endif

	// TODO:
	// workaround to avoid startup crashes by 'recursive callbacks'...?
	bool bMaskRunning = false;
	bool bBlendRunning = false;

	//-

	// blender
#ifdef INCLUDE_BLEND_MODE
	ofxPSBlend psBlend;
	#ifdef BLEND_TOGGLES
	vector<ofParameter<bool>> ENABLE_Blends;
	#endif
#endif

	//--

	// params

private:
	// control
	ofParameter<bool> ENABLE_FboFxHelper { "ENABLE FX", true };
	ofParameter<bool> MODE_SHOW_FboFxHelper { "SHOW FX", false };
	ofParameter<bool> MODE_PRESET_MIXER { "MIXER PRESETS", false };

	// 1. blend
	ofParameterGroup params_Blend { "MODE BLEND" };
	ofParameter<bool> ENABLE_BLEND { "ENABLE MODE BLEND", false };
	ofParameter<bool> SHOW_Preview { "SHOW PREVIEW", true };
	ofParameter<bool> swapChannels { "SWAP CHANNELS", false };
	ofParameter<bool> SHOW_Backgrounds { "SHOW BG TINTS", false };
	ofParameter<std::string> swapInfo { "", "" };
	ofParameter<int> blendMode { "BLEND MODE", 0, 0, 24 };
	ofParameter<std::string> blendName { "", "" };

	// 3. mask
	ofParameterGroup params_Mask { "MODE MASK" };
	ofParameter<bool> ENABLE_MASK { "ENABLE MODE MASK", false };

	//2. mixer
	ofParameterGroup params_Mixer { "MODE MIXER" };
	ofParameter<bool> ENABLE_MIXER { "ENABLE MODE MIXER", false };

	ofParameterGroup params_Backgrounds { "BACKGROUNDS" };
	ofParameter<bool> ENABLE_Channel1 { "CHANNEL 1", true };
	ofParameter<bool> ENABLE_Channel2 { "CHANNEL 2", true };
	ofParameter<bool> ENABLE_Bg1 { "BG1", true };
	ofParameter<bool> ENABLE_Bg2 { "BG2", true };
	ofParameter<bool> ENABLE_BgMix { "BG MIX", true };
	ofParameter<ofColor> colorBg1 { "BG1 COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<ofColor> colorBg2 { "BG2 COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<ofColor> colorBgMix { "BG MIX COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<bool> RESET_Backgrounds { "RESET", false };

#ifdef INCLUDE_MIXER_MODE
private:
	ofxGpuMixer::Mixer mixerGpu;
	ofFbo fbo_Mixer_A;
	ofFbo fbo_Mixer_B;
	ofxGpuMixer::SimpleColorChannel colorChannel;
	ofTexture tex_Mixer_B;
	ofParameter<ofColor> colorBackground;

	#ifdef INCLUDE_ofxGui
	ofxPanel gui_Mixer;
	#endif
#endif

	//--

private:
	//--------------------------------------------------------------
	void drawPreviewBorders(float x, float y, float w, float h) {
		ofPushStyle();
		ofNoFill();
		float pad = 1.0f;
		ofSetLineWidth(3.0f * pad);
		//ofSetColor(32, 255);//dark
		ofSetColor(255, 64); //white
		//ofDrawRectangle(x - pad, y - pad, w + 2 * pad, h + 2 * pad);
		ofDrawRectRounded(x - pad, y - pad, w + 2 * pad, h + 2 * pad, 5.0f);
		ofPopStyle();
	}

	//-

	ofParameter<glm::vec2> preview_Position;

public:
	//--------------------------------------------------------------
	void setPreview_Position(glm::vec2 pos) {
		preview_Position = pos;
	}

	//------------------------------------------------------------------------------

	void setActive(bool b);
	//--------------------------------------------------------------
	void setToggleActive() {
		setActive(!MODE_Active.get());
	}
	//--------------------------------------------------------------
	void setToggleGuiVisible() {
		setGuiVisible(!SHOW_GuiAll.get());
	}
	void setGuiVisible(bool b);

	//TODO:
	//--------------------------------------------------------------
	void setUserVisible(bool b) {
	}

	//--

	//presets loaders
	//--------------------------------------------------------------
	void loadPreset_blend(int p) {
	}

	void setLogLevel(ofLogLevel level);
	void setKey_MODE_App(int k);
	void setPathGlobal(std::string s); //must call before setup. disabled by default

	//--------------------------------------------------------------
	void setAutoSave(bool b) {
		ENABLE_AutoSave = b;
	}

	//--

private:
	int key_MODE_AppMixer = OF_KEY_TAB; //default key to switch MODE_AppMixer
	int window_W, window_H;

	//autosave
	ofParameter<bool> ENABLE_AutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 10000; //10 secs

	//updating some params before save will trigs also the group callbacks
	//so we disable this callbacks just in case params updatings are required
	//in this case we will need to update gui position param
	bool DISABLE_Callbacks = false;

	//-

private:
	ofParameterGroup params_Internal;
	ofParameter<bool> MODE_Active;
	ofParameter<bool> ENABLE_keys_AllMixer;
	//ofParameter<bool> ENABLE_Debug;
	ofParameter<bool> SHOW_GuiAll;
	ofParameter<bool> SHOW_MIXER;
	ofParameter<glm::vec2> Gui_Position;
	ofParameter<bool> SHOW_Help;
	ofParameter<int> MODE_AppMixer;
	ofParameter<std::string> MODE_AppMixer_Name;

#ifdef INCLUDE_ofxGui
	ofxPanel gui_Control;
#endif

	//-

private:
	void Changed_params_Settings(ofAbstractParameter & e);
	void Changed_params_AppSession(ofAbstractParameter & e);

private:
	//keys
	void keyPressed(ofKeyEventArgs & eventArgs);
	void keyReleased(ofKeyEventArgs & eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	//mouse
	void mouseDragged(ofMouseEventArgs & eventArgs);
	void mousePressed(ofMouseEventArgs & eventArgs);
	void mouseReleased(ofMouseEventArgs & eventArgs);
	void addMouseListeners();
	void removeMouseListeners();

	//-

private:
	//path folder and filenames
	std::string path_GLOBAL;
	std::string path_Params_AppSession;
	std::string path_Params_Preset;
	std::string path_Params_Mixer;
	std::string path_Params_Mask;

	void loadParams(ofParameterGroup & g, std::string path);
	void saveParams(ofParameterGroup & g, std::string path);

	void guiRefresh();

	//labels
	ofTrueTypeFont myFont;
	ofTrueTypeFont myFontHelp;
	ofTrueTypeFont myFontSmall;
	std::string myTTF; // gui font for all gui theme
	int sizeTTF;

	void drawPreviewsCheckerboard(float x, float y, float width, float height, float size);
	//TODO:could improve preformance cpu using fbo or loaded image..

	std::string helpInfo;
};
