  # Game Engine

A cross-platform game engine with editor, Lua scripting, Box2D physics, OpenAL audio, and OpenGL rendering — targeting Windows, Linux, Android, and the Web.

## Table of Contents

- [Requirements](#requirements)
- [Build Configurations](#build-configurations)
- [Windows](#windows)
- [Linux](#linux)
- [Android](#android)
- [Web (Emscripten)](#web-emscripten)
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

---

## Web (Emscripten)

```bash
emcmake cmake -B build-web -S .
cmake --build build-web
```

**Serve locally:**

```bash
cd build-web
python -m http.server 8000
```

Then open `http://localhost:8000/YourGame.html` in a browser.

---

## Output Locations

| Platform | Artifact |
|----------|----------|
| Windows / Linux | `build/bin/<config>/` (executables), `build/*.zip` \| `.deb` \| `.rpm` \| `.exe` (installers via CPack) |
| Android | `android/app/build/outputs/apk/debug/app-debug.apk`<br>`android/app/build/outputs/apk/release/app-release.apk` |
| Web | `build-web/*.html`, `*.js`, `*.wasm`, `*.data` |

---

## License

Add your license here.
