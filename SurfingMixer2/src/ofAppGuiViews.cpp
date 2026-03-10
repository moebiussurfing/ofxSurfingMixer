#include "ofAppGuiViews.h"
#include "ofApp.h"

//--------------------------------------------------------------
void ofAppGuiViews::setApp(ofApp * app_) {
	app = app_;
}

//--------------------------------------------------------------
void ofAppGuiViews::drawImGui() {
	if (app == nullptr) return;

	// Gui Manager with Docking features:
	// In between here (Begin/End)
	// we can render ImGui windows and widgets,
	// and all the docking magic.

	//--

	app->ui.Begin();
	{
//--

// 1. Docking magic
#if 1
		app->ui.BeginDocking();
		{
		}
		app->ui.EndDocking();
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
	app->ui.End();
}

//----

//--------------------------------------------------------------
void ofAppGuiViews::drawImGuiSpecialWindows() {
	if (app == nullptr) return;

	drawImGuiWindow0();

	//--

	drawImGuiWindow1();
}

//--------------------------------------------------------------
void ofAppGuiViews::drawImGuiApp() {
	if (app == nullptr) return;

	// for all the queued especial windows in setup()!
	if (app->bGui)
		IMGUI_SUGAR__WINDOWS_CONSTRAINTS;
	if (app->ui.BeginWindow(app->bGui)) {
		app->ui.Add(app->mixer.bGui, OFX_IM_TOGGLE_BIG);
		app->ui.AddSpacingSeparated();
		app->ui.Add(app->w.params, SurfingGuiGroupStyle_Collapsed);

		app->ui.EndWindow();
	}
}

//--------------------------------------------------------------
void ofAppGuiViews::drawImGuiWindow0() {
	if (app == nullptr) return;

	if (app->mixer.bGui)
		IMGUI_SUGAR__WINDOWS_CONSTRAINTS;
	if (app->ui.BeginWindow(app->mixer.bGui)) {
		app->ui.AddLabelHuge("Mixer", false);

		app->ui.AddGroup(app->mixer.getParamsPreset(), SurfingGuiGroupStyle_Collapsed);

		//--

		app->ui.EndWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofAppGuiViews::drawImGuiWindow1() {
	//if (app == nullptr) return;

	//if (app->ui.BeginWindowSpecial(1)) {
	//	app->ui.AddLabelHuge("Window 1", false);
	//	//app->ui.AddGroup(app->mixer.getParamsPreset());

	//	//--

	//	app->ui.EndWindowSpecial();
	//}
}

//----
