  # Game Engine

A cross-platform game engine with editor, Lua scripting, Box2D physics, OpenAL audio, and OpenGL rendering — targeting Windows, Linux, Android, and the Web.

## Table of Contents

- [Requirements](#requirements)
- [Build Configurations](#build-configurations)
- [Command Line Projects](#command-line-projects)
- [Windows](#windows)
- [Linux](#linux)
- [Android](#android)
- [Web (Emscripten)](#web-emscripten)
- [Text Rendering](#text-rendering)
- [Asset Packer](#asset-packer)
- [Output Locations](#output-locations)

---

## Requirements

| Platform | Tooling |
|----------|---------|
| Windows  | Visual Studio 2022 (or Ninja + Dev Command Prompt), CMake ≥ 3.20 |
| Linux    | GCC/Clang, CMake ≥ 3.20, pkg-config |
| Android  | Android SDK, NDK, Gradle |
| Web      | Emscripten SDK (`emcc`, `emcmake`) |

---

## Build Configurations

This project supports five CMake build types:

- `Debug`
- `Release`
- `RelWithDebInfo`
- `MinSizeRel`
- `Distributed` — fully optimized, LTO-enabled, stripped release build intended for shipping

---

## Build Presets

Every project has its own `CMakeLists.txt`, so you can configure and build just
the piece you are working on. The presets select which projects are enabled:

| Preset | Targets | Output |
|--------|---------|--------|
| `runtime` | Sandbox | `build/runtime/bin/Sandbox` |
| `editor` | AeroEditor | `build/editor/bin/AeroEditor` |
| `editor-runtime` | AeroEditor + Sandbox | `build/editor-runtime/bin/` |
| `server` | ServerSandbox | `build/server/bin/ServerSandbox` |
| `tools` | AssetPacker | `build/tools/bin/AssetPacker` |
| `full` | all of the above | `build/full/bin/` |
| `web` | Sandbox (Emscripten) | `build/web/Sandbox.html` |

```bash
# Configure + build a preset
./build.sh runtime
./build.sh editor --clean

# Configure once, then build individual targets
cmake --preset full
cmake --build --preset full --target Sandbox
cmake --build --preset full --target AeroEditor
cmake --build --preset full --target AssetPacker

# Build several targets with the helper script
./build.sh full --target Sandbox --target AeroEditor
```

Because all targets share one build tree, switching targets does not
reconfigure or rebuild the other projects.

## Command Line Projects

Both `AeroEditor` and `Sandbox` accept a project as their first positional
argument. The argument can be either the folder containing the `.aeroproj` file
or the `.aeroproj` file itself:

```bash
./build/full/bin/AeroEditor /path/to/MyGame
./build/full/bin/AeroEditor /path/to/MyGame/MyGame.aeroproj
./build/full/bin/AeroEditor --project /path/to/MyGame
./build/full/bin/AeroEditor --project=/path/to/MyGame/MyGame.aeroproj

./build/full/bin/Sandbox /path/to/MyGame
./build/full/bin/Sandbox /path/to/MyGame/MyGame.aeroproj
```

`AeroEditor` opens the project directly when a valid argument is supplied.
With no argument (or an invalid one) it opens the project manager. Project
Manager launches and the editor's **Project → Open Project...** /
**Open Project Folder...** actions restart the editor with the selected path as
an argument. **Run Default Scene** and **Run Current Scene** pass the active
`.aeroproj` file to `Sandbox` the same way.

The exported-game path is still supported: if `Sandbox` is started without an
argument, it reads `settings.json` from `game.pak` (or the app-data folder in
development builds).

```
CMakeLists.txt                # options, module includes, orchestration
cmake/Platform.cmake          # platform detection, flags, output directories
cmake/Dependencies.cmake      # GLFW, OpenAL, Lua, Box2D, ImGui, ...
cmake/Packaging.cmake         # install rules + CPack
cmake/Summary.cmake           # configuration summary
Application/CMakeLists.txt    # ApplicationEditor / Runtime / Server libraries
Sandbox/CMakeLists.txt        # Sandbox (desktop, Web, Android)
AeroEditor/CMakeLists.txt     # AeroEditor (desktop only)
ServerSandbox/CMakeLists.txt  # ServerSandbox (desktop only)
tools/AssetPacker/CMakeLists.txt
```

---

## Windows

**Using Visual Studio 2022:**

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Distributed
cmake --build build --config Debug
```

**Using Ninja (from a Developer Command Prompt for VS 2022):**

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Distributed
cmake --build build
```

**Packaging (NSIS installer):**

```bash
cd build
cpack -C Distributed
```

---

## Linux

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Distributed
cmake --build build
```

**Packaging (DEB/RPM/TGZ, depending on available tools):**

```bash
cd build
cpack
```

---

## Android

```bash
cd android
gradle wrapper

./gradlew assembleDebug     # Debug APK
./gradlew assembleRelease   # Release APK
```

**Install to a connected device/emulator:**

```bash
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

The Android host uses a black fullscreen theme and `FLAG_SECURE`, and the
native runtime clears the EGL surface to black whenever the app is paused or
its surface is destroyed. Recents/app-switcher previews therefore show a dark
screen instead of a stale gameplay frame.

Lua scripts on Android can use the same `Keyboard`, `Mouse`, `Draw`, and `Log`
APIs as desktop builds, plus:

- `Platform.name()`, `Platform.is_android()`, `Platform.is_mobile()`,
  `Platform.is_active()`, `Platform.is_paused()`, `Platform.get_screen_size()`
- `Touch.count()`, `Touch.is_down(index)`, `Touch.get_id(index)`,
  `Touch.get_position(index)`, `Touch.get_viewport_position(index)`,
  `Touch.get_world_position(index)`, `Touch.get_pinch_distance()`,
  `Touch.is_pinching()`
- `on_pause()` / `on_resume()` hooks in both global scripts and script nodes

`Mouse.is_button_down(Button.Left)` still tracks the primary touch for
compatibility with existing scripts.

---

## Web (Emscripten)

Install the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
and make sure `emcc`/`emcmake` are on your `PATH`. Only the Sandbox runtime is
built for the web; the editor, server, and tools are skipped.

```bash
# Using the build script (web preset, output in build/web)
./build.sh web

# Or configure manually (output in build-web)
emcmake cmake -B build-web -S .
cmake --build build-web
```

The output consists of `Sandbox.html`, `Sandbox.js`, `Sandbox.wasm`, plus two
data files copied next to the HTML:

| File | Contents |
|------|----------|
| `assets.pak` | Engine assets (shaders, fonts, default textures, …) |
| `game.pak` | Packed project (`settings.json` + project files) |

The paks are **fetched at runtime** instead of being embedded in a
`Sandbox.data` file. To iterate on game content, replace `game.pak` in the
served directory and refresh the page — no relink or rebuild is required.
`assets.pak` can be replaced the same way. (If no `assets.pak` exists at
configure time, the build falls back to preloading the loose
`Application/assets` folder and produces a `Sandbox.data` file instead.)

**Serve locally** (browsers cannot load `.wasm`/paks from `file://`):

```bash
cd build/web      # or build-web
python3 -m http.server 8000
```

Then open `http://localhost:8000/Sandbox.html` in a browser.

---

## Text Rendering

Text is rendered from an MSDF font atlas (`Application/assets/textures/atlas.png`
+ `atlas.json`). Two paths are available:

| Mode | Description |
|------|-------------|
| `Combined` (default) | The original path. Glyphs are appended to the same instanced batch as rectangles, circles, and sprites and drawn through the shared quad shader. |
| `Independent` | Glyphs are collected in their own batch and drawn in a dedicated MSDF pass (`Text.glsl` / `TextES.glsl`) after the shape batch, using `fwidth`-based screen-space antialiasing and half-texel UV insets. |

The default stays on `Combined`; at rotation 0 it remains visually compatible
with the original text path. Switch per run without recompiling:

```bash
AERO_TEXT_RENDER=independent ./build/full/bin/Sandbox   # or: new, 1
AERO_TEXT_RENDER=combined    ./build/full/bin/Sandbox   # default
```

Or from code:

```cpp
ag::Renderer2D::set_text_render_mode(ag::Renderer2D::TextRenderMode::Independent);
```

Because the dedicated pass runs after the shape batch of a flush, `Independent`
draws text above shapes queued in the same batch. Use `Combined` if a shape must
be able to cover text in the same frame, or flush between them.

The atlas is loaded with `Filter_Mode::AG_LINEAR_NOMIPMAP`: mipmapping an MSDF
atlas averages its distance channels and destroys glyph edges, so both modes
benefit from that change.

---

## Asset Packer

`AssetPacker` creates the `.pak` archives read by the runtime. It is built
alongside the desktop targets at `build/<preset>/bin/AssetPacker` and is
also used automatically to pack `Application/assets` into `assets.pak`.

```bash
AssetPacker [options] <input_folder> <output.pak>

# Example: pack engine assets
build/full/bin/AssetPacker Application/assets build/full/assets.pak
```

| Option | Description |
|--------|-------------|
| `-h`, `--help` | Show usage |
| `-q`, `--quiet` | Hide the per-file listing |
| `-v`, `--verbose` | List every packed file (default) |
| `--include-hidden` | Include files/folders whose names start with `.` |

The packer writes paths relative to the input folder, sorts them so the same
input always produces the same pak, skips hidden entries by default, never
packs its own output file, and replaces the destination atomically after a
successful write.

---

## Output Locations

| Platform | Artifact |
|----------|----------|
| Windows / Linux | `build/<preset>/bin/` (executables: `Sandbox`, `AeroEditor`, `ServerSandbox`, `AssetPacker`). Multi-config generators add a config folder: `build/<preset>/<Config>/bin/`. |
| Android | `android/app/build/outputs/apk/debug/app-debug.apk`<br>`android/app/build/outputs/apk/release/app-release.apk` |
| Web | `build/web/` (preset) or `build-web/`: `Sandbox.html`, `*.js`, `*.wasm`, `assets.pak`, `game.pak` |

Installers are produced with CPack from the build tree (`cd build/<preset> && cpack`):
`.zip`/`.exe` (NSIS) on Windows, `.deb`/`.rpm`/`.tar.gz` on Linux.

---

## License

Add your license here.
