#pragma once
#include "ofMain.h"


//-----------------------------
//							
//	OPTIONAL DEFINES					
//		
#define USE_OFX_SURFING_FX // un-comment to include fx processor (ofxSurfingFX)
//		
//#define INCLUDE_ofxPresetsManager // un-comment to include the presets manager (ofxPresetsManager)
//							
#define INCLUDE_FX_MASK // un-comment to include the mask fx (ofxDotFrag)
//		
//-----------------------------


//	TODO:
//	+	allow one channel enable/preview without broke mixing


#define INCLUDE_GUI_EXTENDED
#ifdef INCLUDE_GUI_EXTENDED
#include "ofxGuiExtended2.h"
#endif

//#define INCLUDE_ofxGui //TODO: this is for a lite version usign the tiny ofxGui...
#ifdef INCLUDE_ofxGui
#include "ofxGui.h"
#endif

//fx helper
#ifdef USE_OFX_SURFING_FX
#include "ofxSurfingFX.h"
#endif

//1. blend
#define INCLUDE_BLEND_MODE
#ifdef INCLUDE_BLEND_MODE
#include "ofxPSBlend.h"
#endif
//#define BLEND_TOGGLES // too much toggles (24)... out of screen

//2. mixer
#define INCLUDE_MIXER_MODE
#ifdef INCLUDE_MIXER_MODE
#include "ofxGpuMixer.h"
#endif

//3. mask
#define INCLUDE_MASK_MODE
#ifdef INCLUDE_MASK_MODE
#include "ofxAlphaMask.h"
#endif

//fx
#ifdef INCLUDE_FX_MASK
#include "ofxDotFrag.h"
#endif

//presets
#ifdef INCLUDE_ofxPresetsManager
#include "ofxPresetsManager.h"
#endif

//app modes
#define NUM_MODES_APP 3
//we can handle many app modes to change behaviour

#include "ofxSurfingHelpers.h"

//-

class ofxSurfingMixer : public ofBaseApp
{

public:
#pragma mark - OF

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

	//parameters for presets

public:
	//--------------------------------------------------------------
	ofParameterGroup& getParams_Mixer() {
		return params_Preset;
	}

public:
#ifdef USE_OFX_SURFING_FX
	//--------------------------------------------------------------
	ofParameterGroup& getParams_Fx() {
		return FX_Processor.getParametersPreset();
	}
#endif

	//--

private:
#ifdef INCLUDE_GUI_EXTENDED
	ofxGui gui;
	ofxGuiGroup2 *panel_MIXER;
	ofxGuiGroup2* gUser;

	ofJson jButton_BIG;
	ofJson jButton_BIG_L;
	ofJson jSlider_BIG;
	ofJson jNoHead;

	//-

private:
	std::string path_Theme;

public:
	//--------------------------------------------------------------
	void loadTheme(std::string _path) {
		path_Theme = _path;
		ofLogNotice(__FUNCTION__) << "loadTheme: " << path_Theme;
		panel_MIXER->loadTheme(path_Theme);
#ifdef USE_OFX_SURFING_FX
		FX_Processor.loadTheme(path_Theme);
#endif
	}

	//-

	ofxGuiContainer* sliders;
	ofParameter <float> slider1Val, slider2Val, slider3Val, slider4Val;

	ofxGuiTabs* gTabs;
	ofxGuiGroup2* gTab1;
	ofxGuiGroup2* gTab2;
	ofxGuiGroup2* gTab3;

	const int NUM_TABS = 3;
	void Changed_tabGui(int & p);
	ofParameter<int> selectedTab;// { "TAB", 0, 0, NUM_TABS - 1 };
	int selectedTab_PRE = -1;

	//TODO:
	//toggle selector..
	ofParameter<bool> bCh0{ "CH0", false };
	ofParameter<bool> bCh1{ "CH1", false };
	ofParameter<bool> bCh2{ "CH2", false };

	//TODO:
	ofParameterGroup gCh1Mix{ "CHANNEL 1" };
	ofParameterGroup gCh2Mix{ "CHANNEL 2" };

	ofxGuiGroup2* g10;//ch1
	ofxGuiGroup2* g11;//blend
	ofxGuiGroup2* g12;//tint
	ofxGuiGroup2* g20;//ch2
	ofxGuiGroup2* g21;//blend
	ofxGuiGroup2* g22;//tint

#endif

	//-

	bool bGuiAdv = false;

#ifdef INCLUDE_ofxPresetsManager
	ofxPresetsManager presetsManager;
#endif

	//--

	ofParameterGroup params_Control;
	ofParameterGroup params_Basic;

	ofParameterGroup params_UserGui;
	ofParameterGroup params_AppSession;

	ofParameterGroup params_Preset;

	//-

	//fx helper
#ifdef USE_OFX_SURFING_FX
	public:
	ofxSurfingFX FX_Processor;
#endif

	//----

	//API

public:
	void setToggleFX(bool b);

	//-

private:
	void reallocate(int w, int h);
	void resizeFbos(int w, int h);

public:
	//TODO:
	//to route to the fx's?
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

	//TODO: customize channel names
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
	ofFbo fbo_Input_1;//channel1: background ?
	ofFbo fbo_Input_2;//channel2: letters (will be the blend mix from channel 1 and channel 2 already ?
	ofFbo fbo_MixOut;//mixed. final mixed of the 2 channels. for drawing or to apply 'global fx' 

	//--

	//3. MASK
#ifdef INCLUDE_MASK_MODE
	ofxAlphaMask alphaMask;
#endif

	//black and white
#ifdef INCLUDE_FX_MASK
	ofx::dotfrag::HSB frag2;
	ofx::dotfrag::Monochrome frag1;
	ofx::dotfrag::InvertStrobe frag3;
	ofParameter<bool> bReset{ "RESET", false };
	void Changed_bReset();
	ofEventListener listener_bReset;
	ofFbo fbo_VideoFx;
	ofParameter<bool> ENABLE_FX_MASK{ "ENABLE FX MASK", true };
	void update_FxMask();
#endif

	//TODO:
	//workaround to avoid startup crashes by 'recursive callbacks'...?
	bool bMaskRunning = false;
	bool bBlendRunning = false;

	//-

	//blender
#ifdef INCLUDE_BLEND_MODE
	ofxPSBlend psBlend;
#ifdef BLEND_TOGGLES
	vector <ofParameter<bool>> ENABLE_Blends;
#endif
#endif

	//-

	//params

private:

	//control
	ofParameter<bool> ENABLE_FboFxHelper{ "ENABLE FX", true };
	ofParameter<bool> MODE_SHOW_FboFxHelper{ "SHOW FX", false };
	ofParameter<bool> MODE_PRESET_MIXER{ "MIXER PRESETS", false };

	//1. blend
	ofParameterGroup params_Blend{ "MODE BLEND" };
	ofParameter<bool> ENABLE_BLEND{ "ENABLE MODE BLEND", false };
	ofParameter<bool> SHOW_Preview{ "SHOW PREVIEW", true };
	ofParameter<bool> swapChannels{ "SWAP CHANNELS", false };
	ofParameter<bool> SHOW_Backgrounds{ "SHOW BG TINTS", false };
	ofParameter<std::string> swapInfo{ "", "" };
	ofParameter<int> blendMode{ "BLEND MODE", 0, 0, 24 };
	ofParameter<std::string> blendName{ "", "" };

	//3. mask
	ofParameterGroup params_Mask{ "MODE MASK" };
	ofParameter<bool> ENABLE_MASK{ "ENABLE MODE MASK", false };

	//2. mixer
	ofParameterGroup params_Mixer{ "MODE MIXER" };
	ofParameter<bool> ENABLE_MIXER{ "ENABLE MODE MIXER", false };

	ofParameterGroup params_Backgrounds{ "BACKGROUNDS" };
	ofParameter<bool> ENABLE_Channel1{ "CHANNEL 1", true };
	ofParameter<bool> ENABLE_Channel2{ "CHANNEL 2", true };
	ofParameter<bool> ENABLE_Bg1{ "BG1", true };
	ofParameter<bool> ENABLE_Bg2{ "BG2", true };
	ofParameter<bool> ENABLE_BgMix{ "BG MIX", true };
	ofParameter<ofColor> colorBg1{ "BG1 COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<ofColor> colorBg2{ "BG2 COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<ofColor> colorBgMix{ "BG MIX COLOR", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255) };
	ofParameter<bool> RESET_Backgrounds{ "RESET", false };

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
	void drawPreviewBorders(float x, float y, float w, float h)
	{
		ofPushStyle();
		ofNoFill();
		float pad = 1.0f;
		ofSetLineWidth(3.0f*pad);
		//ofSetColor(32, 255);//dark
		ofSetColor(255, 64);//white
		//ofDrawRectangle(x - pad, y - pad, w + 2 * pad, h + 2 * pad);
		ofDrawRectRounded(x - pad, y - pad, w + 2 * pad, h + 2 * pad, 5.0f);
		ofPopStyle();
	}

	//-

	ofParameter<glm::vec2> preview_Position;

public:
	//--------------------------------------------------------------
	void setPreview_Position(glm::vec2 pos)
	{
		preview_Position = pos;
	}

	//------------------------------------------------------------------------------

#pragma mark - API

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
	void setUserVisible(bool b)
	{
		//presets
#ifdef INCLUDE_ofxPresetsManager
		//mixer
		presetsManager.setVisible_PresetClicker(b);
		presetsManager.setEnableKeys(b);
#endif

		//fx
#ifdef USE_OFX_SURFING_FX
		FX_Processor.setUserVisible(b);
#endif
	}

	//--

	//presets loaders
	//--------------------------------------------------------------
	void loadPreset_blend(int p)
	{
#ifdef INCLUDE_ofxPresetsManager
		presetsManager.loadPreset(p);
#endif
	}

	void setLogLevel(ofLogLevel level);
	void setKey_MODE_App(int k);
	void setPathGlobal(std::string s);//must call before setup. disabled by default

	//--------------------------------------------------------------
	void setAutoSave(bool b)
	{
		ENABLE_AutoSave = b;
	}

	//--

private:

	int key_MODE_AppMixer = OF_KEY_TAB;//default key to switch MODE_AppMixer
	int window_W, window_H;

	//autosave
	ofParameter<bool> ENABLE_AutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 10000;//10 secs

	//updating some params before save will trigs also the group callbacks
	//so we disable this callbacks just in case params updatings are required
	//in this case we will need to update gui position param
	bool DISABLE_Callbacks = false;

	//-

#pragma mark - INTERNAL PARAMS
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

#pragma mark - CALLBACKS
private:

	void Changed_params_Settings(ofAbstractParameter &e);
	void Changed_params_AppSession(ofAbstractParameter &e);

#pragma mark - OF LISTENERS
private:

	//keys
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	//mouse
	void mouseDragged(ofMouseEventArgs &eventArgs);
	void mousePressed(ofMouseEventArgs &eventArgs);
	void mouseReleased(ofMouseEventArgs &eventArgs);
	void addMouseListeners();
	void removeMouseListeners();

	//-

#pragma mark - FILE SETTINGS
private:

	//path folder and filenames
	std::string path_GLOBAL;
	std::string path_Params_AppSession;
	std::string path_Params_Preset;
	std::string path_Params_Mixer;
	std::string path_Params_Mask;

	void loadParams(ofParameterGroup &g, std::string path);
	void saveParams(ofParameterGroup &g, std::string path);

	void guiRefresh();

	//labels
	ofTrueTypeFont myFont;
	ofTrueTypeFont myFontHelp;
	ofTrueTypeFont myFontSmall;
	std::string myTTF;// gui font for all gui theme
	int sizeTTF;

	void drawPreviewsCheckerboard(float x, float y, float width, float height, float size);
	//TODO:could improve preformance cpu using fbo or loaded image..

	std::string helpInfo;
};
