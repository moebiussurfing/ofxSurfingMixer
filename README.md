ofxSurfingMixer
-------------------------

# Overview
**ofxSurfingMixer** is an **openFrameworks** addon template for MYSELF.

## Screenshots
![image](/readme_images/Capture5.JPG?raw=true "image")
![image](/readme_images/Capture1.JPG?raw=true "image")
![image](/readme_images/Capture2.JPG?raw=true "image")
![image](/readme_images/Capture3.JPG?raw=true "image")
![image](/readme_images/Capture4.JPG?raw=true "image")

## Features
- 

## Usage
 
### ofApp.h
```.cpp
#include "ofxSurfingMixer.h"
ofxSurfingMixer myAddon;
```

### ofApp.cpp
```.cpp
ofApp::setup(){
	ofxSurfingMixer.setup();
}

ofApp::update(){
	ofxSurfingMixer.update();
}

ofApp::draw(){
	ofxSurfingMixer.draw();
	ofxSurfingMixer.drawGui();
}
```

## Dependencies
- 

## Tested systems
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS High Sierra** / **Xcode 9/10** / **OF ~0.11**

### TODO
* 

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*