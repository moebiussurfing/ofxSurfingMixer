#pragma once
#include "ofMain.h"

//-----------------------------

#define INCLUDE_ofxGui
#ifdef INCLUDE_ofxGui
	#include "ofxGui.h"
#endif

// 1. Blend
#define INCLUDE_BLEND_MODE
#ifdef INCLUDE_BLEND_MODE
	#include "ofxPSBlend.h"
#endif

// 2. Mixer
#include "ofxGpuMixer.h"

// App modes
#define NUM_MODES_APP 3
// we can handle many app modes to change behavior

#include "ofxSurfingHelpers.h"

//-----------------------------

class ofxSurfingMixer : public ofBaseApp {

public:
	ofxSurfingMixer();
	~ofxSurfingMixer();

	void setup();
	void update();
	void updateEngine();
	void exit();
	void windowResized(int w, int h);

private:
	void setupCallbacks();
	void startup();
	void drawPreviews(float x, float y, float _w);

	//--

public:
	// parameters for presets
	ofParameterGroup & getParamsPreset();

	//--

private:
	bool bGuiAdvanced = true;

	//--

	ofParameterGroup params_Preset;
	ofParameterGroup params_Control;

	ofParameterGroup params_UserGui;
	ofParameterGroup params_AppSession;

	//----

private:
	void reallocate(int w, int h);
	void resizeFbos(int w, int h);

public:
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

private:
	ofFbo fbo_Input_1; // channel1: background ?
	ofFbo fbo_Input_2; // channel2: letters (will be the blend mix from channel 1 and channel 2 already ?
	ofFbo fbo_MixOut; // mixed. final mixed of the 2 channels. for drawing or to apply 'global fx'

	//--

	// TODO:
	// workaround to avoid startup crashes by 'recursive callbacks'...?
	bool bBlendRunning = false;

	//-

	// Blender
#ifdef INCLUDE_BLEND_MODE
	ofxPSBlend psBlend;
#endif

	//--

	// Parameters

private:
	// Control
	ofParameter<bool> bEnableFx { "ENABLE FX", true };

	// 1. Blend
	ofParameterGroup params_Blend { "MODE BLEND" };
	ofParameter<bool> bEnableModeBlend { "ENABLE MODE BLEND", false };
	ofParameter<bool> bShowPreview { "SHOW PREVIEW", true };
	ofParameter<bool> bSwapChannels { "SWAP CHANNELS", false };
	ofParameter<bool> bShowBackgrounds { "SHOW BG TINTS", false };
	ofParameter<std::string> infoChanelsSwap { "ROUTE", "" };
	ofParameter<int> blendMode { "BLEND MODE", 0, 0, 24 };
	ofParameter<std::string> blendName { "", "" };

	// 2. Mixer
	ofParameterGroup params_Mixer { "MODE MIXER" };
	ofParameter<bool> bEnableModeMixer { "ENABLE MODE MIXER", false };

	ofParameterGroup params_Backgrounds { "BACKGROUNDS" };
	ofParameter<bool> bEnableChannel1 { "CHANNEL 1", true };
	ofParameter<bool> bEnableChannel2 { "CHANNEL 2", true };
	ofParameter<bool> bEnableBg1 { "BG1", true };
	ofParameter<bool> bEnableBg2 { "BG2", true };
	ofParameter<bool> bEnableBgMix { "BG MIX", true };
	ofParameter<ofColor> colorBg1 { "BG1 COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<ofColor> colorBg2 { "BG2 COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<ofColor> colorBgMix { "BG MIX COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<bool> bResetBackgrounds { "RESET", false };

private:
	// Mixer
	ofxGpuMixer::Mixer mixerGpu;
	ofFbo fbo_Mixer_A;
	ofFbo fbo_Mixer_B;
	ofxGpuMixer::SimpleColorChannel colorChannel;
	ofTexture tex_Mixer_B;
	ofParameter<ofColor> colorBackground;

	//--

private:
	void drawPreviewBorders(float x, float y, float w, float h);
	ofParameter<glm::vec2> positionPreview;

public:
	void setPositionPreview(glm::vec2 pos);

	void setActive(bool b);
	void setToggleActive();
	void setToggleGuiVisible();
	void setGuiVisible(bool b);

	//--

	void setLogLevel(ofLogLevel level);
	void setPathGlobal(std::string s); //must call before setup. disabled by default

	void setAutoSave(bool b);

	//--

private:
	int window_W, window_H;

	// autosave
	ofParameter<bool> bEnableAutosave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 10000; //10 secs

	// updating some params before save will trigs also the group callbacks
	// so we disable this callbacks just in case params updatings are required
	// in this case we will need to update gui position param
	bool bDisableCallbacks = false;

	//-

private:
	ofParameterGroup params_Internal;
	ofParameter<bool> bModeActive;
	ofParameter<bool> bGui;
	ofParameter<glm::vec2> positionGui;
	ofParameter<int> MODE_AppMixer;
	ofParameter<std::string> MODE_AppMixer_Name;

#ifdef INCLUDE_ofxGui
	ofxPanel gui;
#endif

	//-

private:
	ofEventListeners parameterListeners;

private:
	// Keys
	void keyPressed(ofKeyEventArgs & eventArgs);
	void keyReleased(ofKeyEventArgs & eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	// Mouse
	void mouseDragged(ofMouseEventArgs & eventArgs);
	void mousePressed(ofMouseEventArgs & eventArgs);
	void mouseReleased(ofMouseEventArgs & eventArgs);
	void addMouseListeners();
	void removeMouseListeners();

	//-

private:
	// Path folder and filenames
	std::string path_GLOBAL;
	std::string path_Params_AppSession;
	std::string path_Params_Preset;
	std::string path_Params_Mixer;

	void loadParams(ofParameterGroup & g, std::string path);
	void saveParams(ofParameterGroup & g, std::string path);

	void guiRefresh();

	// Labels
	ofTrueTypeFont myFont;
	ofTrueTypeFont myFontHelp;
	ofTrueTypeFont myFontSmall;
	std::string myTTF; // gui font for all gui theme
	int sizeTTF;

	void drawPreviewsCheckerboard(float x, float y, float width, float height, float size);
	//TODO: could improve performance cpu using fbo or loaded image...
};
