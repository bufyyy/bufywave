<h1 align="center">bufywave</h1>

<p align="center">
  A lightweight digital synthesizer VST3 plugin built with the <a href="https://juce.com/">JUCE</a> framework.<br>
  8-voice polyphony, three classic oscillator waveforms, an attack/release envelope, and a live oscilloscope.
</p>

<p align="center">
  <a href="https://github.com/bufyyy/bufywave/releases/latest/download/bufywave-windows.zip">
    <img src="https://img.shields.io/badge/⬇%20Download-bufywave%20(Windows%20VST3)-2ea043?style=for-the-badge" alt="Download bufywave">
  </a>
  <a href="https://github.com/bufyyy/bufywave/releases/latest">
    <img src="https://img.shields.io/github/v/release/bufyyy/bufywave?style=for-the-badge&label=latest" alt="Latest release">
  </a>
</p>

---

## ⬇️ Download

**[Click here to download the latest version (Windows VST3).](https://github.com/bufyyy/bufywave/releases/latest/download/bufywave-windows.zip)**

The download is a `.zip` containing:

- `bufywave.vst3` — the plugin, for use inside a DAW (FL Studio, Ableton, Reaper, …).
- `bufywave.exe` — a standalone app, if you just want to play it without a DAW.

> No release yet? Builds are produced automatically by GitHub Actions. See
> [Publishing a release](#-publishing-a-release-for-maintainers) below if the link 404s.

## ✨ Features

- **8-voice polyphony** — play chords and overlapping notes without dropouts.
- **Three oscillator waveforms** — Sine, Square, and Saw, switchable in real time.
- **Amplitude envelope** — adjustable Attack (0.01–2.0 s) and Release (0.01–3.0 s).
- **Live oscilloscope** — visualizes the output waveform at 30 fps.
- **MIDI input** — responds to all notes on all channels.
- **State persistence** — your settings are saved and restored with the DAW project.

## 🎛️ Controls

| Control     | Range / Options     | Default | Description                                  |
|-------------|---------------------|---------|----------------------------------------------|
| Oscillator  | Sine / Square / Saw | Sine    | Waveform used by every voice.                |
| Attack      | 0.01 – 2.0 s        | 0.1 s   | Time for the note to fade in after key-down. |
| Release     | 0.01 – 3.0 s        | 0.1 s   | Time for the note to fade out after key-up.  |

> The envelope currently uses full sustain with no decay stage, so it behaves as an
> attack/release (AR) envelope.

## 🔌 Installation

First, [download](#️-download) and unzip the package.

### Install the VST3

Copy **`bufywave.vst3`** into your system VST3 folder:

| OS      | VST3 folder                                     |
|---------|-------------------------------------------------|
| Windows | `C:\Program Files\Common Files\VST3`            |
| macOS   | `~/Library/Audio/Plug-Ins/VST3`                 |

Then follow the guide for your DAW below. (After copying, restart your DAW or trigger a plugin rescan.)

<details>
<summary><b>FL Studio</b></summary>

1. Copy `bufywave.vst3` to `C:\Program Files\Common Files\VST3`.
2. Open FL Studio → **Options ▸ Manage plugins**.
3. Make sure the VST3 folder above is listed under **Plugin search paths** (add it if not).
4. Click **Find more plugins** (top-left) to scan. `bufywave` appears under **New** when found.
5. In the plugin list, find **bufywave**, then drag it into the **Channel Rack** (or right-click ▸ Add).
6. Route a MIDI/pattern to it and play. 🎹
</details>

<details>
<summary><b>Ableton Live</b></summary>

1. Copy `bufywave.vst3` to `C:\Program Files\Common Files\VST3` (Windows) or `~/Library/Audio/Plug-Ins/VST3` (macOS).
2. Open Live → **Options/Preferences ▸ Plug-Ins**.
3. Turn **Use VST3 Plug-In System Folders** **On** (or add the folder under *VST3 Plug-In Custom Folder*).
4. Click **Rescan**.
5. In the browser, go to **Plug-Ins ▸ bufywave** and drag it onto a MIDI track.
6. Arm the track and play. 🎹
</details>

<details>
<summary><b>Reaper</b></summary>

1. Copy `bufywave.vst3` to your VST3 folder (see table above).
2. Open Reaper → **Options ▸ Preferences ▸ Plug-ins ▸ VST**.
3. Confirm the VST3 path is listed, then click **Re-scan**.
4. Add a track, click the **FX** button, search for **bufywave**, and add it.
5. Record-arm the track and play. 🎹
</details>

<details>
<summary><b>Other VST3 hosts (Studio One, Cubase, Bitwig, Cakewalk, …)</b></summary>

1. Copy `bufywave.vst3` to the standard VST3 folder for your OS (see table above).
2. Open your DAW's plugin manager and **rescan** plugins.
3. Add **bufywave** to an instrument/MIDI track and play. 🎹
</details>

### Or just run the standalone

Prefer not to use a DAW? Double-click **`bufywave.exe`**, pick your audio/MIDI device under
**Options**, and play with a connected MIDI keyboard.

## 🛠️ Building from source

### Option A — CMake (recommended, no setup)

JUCE is downloaded automatically, so a clean clone builds out of the box.

```sh
git clone https://github.com/bufyyy/bufywave.git
cd bufywave
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The plugin is written to `build/bufywave_artefacts/Release/VST3/bufywave.vst3`
(and a standalone app under `.../Standalone/`).

> Already have JUCE locally? Skip the download with `-DJUCE_PATH=/path/to/JUCE`.

### Option B — Projucer

1. Open `bufywave.jucer` in the **Projucer**.
2. Confirm the JUCE module paths (the project expects JUCE at `../../../JUCE/modules`).
3. Save the project and click **Open in IDE**, then build in Visual Studio 2022.

## 🚀 Publishing a release (for maintainers)

The [download link](#️-download) is served from GitHub Releases, populated by CI:

1. Push a version tag:
   ```sh
   git tag v1.0.0
   git push origin v1.0.0
   ```
2. The **Build & Release** workflow (`.github/workflows/release.yml`) builds the Windows VST3 +
   standalone, zips them as `bufywave-windows.zip`, and attaches it to a new release.
3. The "Download" button then points at that asset automatically.

Every push also uploads the same zip as a build **artifact** on the Actions run, so you can grab a
build even without tagging a release.

## 📁 Project structure

```
bufywave/
├── CMakeLists.txt              # CMake build (downloads JUCE automatically)
├── bufywave.jucer              # Projucer project
├── Source/
│   ├── PluginProcessor.h/.cpp  # Audio processor, parameter layout, MIDI/synth wiring
│   ├── PluginEditor.h/.cpp     # GUI (controls + oscilloscope)
│   ├── SynthVoice.h            # A single voice: oscillator + ADSR rendering
│   └── SynthSound.h            # Which notes/channels the synth responds to
└── .github/workflows/
    └── release.yml             # CI: build on Windows + publish releases
```

## 📝 License

No license has been specified yet. If you intend to share or reuse the code, add a `LICENSE` file.
Note that JUCE itself ships under its own [licensing terms](https://juce.com/get-juce/).
