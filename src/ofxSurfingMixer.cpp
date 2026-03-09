#include "ofxSurfingMixer.h"

//--------------------------------------------------------------
ofxSurfingMixer::ofxSurfingMixer() {
	bDisableCallbacks = true;

	// path for settings
	setPathGlobal("ofxSurfingMixer/");

	path_Params_AppSession = "AppSession_Settings.xml";
	path_Params_Preset = "Preset_Settings.xml";

	//-

	setActive(true); //add key and mouse listeners

	//-

	// labels font
	bool bLoaded;
	sizeTTF = 8;
	myTTF = "assets/fonts/";
	myTTF += "telegrama_render.otf";
	//myTTF += "overpass-mono-semibold.otf";
	myFont.load(myTTF, sizeTTF, true, true);
	bLoaded = myFontSmall.load(myTTF, sizeTTF - 2, true, true);
	if (bLoaded)
		ofLogNotice(__FUNCTION__) << "LOADED FILE '" << myTTF << "'";
	else
		ofLogError(__FUNCTION__) << "FILE '" << myTTF << "' NOT FOUND!";

	//-

	// help font
	sizeTTF = 10;
	myTTF = "assets/fonts/";
	myTTF += "overpass-mono-bold.otf";
	bLoaded = myFontHelp.load(myTTF, sizeTTF, true, true);
	if (bLoaded)
		ofLogNotice(__FUNCTION__) << "LOADED FILE '" << myTTF << "'";
	else
		ofLogError(__FUNCTION__) << "FILE '" << myTTF << "' NOT FOUND!";
}

//--------------------------------------------------------------
void ofxSurfingMixer::reallocate(int w, int h) {
	//TODO:
	//ofEnableArbTex();

	// fbo settings
	ofFbo::Settings settings;
	settings.width = w;
	settings.height = h;
	settings.internalformat = GL_RGBA;

	// allocate
	fbo_Input_1.allocate(settings);
	fbo_Input_2.allocate(settings);
	fbo_MixOut.allocate(settings);

	// clear fbo's
	fbo_Input_1.begin();
	ofClear(0, 255);
	fbo_Input_1.end();

	fbo_Input_2.begin();
	ofClear(0, 255);
	fbo_Input_2.end();

	fbo_MixOut.begin();
	ofClear(0, 255);
	fbo_MixOut.end();

	//-

	// mixer
	fbo_Mixer_A.allocate(settings);
	fbo_Mixer_B.allocate(settings);

	fbo_Mixer_A.begin();
	ofClear(0, 255);
	fbo_Mixer_A.end();

	fbo_Mixer_B.begin();
	ofClear(0, 255);
	fbo_Mixer_B.end();

	//--

	// blend
#ifdef INCLUDE_BLEND_MODE
	psBlend.setup(w, h);
#endif
}

//--------------------------------------------------------------
void ofxSurfingMixer::resizeFbos(int w, int h) {
	//TODO:
	ofEnableArbTex();

	fbo_Input_1.allocate(w, h);
	fbo_Input_2.allocate(w, h);
	fbo_MixOut.allocate(w, h);

	//mixer
	//fbo_Mixer_A.allocate(w, h);//BUG:
	//fbo_Mixer_B.allocate(w, h);
	mixerGpu.fboMix.allocate(w, h);

	//--
}

//--------------------------------------------------------------
void ofxSurfingMixer::guiCustomize() {
}

//--------------------------------------------------------------
void ofxSurfingMixer::guiSetupStyles() {
}
//--------------------------------------------------------------
void ofxSurfingMixer::guiSetup() {
}

//--------------------------------------------------------------
void ofxSurfingMixer::setupParamsInternal() {
	positionPreview.set("positionPreview",
		glm::vec2(5, 5),
		glm::vec2(0, 0),
		glm::vec2(window_W, window_H));

	//-

	// internal

	// params
	bModeActive.set("ACTIVE", true);
	MODE_AppMixer.set("MIXER MODE", 1, 1, NUM_MODES_APP);
	MODE_AppMixer_Name.set("", "");
	MODE_AppMixer_Name.setSerializable(false);
	//ENABLE_Debug.set("DEBUG", true);
	bEnableAutosave.set("AUTO SAVE", false);
	bGui.set("GUI", true);

	// params internal
	params_Internal.setName("INTERNAL");
	params_Internal.add(MODE_AppMixer);
	params_Internal.add(MODE_AppMixer_Name);
	params_Internal.add(bGui);
	params_Internal.add(bModeActive);
	params_Internal.add(bEnableAutosave);

#ifdef INCLUDE_ofxGui
	positionGui.set("GUI POSITION",
		glm::vec2(window_W * 0.5, window_H * 0.5),
		glm::vec2(0, 0),
		glm::vec2(window_W, window_H));
	params_Internal.add(positionGui);
#endif

	//-

	// user params
	params_UserGui.setName("USER");
	params_UserGui.add(MODE_AppMixer_Name);
	params_UserGui.add(ENABLE_Channel1);
	params_UserGui.add(ENABLE_Channel2);
	params_UserGui.add(bShowPreview);
	params_UserGui.add(bShowBackgrounds);
	params_UserGui.add(bSwapChannels);
	//params_UserGui.add(swapInfo);

	bEnableFx = false;

	//--

	// params to listeners and settings too
	params_AppSession.setName("AppSession");
	params_AppSession.add(params_Internal);
	params_AppSession.add(params_UserGui);
}

//--------------------------------------------------------------
void ofxSurfingMixer::setupParamsMixer() {
	// 1. blend

	//params_Blend.add(bEnableBlend);
	params_Blend.add(blendName);
	params_Blend.add(blendMode);

	//--

	// 2. mixer

	colorBackground.set("BACKGROUND", ofColor(255, 0, 0), ofColor(0, 0, 0), ofColor(255, 255, 255));
	//tex_Mixer_B = fbo_Mixer_B.getTexture(); // adding a texture insted of a fbo
	colorChannel.setup(colorBackground.getName(), colorBackground.get(), ofGetWidth(), ofGetHeight());

	mixerGpu.addChannel(colorChannel, ofxGpuMixer::BLEND_ADD);
	mixerGpu.addChannel(fbo_Mixer_A, "CHANNEL 1", ofxGpuMixer::BLEND_ADD);
	mixerGpu.addChannel(fbo_Mixer_B, "CHANNEL 2", ofxGpuMixer::BLEND_ADD);

	// creates the shader in order to mix.
	mixerGpu.setup();

	// for the settings only
	// different from guiSetup() bc there are for gui only
	params_Mixer.add(mixerGpu.getParameterGroup());
	params_Mixer.add(colorBackground);
	mixerGpu.reset();

	//--

	// backgrounds
	params_Backgrounds.add(colorBg1);
	params_Backgrounds.add(colorBg2);
	params_Backgrounds.add(colorBgMix);
	params_Backgrounds.add(bEnableBg1);
	params_Backgrounds.add(bEnableBg2);
	params_Backgrounds.add(bEnableBgMix);
	params_Backgrounds.add(bResetBackgrounds);

	bResetBackgrounds.setSerializable(false);

	//params_Blend.add(params_Backgrounds);

	//-

	blendName.setSerializable(false);
	swapInfo.setSerializable(false);

	//-

	// all params
	params_Basic.setName("Preset_Settings");
	params_Control.setName("Control");
	params_Control.add(ENABLE_Channel1);
	params_Control.add(ENABLE_Channel2);
	params_Control.add(bSwapChannels);
	params_Control.add(bShowBackgrounds);
	params_Control.add(swapInfo);
	params_Control.add(bEnableBlend);
	params_Control.add(bEnableMixer);
	params_Basic.add(params_Control);

	// modes
	params_Basic.add(params_Backgrounds);
	params_Basic.add(params_Blend);
	params_Basic.add(params_Mixer);

	//--
}

//--

// PRE-channel1
//--------------------------------------------------------------
void ofxSurfingMixer::begin_PRE_Channel_1() {
	if (bEnableFx) {
	} else if (!bEnableFx) // bypass fx
	{
		begin_Channel_1();
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_PRE_Channel_1() {
	if (bEnableFx) {
		begin_Channel_1();
		end_Channel_1();
	}

	else if (!bEnableFx) // bypass fx
	{
		end_Channel_1();
	}
}

//--

// PRE-channel2

//--------------------------------------------------------------
void ofxSurfingMixer::begin_PRE_Channel_2() {
	if (bEnableFx) {
	} else if (!bEnableFx) //bypass fx
	{
		begin_Channel_2();
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::setToggleFX(bool b) {
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_PRE_Channel_2() {
	if (bEnableFx) {
		begin_Channel_2();

		end_Channel_2();
	}

	else if (!bEnableFx) //bypass fx
	{
		end_Channel_2();
	}
}

//--

// channel1
//--------------------------------------------------------------
void ofxSurfingMixer::begin_Channel_1() {
	//ofLogVerbose(__FUNCTION__) << "begin_Channel_1";

	//--

	if (bEnableBlend) {
		fbo_Input_1.begin();
	}

	//--

	if (bEnableMixer) {
		if (!bSwapChannels) {
			if (ENABLE_Channel1)
				fbo_Mixer_A.begin();
		} else {
			if (ENABLE_Channel2)
				fbo_Mixer_B.begin();
		}
	}

	//--

	// background
	if (bEnableBg1) {
		ofClear(colorBg1);
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_Channel_1() {
	///*drawing happened before this*///

	//ofLogVerbose(__FUNCTION__) << "end_Channel_1";

	//--

	if (bEnableBlend) {
		fbo_Input_1.end();
	}

	//--

	if (bEnableMixer) {
		if (!bSwapChannels) {
			if (ENABLE_Channel1)
				fbo_Mixer_A.end();
		} else {
			if (ENABLE_Channel2)
				fbo_Mixer_B.end();
		}
	}

	//fbo_Input_1 update done

	//----

	// this mode do not requires process inside fbo's..
	if (bEnableBlend) {
		//process

#ifdef INCLUDE_BLEND_MODE
		if (bEnableBlend) {
			//TODO:
			//crash workaround...
			bBlendRunning = true;

			//blend base
			psBlend.begin();
		}
#endif
		//----

		if (!bSwapChannels) {
			if (ENABLE_Channel1) {
				fbo_Input_1.draw(0, 0);
			}
		} else {
			if (ENABLE_Channel2) {
				fbo_Input_2.draw(0, 0);
			}
		}

		//----

#ifdef INCLUDE_BLEND_MODE
		if (bEnableBlend) {
			psBlend.end();
		}
#endif
	}
}

//--

// channel2
//--------------------------------------------------------------
void ofxSurfingMixer::begin_Channel_2() {
	//ofLogVerbose(__FUNCTION__) << "begin_Channel_2";

	if (bEnableBlend) {
		//blend target
		fbo_Input_2.begin();
	}

	//--

	if (bEnableMixer) {
		if (!bSwapChannels) {
			if (ENABLE_Channel2)
				fbo_Mixer_B.begin();
		} else {
			if (ENABLE_Channel1)
				fbo_Mixer_A.begin();
		}
	}

	//--

	if (bEnableBg2) {
		ofClear(colorBg2);
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_Channel_2() {
	///*drawing happened before this*///

	//ofLogVerbose(__FUNCTION__) << "end_Channel_2";

	if (bEnableBlend) {
		fbo_Input_2.end();
	}

	//--

	if (bEnableMixer) {
		if (!bSwapChannels) {
			if (ENABLE_Channel2) fbo_Mixer_B.end();
		} else {
			if (ENABLE_Channel1) fbo_Mixer_A.end();
		}

		//fbo_Mixer_B.end();
	}
}

//--

// mixer
//--------------------------------------------------------------
void ofxSurfingMixer::begin_Mix() {
	//ofLogVerbose(__FUNCTION__) << "begin_Mix";

	//--

	//mixes fbo_Input_1 + fbo_Input_2
	fbo_MixOut.begin();

	//-

	//background
	if (bEnableBgMix) {
		ofClear(colorBgMix);
	}

	//-

	// draw

	//-

	// 1. blend

#ifdef INCLUDE_BLEND_MODE
	if (bEnableBlend) {
		if (!bSwapChannels) {
			if (ENABLE_Channel2) psBlend.draw(fbo_Input_2.getTextureReference(), blendMode);
		} else {
			if (ENABLE_Channel1) psBlend.draw(fbo_Input_1.getTextureReference(), blendMode);
		}
	}
#endif

	//-
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_Mix() {
	//ofLogVerbose(__FUNCTION__) << "end_Mix";
	//ofLogVerbose(__FUNCTION__) << "---------------------------------------";

	//--

	// 2. mixer

	if (bEnableMixer) {
		ofSetColor(255);

		//raw draw
		mixerGpu.draw(0, 0, ofGetWidth(), ofGetHeight());

		//fliped draw
		//mixerGpu.drawFbo(0, 0, ofGetWidth(), ofGetHeight());
	}

	//--

	fbo_MixOut.end();
}

//--------------------------------------------------------------
void ofxSurfingMixer::drawMixer() {

	//--

#ifdef INCLUDE_BLEND_MODE
	if (bEnableBlend) {
		fbo_MixOut.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
#endif

	if (bEnableMixer) {
		ofSetColor(255);
		fbo_MixOut.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::drawPreviews(float x, float y, float _w) {
	int SELECTED_Channel = mixerGpu.channelSelect.get();

	int numScreens;

	if (bShowBackgrounds)
		numScreens = 3;
	else
		numScreens = 2;

	int alpha = 200;
	float w;
	w = _w;
	//w = window_W / 8.0f;
	float ratio = (float)window_H / window_W;
	float h = w * ratio;
	int pad = 5; //space borders between the 3 preview screens
	//labels padding text from screen left & lower border
	float xLabel = 4;
	float yLabel = sizeTTF + 4;
	//paddin for background labels into screens
	int pad_xScreLb = 4;
	int pad_yScreLb = 12;

	ofPushStyle();

	//black rectangle background
	ofFill();
	ofSetColor(0, alpha);
	ofRectangle r { x, y, pad + numScreens * (w + pad), h + 2 * pad + 2.5f * yLabel };
	ofDrawRectRounded(r, 3.0f);

	//monitors preview
	ofSetColor(255, 255);

	float xx0, xx1, xx2, xx3;
	float yy0, yy1, yy2, yy3;
	if (bShowBackgrounds) //3 monitors
	{
		xx0 = pad + x;
		yy0 = pad + y;
		xx1 = xx0 + (w + pad);
		yy1 = yy0;
		xx2 = xx1 + (w + pad);
		yy2 = yy0;
		xx3 = xx2 + (w + pad);
		yy3 = yy0;
	} else //2 monitors
	{
		xx1 = pad + x;
		yy1 = pad + y;
		xx2 = xx1 + (w + pad);
		yy2 = yy1;
		xx3 = xx2 + (w + pad);
		yy3 = yy1;
		xx0 = xx1;
		yy0 = yy1;
	}

	//labels
	//ofPushMatrix();
	ofSetColor(255);

	if (bShowBackgrounds) myFont.drawString("BACKGROUNDS", xx0 + xLabel, yy0 + h + yLabel + 5);
	myFont.drawString("CHANNEL 1", xx1 + xLabel, yy1 + h + yLabel + 5);
	myFont.drawString("CHANNEL 2", xx2 + xLabel, yy2 + h + yLabel + 5);
	//ofPopMatrix();

	//-

	////1st preview where we draw background or channel 0 in mixer mode
	//ofRectangle r0{ xx0, yy0, w, h };
	////draw grey backgroung viewed when some of the 3 backgrounds are disabled
	//ofPushStyle();
	//ofFill();
	//ofSetColor(128);
	//ofDrawRectangle(r0);
	//ofPopStyle();

	//-

	float wThird; //ration from fullsize/screen image

	//cheackboard
	float sizeBox;
	//sizeBox = wThird / 8.0f;
	sizeBox = h / 20.0f;

	//mixer
	if (bEnableMixer) {
		//backgrounds
		//1st half screen splitted by the 3 backgrounds. seccond half will be for channel 0

		wThird = 0.5f * w / 3.0f;

		ofPushStyle();
		ofFill();

		if (bShowBackgrounds) {
			if (bEnableBg1) {
				ofSetColor(colorBg1);
				ofDrawRectangle(ofRectangle { xx0, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG1", xx0 + pad_xScreLb, yy0 + pad_yScreLb);

			if (bEnableBg2) {
				ofSetColor(colorBg2);
				ofDrawRectangle(ofRectangle { xx0 + wThird, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0 + wThird, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG2", xx0 + wThird + pad_xScreLb, yy0 + pad_yScreLb);

			if (bEnableBgMix) //independent of swap channels
			{
				ofSetColor(colorBgMix);
				ofDrawRectangle(ofRectangle { xx0 + 2 * wThird, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0 + 2 * wThird, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BGMIX", xx0 + 2 * wThird + pad_xScreLb, yy0 + pad_yScreLb);

			//-

			//channel 0
			//2nd half will be fille with channel 0 from mixer
			if (SELECTED_Channel == 0) {
				drawPreviewBorders(xx0 + 0.5f * w, yy0, 0.5f * w, h);
			}
			ofFill();
			ofSetColor(colorBackground);
			ofDrawRectangle(ofRectangle { xx0 + 0.5f * w, yy0, 0.5f * w, h });
			ofSetColor(255);
			myFontSmall.drawString("CH0", xx0 + 0.5f * w + pad_xScreLb, yy0 + pad_yScreLb);
		}

		ofPopStyle();

		//-

		//channel 1
		if (ENABLE_Channel1) {
			fbo_Mixer_A.draw(xx1, yy1, w, h);
			if (SELECTED_Channel == 1) {
				drawPreviewBorders(xx1, yy1, w, h);
			}
		}

		//-

		//channel 2
		if (ENABLE_Channel2) {
			fbo_Mixer_B.draw(xx2, yy2, w, h);
			if (SELECTED_Channel == 2) {
				drawPreviewBorders(xx2, yy2, w, h);
			}
		}

		//fbo_MixOut.draw(xx3, yy3, w, h);
	}

	//blender
#ifdef INCLUDE_BLEND_MODE
	if (bEnableBlend) {
		//backgrounds

		wThird = w / 3.0f;

		ofPushStyle();
		ofFill();

		if (bShowBackgrounds) {
			if (bEnableBg1) {
				ofSetColor(colorBg1);
				ofDrawRectangle(ofRectangle { xx0, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG1", xx0 + pad_xScreLb, yy0 + pad_yScreLb);

			if (bEnableBg2) {
				ofSetColor(colorBg2);
				ofDrawRectangle(ofRectangle { xx0 + wThird, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0 + wThird, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG2", xx0 + wThird + pad_xScreLb, yy0 + pad_yScreLb);

			if (bEnableBgMix) //independent of swap channels
			{
				ofSetColor(colorBgMix);
				ofDrawRectangle(ofRectangle { xx0 + 2 * wThird, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0 + 2 * wThird, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BGMIX", xx0 + 2 * wThird + pad_xScreLb, yy0 + pad_yScreLb);
		}

		ofPopStyle();

		//-

		if (!bSwapChannels) {
			if (ENABLE_Channel1) {
				fbo_Input_1.draw(xx1, yy1, w, h);
			}
			if (ENABLE_Channel2) {
				fbo_Input_2.draw(xx2, yy2, w, h);
				//fbo_MixOut.draw(xx3, yy3, w, h);
			}
		} else {
			if (ENABLE_Channel1) {
				fbo_Input_2.draw(xx1, yy1, w, h);
			}
			if (ENABLE_Channel2) {
				fbo_Input_1.draw(xx2, yy2, w, h);
				//fbo_MixOut.draw(xx3, yy3, w, h);
			}
		}
	}
#endif

	ofPopStyle();

	//--

	//checkboard
	if (!ENABLE_Channel1)
		drawPreviewsCheckerboard(xx1, yy1, w, h, sizeBox);

	if (!ENABLE_Channel2)
		drawPreviewsCheckerboard(xx2, yy2, w, h, sizeBox);

	//fbo_MixOut.draw(xx3, yy3, w, h);

	//--

	// text labels

	// overlay debug
	int num = 7; // lines from bottom border
	int hh = 20;
	int xx = 0;
	//int yy = ofGetHeight() - (num * hh) - 5;//botom border
	//int yy = y + h + 2 * pad - 7;//below monitor preview
	int yy;
	if (bShowBackgrounds)
		yy = yy0 + h + 2.5f * yLabel;
	else
		yy = yy1 + h + 2.5f * yLabel;

	////channels swap
	//std::string str1, str2;
	//if (!bSwapChannels)
	//{
	//	str1 = "CHANNEL1: " + nameChannel1;
	//	str2 = "CHANNEL2: " + nameChannel2;
	//}
	//else
	//{
	//	str1 = "CHANNEL1: " + nameChannel2;
	//	str2 = "CHANNEL2: " + nameChannel1;
	//}
	//ofDrawBitmapStringHighlight(str1, xx, yy + hh);
	//ofDrawBitmapStringHighlight(str2, xx, yy + 2 * hh);

	// mode
	if (bEnableBlend) {
		string str = "";
		str += "BLEND\t\t";
		str += (blendMode.get() < 10 ? " " : "");
		str += ofToString(blendMode.get()); // +"] ";
		str += " ";
		str += ofToUpper(blendName.get());
		//ofDrawBitmapStringHighlight
		myFont.drawString(str, xx0 + xLabel, yy);
	} else if (bEnableMixer) {
		std::string str;
		str = "MIXER\t\t";
		str += "CH-" + ofToString(mixerGpu.channelSelect.get()) + "";
		if (mixerGpu.channelSelect.get() == 0) {
			str += "\t\tBACKGROUND ";
			//str += "\t(BLEND MODE\t";
		} else {
			str += "\t\tBLEND MODE\t";
		}
		if (mixerGpu.channelSelect.get() != 0) //except when ch is bg
		{
			str += (mixerGpu.getBlendMode() < 10 ? " " : "");
			str += ofToString(mixerGpu.getBlendMode());
			str += " ";
			str += ofToUpper(mixerGpu.getBlendModeName());
		}
		//if (mixerGpu.channelSelect.get() == 0)
		//{
		//	str += " )";
		//}

		myFont.drawString(str, xx0 + xLabel, yy);
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::drawGui() {
	//ofLogNotice(__FUNCTION__);

	//blend and mask
	if (bGuiAdv) {
#ifdef INCLUDE_ofxGui
		gui_Control.draw(); //all guis together
#endif
	}

	//-

	// preview monitors
	drawPreviews(positionPreview.get().x, positionPreview.get().y, window_W / 9.0f);
}

//--------------------------------------------------------------
void ofxSurfingMixer::updateMixer() {

	// mixer
	if (bEnableMixer) {
		mixerGpu.update();

		//easy callbacks
		//if (mixerGpu.isUpdated())//not working?
		//{
		//	guiRefresh();
		//}

		if (mixerGpu.isChangedColor()) {
			colorBackground = mixerGpu.getColorChannel0();
		}
	}

	//--

	//blend/mix the 2 channels
	begin_Mix();
	end_Mix();
}

//--------------------------------------------------------------
void ofxSurfingMixer::setup() {
	bDisableCallbacks = true;

	//--

	//log mode
	//ofSetLogLevel(OF_LOG_SILENT);
	ofSetLogLevel("ofxSurfingMixer", OF_LOG_NOTICE);
	//ofSetLogLevel("ofxSurfingMixer", OF_LOG_SILENT);
	ofSetLogLevel("ofxGpuMixer", OF_LOG_VERBOSE);

	//--

	//screen
	window_W = ofGetWidth();
	window_H = ofGetHeight();

	//TODO:
	ofEnableArbTex();

	//allocate fbo's
	reallocate(window_W, window_H);

	//--

	//setup all mixer params
	setupParamsMixer();

	//--

	setupParamsInternal();

	//--

	//gui internal
	//gui theme
#ifdef INCLUDE_ofxGui
	//std::string str = "assets/fonts/overpass-mono-bold.otf";
	//ofFile file(str);
	//if (file.exists()) {
	//	ofxGuiSetFont(str, 9);
	//} else {
	//	ofLogError(__FUNCTION__) << "setup() ofxGuiSetFont '" << str << "' NOT FOUND!";
	//}
	//ofxGuiSetDefaultHeight(20);
	//ofxGuiSetBorderColor(32);
	//ofxGuiSetFillColor(ofColor(48));
	//ofxGuiSetTextColor(ofColor::white);
	//ofxGuiSetHeaderColor(ofColor(24));

	//setup gui advanced (ofxGui)
	gui_Control.setup("ofxSurfingMixer");
	gui_Control.add(params_Internal); //add internal params
	gui_Control.add(params_UserGui); //add user params
	gui_Control.add(params_Basic); //add mixer addon params

	auto & gInternal = gui_Control.getGroup("INTERNAL"); //1st level
	gInternal.minimize();

	//-

	////build
	////gui internal
	//gui_Mixer.setup("MIXER");
	//gui_Mixer.add(mixerGpu.getParameterGroup());
#endif

	//--

	//advanced gui - ofxGuiExtended
	guiSetup();
	guiCustomize();

	//--

	//callbacks

	ofAddListener(params_UserGui.parameterChangedE(), this, &ofxSurfingMixer::Changed_params_AppSession);
	ofAddListener(params_Internal.parameterChangedE(), this, &ofxSurfingMixer::Changed_params_AppSession);
	ofAddListener(params_Basic.parameterChangedE(), this, &ofxSurfingMixer::Changed_params_Settings);
	//must create callback for this group only, now we are sharing with internal callback

	//--

	//presetsManager

	params_Preset.setName("ofxSurfingMixer");
	params_Preset.add(params_Basic); //all
	//params_Preset.add(params_Mixer);
	//params_Preset.add(params_Blend);

	//--------------------------------------------------------------

	//startup

	ofLogNotice(__FUNCTION__) << "STARTUP INIT";

	bDisableCallbacks = false;

	startup();
}

//--------------------------------------------------------------
void ofxSurfingMixer::startup() {
	bModeActive = true;
	bEnableAutosave = false;

	//-

	//settings
	loadParams(params_AppSession, path_GLOBAL + path_Params_AppSession);

	loadParams(params_Preset, path_GLOBAL + path_Params_Preset); //all
	//loadParams(params_Mixer, path_GLOBAL + path_Params_Mixer);

	guiRefresh();

#ifdef INCLUDE_BLEND_MODE
	std::string message = ofToUpper(psBlend.getBlendMode(blendMode));
	ofStringReplace(message, "BLEND", "");
	blendName = message;
#endif

	//will set gui's position after window setup/resizing
	windowResized(window_W, window_H);
}

//--------------------------------------------------------------
void ofxSurfingMixer::update() {
	updateEngine();
	updateMixer();
}

//--------------------------------------------------------------
void ofxSurfingMixer::updateEngine() {
	//ofLogNotice(__FUNCTION__) << "ofGetUsingArbTex()"<< ofToString(ofGetUsingArbTex());

	//--

	// autosave
	if (bEnableAutosave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave) {
		bDisableCallbacks = true;

		//get gui position before save
#ifdef INCLUDE_ofxGui
		positionGui = glm::vec2(gui_Control.getPosition());
#endif

		saveParams(params_AppSession, path_GLOBAL + path_Params_AppSession);

		saveParams(params_Preset, path_GLOBAL + path_Params_Preset);

		//saveParams(params_Mixer, path_GLOBAL + path_Params_Mixer);

		timerLast_Autosave = ofGetElapsedTimeMillis();
		if (true) ofLogNotice(__FUNCTION__) << "\t\t\t\t\t\t\t\t\t[AUTOSAVE]";

		bDisableCallbacks = false;
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::exit() {
	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";

	bDisableCallbacks = true;

	//-

//get gui position before save
#ifdef INCLUDE_ofxGui
	positionGui = glm::vec2(gui_Control.getPosition());
#endif

	saveParams(params_AppSession, path_GLOBAL + path_Params_AppSession);

	saveParams(params_Preset, path_GLOBAL + path_Params_Preset); //all settings

	//saveParams(params_Mixer, path_GLOBAL + path_Params_Mixer);
}

//--------------------------------------------------------------
ofxSurfingMixer::~ofxSurfingMixer() {
	//remove keys and mouse listeners
	removeKeysListeners();
	removeMouseListeners();

	//remove params callbacks listeners
	ofRemoveListener(params_UserGui.parameterChangedE(), this, &ofxSurfingMixer::Changed_params_AppSession);
	ofRemoveListener(params_Internal.parameterChangedE(), this, &ofxSurfingMixer::Changed_params_AppSession);
	ofRemoveListener(params_Basic.parameterChangedE(), this, &ofxSurfingMixer::Changed_params_Settings);

	exit();
}

//--------------------------------------------------------------
void ofxSurfingMixer::setLogLevel(ofLogLevel level) {
	ofSetLogLevel("ofxSurfingMixer", level);
}

//--------------------------------------------------------------
void ofxSurfingMixer::windowResized(int w, int h) {
	window_W = w;
	window_H = h;

	//BUG: on ofxGpuMixer
	//TODO: should convert fbo to &fbo to allow re-allocate
	//reallocatewindow_W, window_H);

	resizeFbos(window_W, window_H);

	//mixer
	//panel_MIXER->setPosition(positionPreview.get().x + 5, positionPreview.get().y + 165);
	//panel_MIXER->setPosition(window_W - (6 * 200), 5);
}

//--

//keys
//--------------------------------------------------------------
void ofxSurfingMixer::keyPressed(ofKeyEventArgs & eventArgs) {
	const int & key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << " '" << (char)key << "' [" << key << "]";

	//modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	if (false) {
		ofLogNotice(__FUNCTION__) << "mod_COMMAND: " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL: " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT: " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT: " << (mod_SHIFT ? "ON" : "OFF");
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::keyReleased(ofKeyEventArgs & eventArgs) {
	const int & key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << (char)key << " [" << key << "]";

	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);
}

//--------------------------------------------------------------
void ofxSurfingMixer::addKeysListeners() {
	ofAddListener(ofEvents().keyPressed, this, &ofxSurfingMixer::keyPressed);
}

//--------------------------------------------------------------
void ofxSurfingMixer::removeKeysListeners() {
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingMixer::keyPressed);
}

//--------------------------------------------------------------
void ofxSurfingMixer::mouseDragged(ofMouseEventArgs & eventArgs) {
	const int & x = eventArgs.x;
	const int & y = eventArgs.y;
	const int & button = eventArgs.button;
	ofLogVerbose(__FUNCTION__) << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxSurfingMixer::mousePressed(ofMouseEventArgs & eventArgs) {
	const int & x = eventArgs.x;
	const int & y = eventArgs.y;
	const int & button = eventArgs.button;
	ofLogVerbose(__FUNCTION__) << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxSurfingMixer::mouseReleased(ofMouseEventArgs & eventArgs) {
	const int & x = eventArgs.x;
	const int & y = eventArgs.y;
	const int & button = eventArgs.button;
	ofLogVerbose(__FUNCTION__) << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxSurfingMixer::addMouseListeners() {
	ofAddListener(ofEvents().mouseDragged, this, &ofxSurfingMixer::mouseDragged);
	ofAddListener(ofEvents().mousePressed, this, &ofxSurfingMixer::mousePressed);
	ofAddListener(ofEvents().mouseReleased, this, &ofxSurfingMixer::mouseReleased);
}

//--------------------------------------------------------------
void ofxSurfingMixer::removeMouseListeners() {
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingMixer::keyPressed);
}

//--------------------------------------------------------------
void ofxSurfingMixer::setActive(bool b) {
	//disables all keys and mouse interaction listeners from the addon

	bModeActive = b;
	//enable/disable all keys bc the main addon where the addons runs could be
	//controlling other things/addons or mode apps..

	//inactive
	if (!b) {
		removeKeysListeners();
		removeMouseListeners();

		//gui
		bGui = false;
	}

	//active
	else {
		addKeysListeners();
		addMouseListeners();

		//gui
		bGui = true;
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::setGuiVisible(bool b) {
	bGui = b;
}

//addon params
//--------------------------------------------------------------
void ofxSurfingMixer::Changed_params_Settings(ofAbstractParameter & e) {
	if (!bDisableCallbacks) {
		std::string name = e.getName();

		//exclude debugs
		if (name != "" && name != " ") {
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		//filter
		if (false) {
		}

		//modes
		else if (name == bEnableBlend.getName()) {
			bDisableCallbacks = true;
			if (bEnableBlend) {
				bEnableMixer = false;
				MODE_AppMixer = 2;
			}
			bDisableCallbacks = false;

			guiRefresh();
		} else if (name == bEnableMixer.getName()) {
			bDisableCallbacks = true;
			if (bEnableMixer) {
				bEnableBlend = false;
				MODE_AppMixer = 1;
			}
			bDisableCallbacks = false;

			guiRefresh();
		}

		//-

		//blend
#ifdef INCLUDE_BLEND_MODE
		//crashes when psBlend is not running yet in setup
		else if (name == blendMode.getName() && bBlendRunning) {
			std::string message = ofToUpper(psBlend.getBlendMode(blendMode));
			ofStringReplace(message, "BLEND", "");
			blendName = message;
		}
#endif
		//-

		else if (name == "RESET") {
			bDisableCallbacks = true;
			if (bResetBackgrounds) {
				bResetBackgrounds = false;

				colorBg1 = ofColor(0, 255);
				colorBg2 = ofColor(0, 255);
				colorBgMix = ofColor(0, 255);
				ENABLE_Channel1 = true;
				ENABLE_Channel2 = true;
				bEnableBg1 = true;
				bEnableBg2 = true;
				bEnableBgMix = true;
			}
			if (bEnableMixer) {
				mixerGpu.setColorChannel0(ofColor(0, 255));
			}
			bDisableCallbacks = false;
		}

		//-

		else if (name == bSwapChannels.getName()) {
			if (!bSwapChannels) {
				swapInfo = "CH1 * CH2";
			} else {
				swapInfo = "CH2 * CH1";
			}
		}

		//mixer
		else if (name == mixerGpu.channelSelect.getName()) {
			guiRefresh();
		}
	}
}

//addon internal params
//--------------------------------------------------------------
void ofxSurfingMixer::Changed_params_AppSession(ofAbstractParameter & e) {
	if (!bDisableCallbacks) {
		std::string name = e.getName();

		//exclude debugs
		if (name != ""
			&& name != "exclude") {
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		if (false) {
		}

		else if (name == "MIXER MODE") {
			switch (MODE_AppMixer) {

				//case 0:
				//	MODE_AppMixer_Name = "RUN";
				//	//setActive(false);
				//	break;

			case 1:
				MODE_AppMixer_Name = "MODE 1 BLEND";
				break;

			case 2:
				MODE_AppMixer_Name = "MODE 2 MIXER";
				break;

			default:
				MODE_AppMixer_Name = "UNKNOWN";
				break;
			}
		}
#ifdef INCLUDE_ofxGui
		else if (name == "GUI POSITION") {
			gui_Control.setPosition(positionGui.get().x, positionGui.get().y);
		}
#endif
		else if (name == "ACTIVE") {
			setActive(bModeActive);
		}

		//-

		//gui
		else if (name == "GUI") {
			//mixer
		}

		else if (name == "HELP") {
		} else if (name == "DEBUG") {
		}

		else if (name == "KEYS") {
		}

		//----

		// workflow
		// clear fbos to void freeze
		else if (name == ENABLE_Channel1.getName() || name == ENABLE_Channel2.getName()) {
			//clear fbo's
			fbo_Input_1.begin();
			ofClear(0, 255);
			fbo_Input_1.end();

			fbo_Input_2.begin();
			ofClear(0, 255);
			fbo_Input_2.end();

			fbo_MixOut.begin();
			ofClear(0, 255);
			fbo_MixOut.end();

			fbo_Mixer_A.begin();
			ofClear(0, 255);
			fbo_Mixer_A.end();

			fbo_Mixer_B.begin();
			ofClear(0, 255);
			fbo_Mixer_B.end();
		}

		else if (name == bShowPreview.getName()) {
			//gUser->getControl(bShowBackgrounds.getName())->setEnabled(bShowPreview.get());
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::setKey_MODE_App(int k) {
	key_MODE_AppMixer = k;
}

//--------------------------------------------------------------
void ofxSurfingMixer::setPathGlobal(std::string s) //must call before setup. disabled by default
{
	path_GLOBAL = s;

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
}

//--------------------------------------------------------------
void ofxSurfingMixer::loadParams(ofParameterGroup & g, std::string path) {
	ofLogNotice(__FUNCTION__) << path;

	ofXml settings;
	settings.load(path);
	ofDeserialize(settings, g);
}

//--------------------------------------------------------------
void ofxSurfingMixer::saveParams(ofParameterGroup & g, std::string path) {
	//TODO:
	//if (!bEnableAutosave && !bDisableCallbacks)//reduce double log when autosave happens
	{
		ofLogNotice(__FUNCTION__) << path;
	}

	ofXml settings;
	ofSerialize(settings, g);
	settings.save(path);
}

//--------------------------------------------------------------
void ofxSurfingMixer::guiRefresh() {
	//if (0)
	{
		ofLogNotice(__FUNCTION__);

		//--

		//--

		//workflow: set a default mode if all are disabled
		if (!bEnableBlend && !bEnableMixer) {
			MODE_AppMixer = 2;
			bEnableMixer = true;
		}

		//----

		// internal ofxGui
#ifdef INCLUDE_ofxGui
		// blend gui pannel
		auto & gInternal = gui_Control.getGroup("INTERNAL"); //1st level
		//gInternal.minimize();

		auto & gGuiPos = gInternal.getGroup("GUI POSITION"); //2nd level
		gGuiPos.minimize();

		// addon settings
		auto & gSettings = gui_Control.getGroup(params_Basic.getName()); //1st level

		// debug
		auto & gDebug = gSettings.getGroup("BACKGROUNDS"); //2nd level
		gDebug.minimize();

		// modes
		auto & gBlend = gSettings.getGroup(params_Blend.getName()); //2nd level

		// collapse all
		gBlend.minimize();

		//modes
		if (bEnableBlend) {
			gBlend.maximize();
		} else if (bEnableMixer) {
			gBlend.minimize();
		}
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::drawPreviewsCheckerboard(float x, float y, float width, float height, float size) {
	ofPushStyle();

	//TODO: should replace to a checkerbg.png

	ofColor c1, c2;
	int a = 200;
	c1.set(ofColor(0, a));
	c2.set(ofColor(48, a));

	ofFill();
	ofSetColor(c1);
	ofDrawRectangle(ofRectangle { x, y, width, height });

	ofPushMatrix();
	ofTranslate(x, y);
	ofSetColor(c2);

	int numWidth = width / size;
	int numHeight = height / size;
	for (int h = 0; h < numHeight; h++) {
		for (int w = 0; w < numWidth; w++) {
			if ((h + w) % 2 == 0) {
				ofDrawRectangle(w * size, h * size, size, size);
			}
		}
	}

	ofPopMatrix();
	ofPopStyle();
}
