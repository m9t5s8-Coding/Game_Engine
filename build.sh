#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
RESET='\033[0m'

show_help() {
  echo -e "${BLUE}========================================${RESET}"
  echo -e "${GREEN}Game Engine Build Script${RESET}"
  echo -e "${BLUE}========================================${RESET}"
  echo ""
  echo "Usage: ./build.sh [preset] [options]"
  echo ""
  echo "Presets:"
  echo "  runtime         - Build only Sandbox"
  echo "  editor          - Build only AeroEditor"
  echo "  editor-runtime  - Build AeroEditor + Sandbox"
  echo "  server          - Build only ServerSandbox"
  echo "  tools           - Build only AssetPacker"
  echo "  full            - Build everything"
  echo "  web             - Build for Web/Emscripten"
  echo ""
  echo "Options:"
  echo "  --target <name> - Build only this target (repeatable)"
  echo "  --clean         - Clean build directory before building"
  echo "  --configure     - Only configure, don't build"
  echo "  --build         - Only build (after configuration)"
  echo "  --verbose       - Verbose output"
  echo "  --help          - Show this help"
  echo ""
  echo "Examples:"
  echo "  ./build.sh runtime"
  echo "  ./build.sh full --target AssetPacker"
  echo "  ./build.sh editor --clean"
  echo "  ./build.sh full --verbose"
  echo ""
}

# Check if CMake is available
if ! command -v cmake &>/dev/null; then
  echo -e "${RED}Error: CMake not found in PATH${RESET}"
  echo "Please install CMake and add it to your PATH"
  exit 1
fi

# Check if Ninja is available (optional)
if ! command -v ninja &>/dev/null; then
  echo -e "${YELLOW}Warning: Ninja not found. Using default generator.${RESET}"
fi

# Show help if no arguments
if [ $# -eq 0 ]; then
  show_help
  exit 0
fi

# Parse arguments
PRESET="$1"
CLEAN=0
CONFIGURE_ONLY=0
BUILD_ONLY=0
VERBOSE=0
TARGETS=()
shift

while [ $# -gt 0 ]; do
  case "$1" in
  --clean)
    CLEAN=1
    ;;
  --configure)
    CONFIGURE_ONLY=1
    ;;
  --build)
    BUILD_ONLY=1
    ;;
  --verbose)
    VERBOSE=1
    ;;
  --target)
    if [ $# -lt 2 ] || [ -z "$2" ]; then
      echo -e "${RED}Error: --target requires a target name${RESET}"
      exit 1
    fi
    TARGETS+=("$2")
    shift
    ;;
  --help)
    show_help
    exit 0
    ;;
  *)
    echo -e "${RED}Unknown option: $1${RESET}"
    exit 1
    ;;
  esac
  shift
done

# Validate preset
VALID_PRESETS=("runtime" "editor" "editor-runtime" "server" "tools" "full" "web")
IS_VALID=0
for p in "${VALID_PRESETS[@]}"; do
  if [ "$p" = "$PRESET" ]; then
    IS_VALID=1
    break
  fi
done

if [ $IS_VALID -eq 0 ]; then
  echo -e "${RED}Error: Unknown preset '$PRESET'${RESET}"
  echo "Available presets: ${VALID_PRESETS[*]}"
  exit 1
fi

# Check for Emscripten if building web
if [ "$PRESET" = "web" ]; then
  if ! command -v emcmake &>/dev/null; then
    echo -e "${RED}Error: emcmake not found. Please install Emscripten.${RESET}"
    echo "Set EMSDK environment variable or add Emscripten to PATH"
    exit 1
  fi
fi

# Set build directory
BUILD_DIR="build/$PRESET"

# Clean if requested
if [ $CLEAN -eq 1 ]; then
  echo -e "${YELLOW}Cleaning build directory: $BUILD_DIR${RESET}"
  if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}Cleaned successfully${RESET}"
  else
    echo -e "${YELLOW}Build directory doesn't exist, nothing to clean${RESET}"
  fi
fi

# Configure
if [ $BUILD_ONLY -eq 0 ]; then
  echo -e "${BLUE}Configuring preset: $PRESET${RESET}"

  if [ "$PRESET" = "web" ]; then
    if ! emcmake cmake --preset "$PRESET"; then
      echo -e "${RED}Configuration failed!${RESET}"
      exit 1
    fi
  else
    if ! cmake --preset "$PRESET"; then
      echo -e "${RED}Configuration failed!${RESET}"
      exit 1
    fi
  fi
  echo -e "${GREEN}Configuration successful${RESET}"
fi

# Build
if [ $CONFIGURE_ONLY -eq 0 ]; then
  if [ ${#TARGETS[@]} -gt 0 ]; then
    echo -e "${BLUE}Building targets in preset '$PRESET': ${TARGETS[*]}${RESET}"
  else
    echo -e "${BLUE}Building preset: $PRESET${RESET}"
  fi

  BUILD_ARGS=()
  for target in "${TARGETS[@]}"; do
    BUILD_ARGS+=(--target "$target")
  done
  if [ $VERBOSE -eq 1 ]; then
    BUILD_ARGS+=(--verbose)
  fi

  cmake --build "$BUILD_DIR" "${BUILD_ARGS[@]}"

  if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${RESET}"
    exit $?
  fi
  echo -e "${GREEN}Build successful${RESET}"
fi

echo -e "${GREEN}========================================${RESET}"
echo -e "${GREEN}Build completed successfully!${RESET}"
echo -e "${GREEN}Output directory: $BUILD_DIR${RESET}"
echo -e "${GREEN}========================================${RESET}"
