# Architecture Overview — `ofxSurfingMixer`

## 1) Project purpose

This repository implements an openFrameworks addon called `ofxSurfingMixer`, focused on mixing 2 real-time render channels with two main modes:

- `MODE BLEND`: composition of `Channel 1` + `Channel 2` using `ofxPSBlend`.
- `MODE MIXER`: GPU channel mixing using `ofxGpuMixer`.

It also includes a sample app (`src/ofApp.*`) that shows how to integrate the addon into the standard openFrameworks lifecycle.

---

## 2) Main structure

### Sample app

- `src/main.cpp`
  - Creates a `1920x1080` window.
  - Starts `ofApp`.

- `src/ofApp.h` / `src/ofApp.cpp`
  - Holds instances of:
    - `ofxSurfingMixer mixer`
    - `ofxSceneTEST scene`
  - Per-frame flow:
    1. Draws scene into `Channel 1` (`begin_CH1` / `end_CH1`).
    2. Draws scene into `Channel 2` (`begin_CH2` / `end_CH2`).
    3. Calls `mixer.update()`.
    4. Draws output via `mixer.draw()` and UI via `mixer.drawGui()`.

### Addon

- `src/ofxSurfingMixer.h` / `src/ofxSurfingMixer.cpp`
  - Core class: `ofxSurfingMixer : public ofBaseApp`.
  - Responsibilities:
    - Input/output FBO management.
    - Shader-based blending (`ofxPSBlend`).
    - Channel-based GPU mixing (`ofxGpuMixer`).
    - Parameters (`ofParameterGroup`) + XML persistence.
    - UI (`ofxGui`) and previews.
    - Per-parameter listeners using `ofEventListeners`.

---

## 3) Render pipeline

### Key FBOs

- `fbo_Input_1`, `fbo_Input_2`: per-channel input buffers.
- `fbo_Mixer_A`, `fbo_Mixer_B`: intermediate buffers for mixer mode.
- `fbo_MixOut`: final composited output.

### Per-frame flow in `ofxSurfingMixer`

1. **Per-channel capture**
   - `begin_CH1` / `end_CH1`
   - `begin_CH2` / `end_CH2`
   - If `bEnableFx` is disabled, it bypasses directly to `begin/end_Channel_X`.

2. **Composition**
   - `updateMixer()`:
     - Updates `mixerGpu` when mixer mode is active.
     - Runs `begin_Mix()` + `end_Mix()` to produce `fbo_MixOut`.

3. **Output**
   - `draw()` draws `fbo_MixOut` to screen.
   - `drawGui()` draws panels and optional previews.

---

## 4) Operating modes

### A) Blend mode

- Enabled by `bEnableModeBlend`.
- Uses `ofxPSBlend` for base and blend operation.
- The second channel is applied over the first using `blendMode`.
- `blendName` is derived from the current `psBlend` mode.

### B) Mixer mode

- Enabled by `bEnableModeMixer`.
- Uses `ofxGpuMixer::Mixer` with 3 channels:
  - Channel 0: background color (`colorBackground`)
  - Channel 1: `fbo_Mixer_A`
  - Channel 2: `fbo_Mixer_B`
- Supports channel selection and per-channel blend mode from `mixerGpu` parameters.

### Mode exclusivity

- Listeners enforce mutual exclusivity:
  - If blend is enabled, mixer is disabled.
  - If mixer is enabled, blend is disabled.
- If both are disabled, one mode is forced by workflow to avoid an invalid state.

---

## 5) Parameter system and persistence

### Main groups

- `params_Internal`
  - Internal state and app session data (active, GUI, autosave, etc.).
- `params_UserGui`
  - User-facing operational toggles.
- `params_Preset`
  - Mode/control/background/mixer setup for presets.

### Save/load

- Base folder configurable via `setPathGlobal()`.
- Files:
  - `AppSession_Settings.xml`
  - `Preset_Settings.xml`
- Methods:
  - `loadParams(ofParameterGroup&, path)`
  - `saveParams(ofParameterGroup&, path)`

### Autosave

- Controlled by `bEnableAutosave` and `timeToAutosave`.
- On each autosave interval, it writes `params_AppSession` and `params_Preset`.

---

## 6) Callbacks and events

The project uses the recommended per-parameter listener pattern (no name-based filtering):

- `ofEventListeners parameterListeners` keeps listeners alive.
- Central registration in `setupCallbacks()` with individual lambdas for:
  - `bEnableModeBlend`, `bEnableModeMixer`
  - `blendMode`
  - `bResetBackgrounds`
  - `bSwapChannels`
  - `positionGui`
  - `bModeActive`
  - `bEnableChannel1`, `bEnableChannel2`

This avoids the global callback approach using `if (name == ...)`, reducing coupling and crash risk from ambiguous routing.

---

## 7) UI and UX

- `ofxGui` (if `INCLUDE_ofxGui`): panel bound to `params_Preset`.
- Previews (`drawPreviews`) show:
  - backgrounds (`BG1`, `BG2`, `BGMIX`),
  - channels 1/2,
  - mixer selected-channel state,
  - text overlays for mode and blend.
- Checkerboard is drawn when channels/backgrounds are disabled.

---

## 8) External dependencies

- openFrameworks core (`ofMain`, `ofFbo`, `ofParameter`, `ofXml`, events).
- `ofxGui` (UI).
- `ofxPSBlend` (blend mode).
- `ofxGpuMixer` (shader/channel mixer mode).
- `ofxSurfingHelpers` (folder and utility helpers).
- `ofxSceneTEST` (sample integration only).

---

## 9) `ofxSurfingMixer` lifecycle

1. Constructor:
   - configures paths,
   - enables input listeners,
   - loads fonts.
2. `setup()`:
   - reads window size,
   - allocates FBOs,
   - initializes parameters,
   - initializes GUI,
   - calls `setupCallbacks()`,
   - calls `startup()` (XML load + refresh).
3. `update()`:
   - runs `updateEngine()` + `updateMixer()`.
4. `draw` (from host app):
   - runs `draw()` + `drawGui()`.
5. `exit()` / destructor:
   - persists state and removes input listeners.

---

## 10) Recommended extension points

- Add a new `Mode` (e.g., global post-fx) reusing `fbo_MixOut`.
- Extract FBO clearing into a private helper to remove duplication.
- Split preview rendering into a dedicated UI module.
- Add a manual test checklist for resize, swap, reset, and autosave.

---

## 11) Conventions observed in this repo

- Real-time, frame-based design.
- Heavy use of `ofParameter` for state/UI/persistence.
- Callback guards (`bDisableCallbacks`) to prevent recursion.
- Preference for individual lambda listeners (`ofEventListener`) over global name-filtered callbacks.
