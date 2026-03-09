ofxSurfingMixer
-------------------------

# Overview
**ofxSurfingMixer** is an **openFrameworks** addon for GPU mixing 2 video channels y realtime.

## Screenshots
[]("Capture.PNG")

## Features
- Bundles `ofxPSBlend` and `ofxGpuMixer` features.

## Usage

### ofApp.h
```.cpp
#include "ofxSurfingMixer.h"
ofxSurfingMixer mixer;
```

### ofApp.cpp
```.cpp
void ofApp::update() {
	// channel 1
	mixer.begin_CH1();
	{
		scene.drawChannel1();
	}
	mixer.end_CH1();

	// channel 2
	mixer.begin_CH2();
	{
		scene.drawChannel2();
	}
	mixer.end_CH2();

	mixer.update();
}

void ofApp::draw() {
	// Mixer OUT
	mixer.draw();

	// Gui
	mixer.drawGui();
}
```

## Dependencies
- ofxSurfingMixer
- ofxGui
- ofxSceneTEST -> FOR TESTING ONLY

## Tested systems
- **Windows10** / **VS2026** / **OF 0.12.1**

## Author
Addon by **@moebiusSurfing**
*(ManuMolina). 2020.*

## License
*MIT License.*
