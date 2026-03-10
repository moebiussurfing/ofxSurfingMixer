#pragma once

class ofApp;

class ofAppGuiViews {
public:
	void setApp(ofApp* app_);

	void drawImGui();
	void drawImGuiApp();
	void drawImGuiSpecialWindows();
	void drawImGuiSpecialWindow0();
	void drawImGuiSpecialWindow1();

private:
	ofApp* app = nullptr;
};
