#!/usr/bin/env python3
"""
Game Engine Build Script
Cross-platform build automation for CMake presets
"""

import os
import sys
import subprocess
import shutil
import argparse
import json
from pathlib import Path
import platform


# Colors for output
class Colors:
    GREEN = "\033[0;32m"
    YELLOW = "\033[1;33m"
    RED = "\033[0;31m"
    BLUE = "\033[0;34m"
    RESET = "\033[0m"

    @staticmethod
    def disable():
        Colors.GREEN = ""
        Colors.YELLOW = ""
        Colors.RED = ""
        Colors.BLUE = ""
        Colors.RESET = ""


# Windows doesn't support ANSI colors by default
if platform.system() == "Windows":
    # Enable ANSI colors on Windows 10+
    try:
        import ctypes

        kernel32 = ctypes.windll.kernel32
        kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)
    except:
        Colors.disable()


class BuildSystem:
    def __init__(self):
        self.presets = {
            "runtime": {
                "description": "Build only Sandbox",
                "cache_vars": {
                    "BUILD_SANDBOX": "ON",
                    "BUILD_EDITOR": "OFF",
                    "BUILD_SERVER": "OFF",
                    "BUILD_TOOLS": "OFF",
                },
            },
            "editor": {
                "description": "Build only Editor",
                "cache_vars": {
                    "BUILD_SANDBOX": "OFF",
                    "BUILD_EDITOR": "ON",
                    "BUILD_SERVER": "OFF",
                    "BUILD_TOOLS": "OFF",
                },
            },
            "editor-runtime": {
                "description": "Build Editor + Sandbox",
                "cache_vars": {
                    "BUILD_SANDBOX": "ON",
                    "BUILD_EDITOR": "ON",
                    "BUILD_SERVER": "OFF",
                    "BUILD_TOOLS": "OFF",
                },
            },
            "server": {
                "description": "Build only Server",
                "cache_vars": {
                    "BUILD_SANDBOX": "OFF",
                    "BUILD_EDITOR": "OFF",
                    "BUILD_SERVER": "ON",
                    "BUILD_TOOLS": "OFF",
                },
            },
            "full": {
                "description": "Build everything",
                "cache_vars": {
                    "BUILD_SANDBOX": "ON",
                    "BUILD_EDITOR": "ON",
                    "BUILD_SERVER": "ON",
                    "BUILD_TOOLS": "ON",
                },
            },
            "web": {
                "description": "Build for Web/Emscripten",
                "cache_vars": {
                    "EMSCRIPTEN": "ON",
                    "BUILD_SANDBOX": "ON",
                    "BUILD_EDITOR": "OFF",
                    "BUILD_SERVER": "OFF",
                    "BUILD_TOOLS": "OFF",
                },
                "requires": ["emcmake"],
            },
        }

    def check_dependencies(self, preset):
        """Check if required dependencies are available"""
        if preset == "web":
            if shutil.which("emcmake") is None:
                print(
                    f"{Colors.RED}Error: emcmake not found. Please install Emscripten.{Colors.RESET}"
                )
                return False
        return True

    def run_command(self, cmd, verbose=False):
        """Run a command and handle output"""
        print(f"{Colors.BLUE}Running: {' '.join(cmd)}{Colors.RESET}")

        if verbose:
            result = subprocess.run(cmd, shell=(platform.system() == "Windows"))
        else:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                shell=(platform.system() == "Windows"),
            )

        if result.returncode != 0:
            if not verbose:
                print(result.stdout)
                print(result.stderr)
            print(
                f"{Colors.RED}Command failed with exit code {result.returncode}{Colors.RESET}"
            )
            sys.exit(result.returncode)

        return result

    def clean(self, preset):
        """Clean build directory"""
        build_dir = Path(f"build/{preset}")
        if build_dir.exists():
            print(f"{Colors.YELLOW}Cleaning {build_dir}{Colors.RESET}")
            shutil.rmtree(build_dir)
            print(f"{Colors.GREEN}Cleaned successfully{Colors.RESET}")
        else:
            print(
                f"{Colors.YELLOW}Build directory doesn't exist, nothing to clean{Colors.RESET}"
            )

    def configure(self, preset, verbose=False):
        """Configure the project"""
        print(f"{Colors.BLUE}Configuring preset: {preset}{Colors.RESET}")

        cmd = []
        if preset == "web":
            cmd.append("emcmake")

        cmd.append("cmake")
        cmd.append(f"--preset={preset}")

        if verbose:
            cmd.append("--verbose")

        self.run_command(cmd, verbose)
        print(f"{Colors.GREEN}Configuration successful{Colors.RESET}")

    def build(self, preset, verbose=False, clean=False):
        """Build the project"""
        if clean:
            self.clean(preset)

        self.configure(preset, verbose)

        print(f"{Colors.BLUE}Building preset: {preset}{Colors.RESET}")

        build_dir = f"build/{preset}"
        cmd = ["cmake", "--build", build_dir]

        if verbose:
            cmd.append("--verbose")

        # Add parallel jobs for faster builds
        if platform.system() != "Windows":
            import multiprocessing

            jobs = multiprocessing.cpu_count()
            cmd.extend(["-j", str(jobs)])

        self.run_command(cmd, verbose)
        print(f"{Colors.GREEN}Build successful{Colors.RESET}")

    def run_tests(self, preset):
        """Run tests after building"""
        build_dir = f"build/{preset}"
        cmd = ["ctest", "--test-dir", build_dir, "--output-on-failure"]
        self.run_command(cmd)

    def list_presets(self):
        """List available presets"""
        print(f"{Colors.BLUE}========================================{Colors.RESET}")
        print(f"{Colors.GREEN}Available Presets{Colors.RESET}")
        print(f"{Colors.BLUE}========================================{Colors.RESET}")
        for name, info in self.presets.items():
            print(f"  {Colors.GREEN}{name:16}{Colors.RESET} - {info['description']}")


def main():
    parser = argparse.ArgumentParser(description="Game Engine Build Script")
    parser.add_argument(
        "preset",
        nargs="?",
        choices=["runtime", "editor", "editor-runtime", "server", "full", "web"],
        help="Build preset to use",
    )
    parser.add_argument(
        "--clean", action="store_true", help="Clean build directory before building"
    )
    parser.add_argument(
        "--configure", action="store_true", help="Only configure, don't build"
    )
    parser.add_argument(
        "--build", action="store_true", help="Only build (after configuration)"
    )
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    parser.add_argument("--list", action="store_true", help="List available presets")
    parser.add_argument("--test", action="store_true", help="Run tests after building")

    args = parser.parse_args()

    build = BuildSystem()

    if args.list:
        build.list_presets()
        return 0

    if not args.preset:
        parser.print_help()
        return 1

    if not build.check_dependencies(args.preset):
        return 1

    if args.build:
        build.build(args.preset, args.verbose, args.clean)
    elif args.configure:
        if args.clean:
            build.clean(args.preset)
        build.configure(args.preset, args.verbose)
    else:
        build.build(args.preset, args.verbose, args.clean)

    if args.test:
        build.run_tests(args.preset)

    print(f"{Colors.GREEN}========================================{Colors.RESET}")
    print(f"{Colors.GREEN}Build completed successfully!{Colors.RESET}")
    print(f"{Colors.GREEN}Output directory: build/{args.preset}{Colors.RESET}")
    print(f"{Colors.GREEN}========================================{Colors.RESET}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
