#include "ofxSurfingMixer.h"

//--------------------------------------------------------------
ofxSurfingMixer::ofxSurfingMixer() {
	DISABLE_Callbacks = true;

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

	//-

	//build help
	helpInfo = "HELP\nofxSurfingMixer";
	helpInfo += "\n\n";
	helpInfo += "H                 SHOW HELP";
	helpInfo += "\n";
	helpInfo += "G                 SHOW GUI";
	helpInfo += "\n";
	helpInfo += "F1                SHOW GUI";
	helpInfo += "\n";
	helpInfo += "F2                SHOW PREVIEWS";
	helpInfo += "\n";
	helpInfo += "F3                SHOW PRESETS";
	helpInfo += "\n";
	helpInfo += "\n";
	helpInfo += " <|>              BLEND TYPE";
	helpInfo += "\n";
	helpInfo += "TAB               MODE BLEND-MIX";
	helpInfo += "\n";
	helpInfo += "TAB+Ctrl          MODE FX";
	helpInfo += "\n";
	helpInfo += "/                 SWAP CHANNELS";
	helpInfo += "\n";
	helpInfo += "K+Ctrl            ENABLE KEYS";
	helpInfo += "\n";
	helpInfo += "J                 LOAD THEME";
	helpInfo += "\n";
	helpInfo += "\n";
	helpInfo += "MODE MIXER";
	helpInfo += "\n";
	helpInfo += " <|>              BLEND TYPE";
	helpInfo += "\n";
	helpInfo += "up|down           CHANNEL";
	helpInfo += "\n";
	helpInfo += "S                 SOLO";
	helpInfo += "\n";
	helpInfo += "\n";
	helpInfo += "\n";
}

//--------------------------------------------------------------
void ofxSurfingMixer::reallocate(int w, int h) {
	//TODO:
	//ofEnableArbTex();

	//fbo settings
	ofFbo::Settings settings;
	settings.width = w;
	settings.height = h;
	settings.internalformat = GL_RGBA;

	//allocate
	fbo_Input_1.allocate(settings);
	fbo_Input_2.allocate(settings);
	fbo_MixOut.allocate(settings);

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

	//-

	// mixer
#ifdef INCLUDE_MIXER_MODE
	fbo_Mixer_A.allocate(settings);
	fbo_Mixer_B.allocate(settings);

	fbo_Mixer_A.begin();
	ofClear(0, 255);
	fbo_Mixer_A.end();

	fbo_Mixer_B.begin();
	ofClear(0, 255);
	fbo_Mixer_B.end();
#endif

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
#ifdef INCLUDE_MIXER_MODE
	//fbo_Mixer_A.allocate(w, h);//BUG:
	//fbo_Mixer_B.allocate(w, h);
	mixerGpu.fboMix.allocate(w, h);
#endif

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
	preview_Position.set("preview_Position",
		glm::vec2(5, 5),
		glm::vec2(0, 0),
		glm::vec2(window_W, window_H));

	//-

	// internal

	// params
	MODE_Active.set("ACTIVE", true);
	ENABLE_keys_AllMixer.set("KEYS", true);
	SHOW_Help.set("HELP", true);
	MODE_AppMixer.set("MIXER MODE", 1, 1, NUM_MODES_APP);
	MODE_AppMixer_Name.set("", "");
	MODE_AppMixer_Name.setSerializable(false);
	//ENABLE_Debug.set("DEBUG", true);
	ENABLE_AutoSave.set("AUTO SAVE", false);
	SHOW_GuiAll.set("GUI", true);
	SHOW_MIXER.set("SHOW_MIXER", true);

	// params internal
	params_Internal.setName("INTERNAL");
	params_Internal.add(MODE_AppMixer);
	params_Internal.add(MODE_AppMixer_Name);
	params_Internal.add(SHOW_GuiAll);
	params_Internal.add(MODE_Active);
	params_Internal.add(ENABLE_keys_AllMixer);
	params_Internal.add(ENABLE_AutoSave);

#ifdef INCLUDE_ofxGui
	Gui_Position.set("GUI POSITION",
		glm::vec2(window_W * 0.5, window_H * 0.5),
		glm::vec2(0, 0),
		glm::vec2(window_W, window_H));
	params_Internal.add(Gui_Position);
#endif

	//-

	// user params
	params_UserGui.setName("USER");
	params_UserGui.add(MODE_AppMixer_Name);
	params_UserGui.add(ENABLE_Channel1);
	params_UserGui.add(ENABLE_Channel2);
	params_UserGui.add(SHOW_Preview);
	params_UserGui.add(SHOW_Backgrounds);
	params_UserGui.add(swapChannels);
	//params_UserGui.add(swapInfo);

	MODE_PRESET_MIXER = false;

	ENABLE_FboFxHelper = false;
	MODE_SHOW_FboFxHelper = false;

	//--

	// params to listeners and settings too
	params_AppSession.setName("AppSession");
	params_AppSession.add(params_Internal);
	params_AppSession.add(params_UserGui);
}

//--------------------------------------------------------------
void ofxSurfingMixer::setupParamsMixer() {
	// 1. blend

	//params_Blend.add(ENABLE_BLEND);
	params_Blend.add(blendName);
	params_Blend.add(blendMode);

#ifdef BLEND_TOGGLES
	ENABLE_Blends.clear();
	ENABLE_Blends.resize(psBlend.getNumBlends());
	for (int i = 0; i < ENABLE_Blends.size(); i++) {
		ENABLE_Blends[i].set(psBlend.getBlendMode(i), false);
		params_Blend.add(ENABLE_Blends[i]);
	}
#endif

	//--

	// 2. mixer

#ifdef INCLUDE_MIXER_MODE
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
#endif

	mixerGpu.reset();

	//--

	// backgrounds
	params_Backgrounds.add(colorBg1);
	params_Backgrounds.add(colorBg2);
	params_Backgrounds.add(colorBgMix);
	params_Backgrounds.add(ENABLE_Bg1);
	params_Backgrounds.add(ENABLE_Bg2);
	params_Backgrounds.add(ENABLE_BgMix);
	params_Backgrounds.add(RESET_Backgrounds);

	RESET_Backgrounds.setSerializable(false);

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
	params_Control.add(swapChannels);
	params_Control.add(SHOW_Backgrounds);
	params_Control.add(swapInfo);
	params_Control.add(ENABLE_BLEND);
	params_Control.add(ENABLE_MIXER);
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
	if (ENABLE_FboFxHelper) {
	} else if (!ENABLE_FboFxHelper) // bypass fx
	{
		begin_Channel_1();
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_PRE_Channel_1() {
	if (ENABLE_FboFxHelper) {
		begin_Channel_1();
		end_Channel_1();
	}

	else if (!ENABLE_FboFxHelper) // bypass fx
	{
		end_Channel_1();
	}
}

//--

// PRE-channel2

//--------------------------------------------------------------
void ofxSurfingMixer::begin_PRE_Channel_2() {
	if (ENABLE_FboFxHelper) {
	} else if (!ENABLE_FboFxHelper) //bypass fx
	{
		begin_Channel_2();
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::setToggleFX(bool b) {
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_PRE_Channel_2() {
	if (ENABLE_FboFxHelper) {
		begin_Channel_2();

		end_Channel_2();
	}

	else if (!ENABLE_FboFxHelper) //bypass fx
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

	if (ENABLE_BLEND) {
		fbo_Input_1.begin();
	}

	//--

#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		if (!swapChannels) {
			if (ENABLE_Channel1)
				fbo_Mixer_A.begin();
		} else {
			if (ENABLE_Channel2)
				fbo_Mixer_B.begin();
		}
	}
#endif

	//--

	// background
	if (ENABLE_Bg1) {
		ofClear(colorBg1);
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_Channel_1() {
	///*drawing happened before this*///

	//ofLogVerbose(__FUNCTION__) << "end_Channel_1";

	//--

	if (ENABLE_BLEND) {
		fbo_Input_1.end();
	}

	//--

#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		if (!swapChannels) {
			if (ENABLE_Channel1)
				fbo_Mixer_A.end();
		} else {
			if (ENABLE_Channel2)
				fbo_Mixer_B.end();
		}
	}
#endif

	//fbo_Input_1 update done

	//----

	// this mode do not requires process inside fbo's..
	if (ENABLE_BLEND) {
		//process

#ifdef INCLUDE_BLEND_MODE
		if (ENABLE_BLEND) {
			//TODO:
			//crash workaround...
			bBlendRunning = true;

			//blend base
			psBlend.begin();
		}
#endif
		//----

		if (!swapChannels) {
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
		if (ENABLE_BLEND) {
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

	if (ENABLE_BLEND) {
		//blend target
		fbo_Input_2.begin();
	}

	//--

#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		if (!swapChannels) {
			if (ENABLE_Channel2)
				fbo_Mixer_B.begin();
		} else {
			if (ENABLE_Channel1)
				fbo_Mixer_A.begin();
		}
	}
#endif

	//--

	if (ENABLE_Bg2) {
		ofClear(colorBg2);
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::end_Channel_2() {
	///*drawing happened before this*///

	//ofLogVerbose(__FUNCTION__) << "end_Channel_2";

	if (ENABLE_BLEND) {
		fbo_Input_2.end();
	}

	//--

#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		if (!swapChannels) {
			if (ENABLE_Channel2) fbo_Mixer_B.end();
		} else {
			if (ENABLE_Channel1) fbo_Mixer_A.end();
		}

		//fbo_Mixer_B.end();
	}
#endif
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
	if (ENABLE_BgMix) {
		ofClear(colorBgMix);
	}

	//-

	// draw

	//-

	// 1. blend

#ifdef INCLUDE_BLEND_MODE
	if (ENABLE_BLEND) {
		if (!swapChannels) {
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

#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		ofSetColor(255);

		//raw draw
		mixerGpu.draw(0, 0, ofGetWidth(), ofGetHeight());

		//fliped draw
		//mixerGpu.drawFbo(0, 0, ofGetWidth(), ofGetHeight());
	}
#endif

	//--

	fbo_MixOut.end();
}

//--------------------------------------------------------------
void ofxSurfingMixer::drawMixer() {

	//--

#ifdef INCLUDE_BLEND_MODE
	if (ENABLE_BLEND) {
		fbo_MixOut.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
#endif

#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		ofSetColor(255);
		fbo_MixOut.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingMixer::drawPreviews(float x, float y, float _w) {
	int SELECTED_Channel = mixerGpu.channelSelect.get();

	int numScreens;

	if (SHOW_Backgrounds)
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
	if (SHOW_Backgrounds) //3 monitors
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

	if (SHOW_Backgrounds) myFont.drawString("BACKGROUNDS", xx0 + xLabel, yy0 + h + yLabel + 5);
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
#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
		//backgrounds
		//1st half screen splitted by the 3 backgrounds. seccond half will be for channel 0

		wThird = 0.5f * w / 3.0f;

		ofPushStyle();
		ofFill();

		if (SHOW_Backgrounds) {
			if (ENABLE_Bg1) {
				ofSetColor(colorBg1);
				ofDrawRectangle(ofRectangle { xx0, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG1", xx0 + pad_xScreLb, yy0 + pad_yScreLb);

			if (ENABLE_Bg2) {
				ofSetColor(colorBg2);
				ofDrawRectangle(ofRectangle { xx0 + wThird, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0 + wThird, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG2", xx0 + wThird + pad_xScreLb, yy0 + pad_yScreLb);

			if (ENABLE_BgMix) //independent of swap channels
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
#endif

	//blender
#ifdef INCLUDE_BLEND_MODE
	if (ENABLE_BLEND) {
		//backgrounds

		wThird = w / 3.0f;

		ofPushStyle();
		ofFill();

		if (SHOW_Backgrounds) {
			if (ENABLE_Bg1) {
				ofSetColor(colorBg1);
				ofDrawRectangle(ofRectangle { xx0, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG1", xx0 + pad_xScreLb, yy0 + pad_yScreLb);

			if (ENABLE_Bg2) {
				ofSetColor(colorBg2);
				ofDrawRectangle(ofRectangle { xx0 + wThird, yy0, wThird, h });
			} else {
				drawPreviewsCheckerboard(xx0 + wThird, yy0, wThird, h, sizeBox);
			}
			ofSetColor(255);
			myFontSmall.drawString("BG2", xx0 + wThird + pad_xScreLb, yy0 + pad_yScreLb);

			if (ENABLE_BgMix) //independent of swap channels
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

		if (!swapChannels) {
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
	if (SHOW_Backgrounds)
		yy = yy0 + h + 2.5f * yLabel;
	else
		yy = yy1 + h + 2.5f * yLabel;

	////channels swap
	//std::string str1, str2;
	//if (!swapChannels)
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
	if (ENABLE_BLEND) {
		string str = "";
		str += "BLEND\t\t";
		str += (blendMode.get() < 10 ? " " : "");
		str += ofToString(blendMode.get()); // +"] ";
		str += " ";
		str += ofToUpper(blendName.get());
		//ofDrawBitmapStringHighlight
		myFont.drawString(str, xx0 + xLabel, yy);
	} else if (ENABLE_MIXER) {
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
	drawPreviews(preview_Position.get().x, preview_Position.get().y, window_W / 9.0f);

	//-

	// mixer
#ifdef INCLUDE_MIXER_MODE

	// gui internal
	#ifdef INCLUDE_ofxGui
	gui_Mixer.draw();
	#endif
#endif

	//---

	// help info:
	float w = ofxSurfingHelpers::getWidthBBtextBoxed(myFontHelp, helpInfo);
	float h = ofxSurfingHelpers::getHeightBBtextBoxed(myFontHelp, helpInfo);
	float x = ofGetWidth() * 0.5 - w * 0.5 - w; //displaced
	float y = ofGetHeight() * 0.5 - h * 0.5;
	ofxSurfingHelpers::drawTextBoxed(myFontHelp, helpInfo, x, y);
}

//--------------------------------------------------------------
void ofxSurfingMixer::updateMixer() {

	//--

	// mixer
#ifdef INCLUDE_MIXER_MODE
	if (ENABLE_MIXER) {
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
#endif

	//--

	//blend/mix the 2 channels
	begin_Mix();
	end_Mix();
}

//--------------------------------------------------------------
void ofxSurfingMixer::setup() {
	DISABLE_Callbacks = true;

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

	//build
	//gui internal
	gui_Mixer.setup("MIXER");
	gui_Mixer.add(mixerGpu.getParameterGroup());
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

	DISABLE_Callbacks = false;

	startup();
}

//--------------------------------------------------------------
void ofxSurfingMixer::startup() {
	MODE_Active = true;
	ENABLE_AutoSave = false;

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

	//autosave
	if (ENABLE_AutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave) {
		DISABLE_Callbacks = true;

		//get gui position before save
#ifdef INCLUDE_ofxGui
		Gui_Position = glm::vec2(gui_Control.getPosition());
#endif

		saveParams(params_AppSession, path_GLOBAL + path_Params_AppSession);

		saveParams(params_Preset, path_GLOBAL + path_Params_Preset);

		//saveParams(params_Mixer, path_GLOBAL + path_Params_Mixer);

		timerLast_Autosave = ofGetElapsedTimeMillis();
		if (true) ofLogNotice(__FUNCTION__) << "\t\t\t\t\t\t\t\t\t[AUTOSAVE]";

		DISABLE_Callbacks = false;
	}

	//-

//mixer
#ifdef INCLUDE_MIXER_MODE
	#ifdef INCLUDE_ofxGui
	if (ENABLE_MIXER && SHOW_MIXER && bGuiAdv) {
		//gui internal
		//below the gui
		int xPad = 2;
		int yPad = 0;
		ofRectangle wr = gui_Control.getShape();
		float x = wr.getTopRight().x + xPad;
		float y = wr.getTopRight().y + yPad;
		gui_Mixer.setPosition(x, y);
	}
	#endif
#endif
}

////--------------------------------------------------------------
//void ofxSurfingMixer::draw()
//{
//}

//--------------------------------------------------------------
void ofxSurfingMixer::exit() {
	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";
	ofLogNotice(__FUNCTION__);

	DISABLE_Callbacks = true;

	//-

//get gui position before save
#ifdef INCLUDE_ofxGui
	Gui_Position = glm::vec2(gui_Control.getPosition());
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
	//panel_MIXER->setPosition(preview_Position.get().x + 5, preview_Position.get().y + 165);
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

	//-

	//disabler for all keys. (independent from MODE_Active)
	if (ENABLE_keys_AllMixer) {
		//browse blend modes
		if (false) { }

#ifdef INCLUDE_BLEND_MODE
		//if (!MODE_SHOW_FboFxHelper)//not in fx edit mode to avoid collisions..
		if (true) {
			if (ENABLE_BLEND) {
				if (key == OF_KEY_RIGHT) {
					if (blendMode >= 24) {
						blendMode = 0;
					} else {
						blendMode++;
					}
				} else if (key == OF_KEY_LEFT) {
					if (blendMode <= 0) {
						blendMode = 24;
					} else {
						blendMode--;
					}
				}
			}
		}
#endif

#ifdef INCLUDE_MIXER_MODE
		if (ENABLE_MIXER) {
			//select blend mode
			//if (!MODE_SHOW_FboFxHelper)//not in fx edit mode to avoid collision
			if (true) {
				//-

				//mixer
				if (key == OF_KEY_RIGHT) {
					if (mixerGpu.channelSelect.get() != 0) //except channel 0 background
					{
						if (mixerGpu.getBlendMode() >= mixerGpu.getLastBlendMode()) {
							mixerGpu.setBlendMode(mixerGpu.getLastBlendMode()); //blocked
							//mixerGpu.setBlendMode(1);//cycled. first blendMode
						} else {
							mixerGpu.setBlendMode(mixerGpu.getBlendMode() + 1);
						}
					} else //browse bg hue
					{
						ofColor c = colorBackground.get();
						int bgH = c.getHue();
						bgH += 5;
						if (bgH > 255)
							bgH = 255;
						c.setHue(bgH);
						colorBackground = c;
						mixerGpu.setColorChannel0(colorBackground.get());
					}
				} else if (key == OF_KEY_LEFT) //block channel 0 background
				{
					if (mixerGpu.channelSelect.get() != 0) {
						if (mixerGpu.getBlendMode() <= 1) //first blendMode
						{
							mixerGpu.setBlendMode(1); //blocked
							//mixerGpu.setBlendMode(mixerGpu.getLastBlendMode());//cycled
						} else {
							mixerGpu.setBlendMode(mixerGpu.getBlendMode() - 1);
						}
					} else //browse bg hue
					{
						ofColor c = colorBackground.get();
						int bgH = c.getHue();
						bgH -= 5;
						if (bgH < 0)
							bgH = 0;
						c.setHue(bgH);
						colorBackground = c;
						mixerGpu.setColorChannel0(colorBackground.get());
					}
				}

				//select channel
				else if (key == OF_KEY_DOWN) {
					if (mixerGpu.channelSelect.get() <= 0) {
						mixerGpu.selectChannel(0); //blocked
						//mixerGpu.selectChannel(mixerGpu.getLastChannel());//cycled
					} else {
						mixerGpu.selectChannel(mixerGpu.channelSelect.get() - 1);
					}
				} else if (key == OF_KEY_UP) {
					if (mixerGpu.channelSelect.get() >= mixerGpu.getLastChannel()) {
						mixerGpu.selectChannel(mixerGpu.getLastChannel());
						//mixerGpu.selectChannel(0);//cycle
					} else {
						mixerGpu.selectChannel(mixerGpu.channelSelect.get() + 1);
					}
				}

				//solo
				else if (key == 's') {
					mixerGpu.toggleSolo();
				}

				//reload gui theme
				else if (key == 'j') {
					//loadTheme(path_Theme);
				}
			}
		}
#endif
		//--

		//gui
		if (key == 'g' || key == OF_KEY_F1) {
			SHOW_GuiAll = !SHOW_GuiAll;
			ofLogNotice(__FUNCTION__) << "SHOW_GuiAll:" << SHOW_GuiAll.get();

			if (!SHOW_GuiAll) bGuiAdv = SHOW_GuiAll;
		}
		//---

		//main panels
		////mixer, blender & mask gui
		//if (key == OF_KEY_F1)
		//{
		//	SHOW_MIXER = !SHOW_MIXER;
		//}

		//preview monitors
		else if (key == OF_KEY_F2) {
			SHOW_Preview = !SHOW_Preview;
		}
		//preview monitors
		else if (key == 'p') {
			SHOW_Preview = !SHOW_Preview;
		}

		//swap
		else if (key == '/') // || key == OF_KEY_BACKSPACE)
		{
			swapChannels = !swapChannels;
		}

		//---

		//blend mixer modes browse TAB
		else if (key == key_MODE_AppMixer && !mod_CONTROL) //key TAB
		{
			//disabled app modes
			//int i = MODE_AppMixer - 1;
			//i++;
			//i = i % NUM_MODES_APP;
			//MODE_AppMixer = i + 1;

			////using tabs
			//int i = selectedTab;
			//i++;
			//i = i % NUM_TABS;
			//selectedTab = i;
			//gTabs->setActiveTab(selectedTab);
		}

		//---

		//help
		else if (key == 'h') {
			SHOW_Help = !SHOW_Help;
		}

		////debug
		//else if (key == 'd')
		//{
		//	ENABLE_Debug = !ENABLE_Debug;
		//}

		//---

		//edit fx
		else if (key == key_MODE_AppMixer && mod_CONTROL) {
			MODE_SHOW_FboFxHelper = !MODE_SHOW_FboFxHelper;
		}

		//---
	}

	//--

	// this keys are allways actived. can't be disabled globally, just unsuscribing callback listeners...

	//key enabler (never locked by internal settings)
	if (key == 'k' && mod_CONTROL) {
		ENABLE_keys_AllMixer = !ENABLE_keys_AllMixer;
		ofLogNotice(__FUNCTION__) << "KEYS: " << (ENABLE_keys_AllMixer ? "ON" : "OFF");

		if (!ENABLE_keys_AllMixer) {
			ofLogNotice(__FUNCTION__) << "ALL KEYS DISABLED. PRESS 'k' TO ENABLE GAIN!";
		} else {
			ofLogNotice(__FUNCTION__) << "KEYS ENABLED BACK";
		}
	}

	//-

	if (key == 'G') {
		bGuiAdv = !bGuiAdv;
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

//mouse
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

	MODE_Active = b;
	ENABLE_keys_AllMixer = b;
	//enable/disable all keys bc the main addon where the addons runs could be
	//controlling other things/addons or mode apps..

	//inactive
	if (!b) {
		removeKeysListeners();
		removeMouseListeners();

		//fx
		//MODE_SHOW_FboFxHelper = false;

		//hide presets

		//mixer

		//preview monitors
		//SHOW_Preview = false;

		//gui
		SHOW_GuiAll = false;
	}

	//active
	else {
		addKeysListeners();
		addMouseListeners();

		//gui
		SHOW_GuiAll = true;
	}
}

//--------------------------------------------------------------
void ofxSurfingMixer::setGuiVisible(bool b) {
	SHOW_GuiAll = b;
}

//addon params
//--------------------------------------------------------------
void ofxSurfingMixer::Changed_params_Settings(ofAbstractParameter & e) {
	if (!DISABLE_Callbacks) {
		std::string name = e.getName();

		//exclude debugs
		if (name != "" && name != " ") {
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		//filter
		if (false) {
		}

		//modes
		else if (name == ENABLE_BLEND.getName()) {
			DISABLE_Callbacks = true;
			if (ENABLE_BLEND) {
				ENABLE_MIXER = false;
				MODE_AppMixer = 2;
			}
			DISABLE_Callbacks = false;

			guiRefresh();
		} else if (name == ENABLE_MIXER.getName()) {
			DISABLE_Callbacks = true;
			if (ENABLE_MIXER) {
				ENABLE_BLEND = false;
				MODE_AppMixer = 1;
			}
			DISABLE_Callbacks = false;

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

	#ifdef BLEND_TOGGLES
			DISABLE_Callbacks = true;
			for (int i = 0; i < ENABLE_Blends.size(); i++) {
				ENABLE_Blends[i] = (blendMode.get() == i);
			}
			DISABLE_Callbacks = false;
	#endif
		}
#endif
		//-

		else if (name == "RESET") {
			DISABLE_Callbacks = true;
			if (RESET_Backgrounds) {
				RESET_Backgrounds = false;

				colorBg1 = ofColor(0, 255);
				colorBg2 = ofColor(0, 255);
				colorBgMix = ofColor(0, 255);
				ENABLE_Channel1 = true;
				ENABLE_Channel2 = true;
				ENABLE_Bg1 = true;
				ENABLE_Bg2 = true;
				ENABLE_BgMix = true;
			}
			if (ENABLE_MIXER) {
#ifdef INCLUDE_MIXER_MODE
				mixerGpu.setColorChannel0(ofColor(0, 255));
#endif
			}
			DISABLE_Callbacks = false;
		}

		//-

		else if (name == swapChannels.getName()) {
			if (!swapChannels) {
				swapInfo = "CH1 * CH2";
			} else {
				swapInfo = "CH2 * CH1";
			}
		}

		//mixer
		else if (name == mixerGpu.channelSelect.getName()) {
			guiRefresh();
		}

		//-

		//blend
		//toggle selectors
#ifdef BLEND_TOGGLES
		bool bdo = false;
		for (int i = 0; i < ENABLE_Blends.size(); i++) {
			if (ENABLE_Blends[i].getName() == psBlend.getBlendMode(i)) {
				DISABLE_Callbacks = true;
				blendMode = i;
				bdo = true;
				DISABLE_Callbacks = false;
			}
		}
		if (bdo) {
			DISABLE_Callbacks = true;
			for (int i = 0; i < ENABLE_Blends.size(); i++) {
				ENABLE_Blends[i] = (blendMode.get() == i);
			}
			DISABLE_Callbacks = false;
		}
#endif
	}
}

//addon internal params
//--------------------------------------------------------------
void ofxSurfingMixer::Changed_params_AppSession(ofAbstractParameter & e) {
	if (!DISABLE_Callbacks) {
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
			gui_Control.setPosition(Gui_Position.get().x, Gui_Position.get().y);
		}
#endif
		else if (name == "ACTIVE") {
			setActive(MODE_Active);
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

		else if (name == SHOW_Preview.getName()) {
			//gUser->getControl(SHOW_Backgrounds.getName())->setEnabled(SHOW_Preview.get());
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
	//if (!ENABLE_AutoSave && !DISABLE_Callbacks)//reduce double log when autosave happens
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
		if (!ENABLE_BLEND && !ENABLE_MIXER) {
			MODE_AppMixer = 2;
			ENABLE_MIXER = true;
		}

		//----

		//internal ofxGui
#ifdef INCLUDE_MIXER_MODE
	#ifdef INCLUDE_ofxGui
		if (ENABLE_MIXER && SHOW_MIXER && bGuiAdv) {
			auto & gMixer = gui_Mixer.getGroup(params_Mixer.getName());

			auto & gPrv = gMixer.getGroup("PREVIEW");
			auto & gCh0 = gMixer.getGroup("BACKGROUND");
			auto & gCh1 = gMixer.getGroup("CHANNEL 1");
			auto & gCh1Tint = gCh1.getGroup("TINT");
			auto & gCh2 = gMixer.getGroup("CHANNEL 2");
			auto & gCh2Tint = gCh2.getGroup("TINT");

			gPrv.minimize();
			gCh0.minimize();
			gCh1.minimize();
			gCh1Tint.minimize();
			gCh2.minimize();
			gCh2Tint.minimize();

			//-

			gPrv.maximize();

			switch (mixerGpu.channelSelect.get()) {
			case 0:
				gCh0.maximize();
				break;
			case 1:
				gCh1.maximize();
				break;
			case 2:
				gCh2.maximize();
				break;
			default:
				break;
			}
		}
	#endif

		//--

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
		if (ENABLE_BLEND) {
			gBlend.maximize();
		#ifdef INCLUDE_MIXER_MODE
					//gMixer.minimize();
		#endif
		} else if (ENABLE_MIXER) {
			gBlend.minimize();
		#ifdef INCLUDE_MIXER_MODE
					//gMixer.maximize();
		#endif
		}
	#endif
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
