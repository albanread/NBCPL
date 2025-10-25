#!/usr/bin/env python3
"""
Build script for NewBCPL compiler
Usage:
  ./build.py                 - Clean build (default, recommended)
  ./build.py --incr          - Incremental build (faster for development)
  ./build.py --runtime=MODE  - Specify runtime mode (jit, standalone, unified)

Clean builds are the default to ensure reliability and catch issues that
incremental builds might miss. Use --incr only when doing rapid development.
"""

import os
import sys
import subprocess
import shutil
import argparse
import multiprocessing
import glob
import time
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
from tqdm import tqdm


class BuildConfig:
    def __init__(self):
        self.build_dir = "build"
        self.obj_dir = os.path.join(self.build_dir, "obj")
        self.bin_dir = os.path.join(self.build_dir, "bin")
        self.errors_file = "errors.txt"
        self.max_jobs = multiprocessing.cpu_count()
        self.runtime_mode = "jit"
        self.clean_build = True


class NewBCPLBuilder:
    def __init__(self, config):
        self.config = config
        self.compilation_errors = []

    def log(self, message, level="INFO"):
        """Log messages with consistent formatting"""
        if level == "ERROR":
            print(f"❌ {message}", file=sys.stderr)
        elif level == "SUCCESS":
            print(f"✅ {message}")
        elif level == "INFO":
            print(f"📝 {message}")
        else:
            print(message)

    def run_command(self, cmd, shell=False, capture_output=True, check=True):
        """Run a command with error handling"""
        try:
            if isinstance(cmd, str) and not shell:
                cmd = cmd.split()

            result = subprocess.run(
                cmd, shell=shell, capture_output=capture_output, text=True, check=check
            )
            return result
        except subprocess.CalledProcessError as e:
            with open(self.config.errors_file, "a") as f:
                f.write(
                    f"Command failed: {' '.join(cmd) if isinstance(cmd, list) else cmd}\n"
                )
                f.write(f"Error: {e.stderr}\n")
            raise

    def clean_build_artifacts(self):
        """Clean previous build artifacts while preserving runtime libraries"""
        if not self.config.clean_build:
            return

        self.log("Performing clean build (default)...")
        self.log("Cleaning previous build artifacts (excluding runtime libraries)...")

        # Backup runtime libraries
        temp_backup = "temp_lib_backup"
        runtime_libs = [
            "libbcpl_runtime_jit.a",
            "libbcpl_runtime_c.a",
            "libbcpl_runtime.a",
        ]

        os.makedirs(temp_backup, exist_ok=True)
        for lib in runtime_libs:
            if os.path.exists(lib):
                shutil.copy2(lib, temp_backup)

        # Remove build directory
        if os.path.exists(self.config.build_dir):
            try:
                shutil.rmtree(self.config.build_dir)
            except OSError as e:
                self.log(f"Warning: Could not completely clean build directory: {e}")

        # Restore runtime libraries
        for lib in runtime_libs:
            backup_path = os.path.join(temp_backup, lib)
            if os.path.exists(backup_path):
                shutil.copy2(backup_path, lib)

        if os.path.exists(temp_backup):
            shutil.rmtree(temp_backup)

    def setup_build_directories(self):
        """Create necessary build directories"""
        os.makedirs(self.config.obj_dir, exist_ok=True)
        os.makedirs(self.config.bin_dir, exist_ok=True)
        self.log(
            f"Created build directories: {self.config.obj_dir} and {self.config.bin_dir}"
        )

    def increment_version(self):
        """Increment patch version number"""
        self.log("Incrementing version number...")
        version_file = "version.h"

        if not os.path.exists(version_file):
            return

        with open(version_file, "r") as f:
            lines = f.readlines()

        with open(version_file, "w") as f:
            for line in lines:
                if line.strip().startswith("#define BCPL_VERSION_PATCH"):
                    parts = line.strip().split()
                    if len(parts) >= 3:
                        try:
                            patch = int(parts[2]) + 1
                            f.write(f"#define BCPL_VERSION_PATCH {patch}\n")
                        except ValueError:
                            f.write(line)
                    else:
                        f.write(line)
                else:
                    f.write(line)

    def find_source_files(self):
        """Find all source files to compile - match bash script exactly"""
        all_files = set()

        # Core files (excluding files that are explicitly added elsewhere)
        core_files = glob.glob("./*.cpp")
        for file in core_files:
            filename = os.path.basename(file)
            # Exclude specific files as in bash script
            if not (
                filename.startswith("live_")
                or filename == "peephole_test.cpp"
                or filename == "test_sdl2_minimal.cpp"
                or filename.startswith("test_")
                or filename.startswith("debug_")
            ):
                all_files.add(file)

        # Other directories - use recursive search
        directory_patterns = [
            "encoders/**/*.cpp",
            "passes/**/*.cpp",
            "linker_helpers/**/*.cpp",
            "codegen/**/*.cpp",
            "modules/**/*.cpp",
            "generators/**/*.cpp",
        ]
        for pattern in directory_patterns:
            files = glob.glob(pattern, recursive=True)
            all_files.update(files)

        # testing/encoder_validation (exclude test_nop_validation.cpp)
        test_files = glob.glob("testing/encoder_validation/*.cpp")
        for file in test_files:
            if not os.path.basename(file) == "test_nop_validation.cpp":
                all_files.add(file)

        # Analysis files - include all subdirectories recursively
        analysis_files = glob.glob("analysis/**/*.cpp", recursive=True)
        all_files.update(analysis_files)

        # Peephole pattern files
        peephole_files = glob.glob("optimizer/patterns/*.cpp")
        all_files.update(peephole_files)

        # HeapManager files
        heap_files = glob.glob("HeapManager/*.cpp")
        all_files.update(heap_files)

        # Live analysis files
        live_files = glob.glob("./live_*.cpp")
        all_files.update(live_files)

        # Signal files
        signal_files = ["./SignalSafeUtils.cpp", "./SignalHandler.cpp"]
        for file in signal_files:
            if os.path.exists(file):
                all_files.add(file)

        # Format files
        format_files = glob.glob("format/*.cpp")
        all_files.update(format_files)

        return sorted(list(all_files))

    def get_files_to_compile(self, all_source_files):
        """Determine which files need compilation"""
        if self.config.clean_build:
            return all_source_files

        files_to_compile = []
        for src_file in all_source_files:
            base_name = os.path.splitext(os.path.basename(src_file))[0]
            obj_file = os.path.join(self.config.obj_dir, f"{base_name}.o")

            if not os.path.exists(obj_file) or os.path.getmtime(
                src_file
            ) > os.path.getmtime(obj_file):
                files_to_compile.append(src_file)

        return files_to_compile

    def compile_single_file(self, src_file):
        """Compile a single source file"""
        base_name = os.path.splitext(os.path.basename(src_file))[0]
        obj_file = os.path.join(self.config.obj_dir, f"{base_name}.o")

        compile_cmd = [
            "clang++",
            "-g",
            "-fno-omit-frame-pointer",
            "-std=c++17",
            "-DSDL2_RUNTIME_ENABLED",
            "-I.",
            "-I./NewBCPL",
            "-I./analysis/az_impl",
            "-I./analysis",
            "-I./",
            "-I./include",
            "-I./HeapManager",
            "-I./runtime",
            "-c",
            src_file,
            "-o",
            obj_file,
        ]

        try:
            result = subprocess.run(
                compile_cmd, capture_output=True, text=True, check=True
            )
            return True, src_file, ""
        except subprocess.CalledProcessError as e:
            error_msg = f"Compilation failed for {src_file}:\n{e.stderr}"
            return False, src_file, error_msg

    def compile_sources(self, files_to_compile):
        """Compile source files with progress bar"""
        if not files_to_compile:
            self.log("No source files need recompilation.")
            return True

        total_files = len(files_to_compile)
        self.log(
            f"Compiling {total_files} source files (debug enabled, clang++, {self.config.max_jobs} parallel jobs)..."
        )

        # Clear errors file
        open(self.config.errors_file, "w").close()

        success_count = 0
        failed_files = []

        with ProcessPoolExecutor(max_workers=self.config.max_jobs) as executor:
            # Submit all compilation tasks
            future_to_file = {
                executor.submit(self.compile_single_file, src_file): src_file
                for src_file in files_to_compile
            }

            # Process completed tasks with progress bar
            with tqdm(total=total_files, desc="Compiling", unit="files") as pbar:
                for future in as_completed(future_to_file):
                    success, src_file, error_msg = future.result()

                    if success:
                        success_count += 1
                    else:
                        failed_files.append(src_file)
                        with open(self.config.errors_file, "a") as f:
                            f.write(error_msg + "\n")

                    pbar.update(1)
                    pbar.set_postfix(
                        {"Success": success_count, "Failed": len(failed_files)}
                    )

        if failed_files:
            self.log(f"Compilation failed for {len(failed_files)} files", "ERROR")
            self.log("Please check errors.txt and fix errors", "ERROR")
            return False

        self.log("Compilation phase completed successfully.")
        return True

    def check_compilation_errors(self):
        """Check for compilation errors and display appropriate message"""
        # If no errors file exists or it's empty, compilation succeeded
        if (
            not os.path.exists(self.config.errors_file)
            or os.path.getsize(self.config.errors_file) == 0
        ):
            print("----------------------------------------")
            self.log("Compilation succeeded, please continue", "SUCCESS")
            print("----------------------------------------")
            return True
        else:
            print("----------------------------------------")
            self.log("BUILD FAILED: Compilation errors detected!", "ERROR")
            self.log("Please check errors.txt and fix errors", "ERROR")
            print("----------------------------------------")
            return False

    def determine_runtime_library(self):
        """Determine which runtime library to use"""
        runtime_options = [
            (
                "./libbcpl_runtime_graphics_static.a",
                "unified runtime library with Cairo graphics and static SDL2 (self-contained)",
                True,
                True,
            ),
            (
                "./libbcpl_runtime_cairo_static.a",
                "unified runtime library with Cairo graphics (self-contained)",
                True,
                False,
            ),
            (
                "./libbcpl_runtime_sdl2_static.a",
                "unified runtime library with static SDL2 (self-contained)",
                False,
                True,
            ),
        ]

        for lib_path, description, has_cairo, has_sdl2 in runtime_options:
            if os.path.exists(lib_path):
                self.log(f"Using {description}")
                return lib_path, has_cairo, has_sdl2

        # Fallback
        return "./libbcpl_runtime_sdl2_static.a", False, True

    def get_library_flags(self, has_cairo, has_sdl2):
        """Get library flags for linking"""
        defines = "-DSDL2_RUNTIME_ENABLED"
        cairo_libs = ""
        sdl2_libs = ""

        if has_cairo:
            defines += " -DCAIRO_RUNTIME_ENABLED"
            try:
                result = subprocess.run(
                    ["pkg-config", "--libs", "cairo"],
                    capture_output=True,
                    text=True,
                    check=True,
                )
                cairo_libs = result.stdout.strip()
                self.log("Including Cairo libraries in link")
            except (subprocess.CalledProcessError, FileNotFoundError):
                cairo_libs = "-lcairo"

        if has_sdl2:
            try:
                result = subprocess.run(
                    ["pkg-config", "--libs", "sdl2"],
                    capture_output=True,
                    text=True,
                    check=True,
                )
                sdl2_libs = result.stdout.strip()
                self.log("Including SDL2 libraries in link")
            except (subprocess.CalledProcessError, FileNotFoundError):
                sdl2_libs = "-lSDL2"

        return defines, cairo_libs, sdl2_libs

    def link_executable(self):
        """Link object files into final executable"""
        runtime_lib, has_cairo, has_sdl2 = self.determine_runtime_library()
        defines, cairo_libs, sdl2_libs = self.get_library_flags(has_cairo, has_sdl2)

        self.log(
            f"Linking object files with {runtime_lib.split('/')[-1]} (debug enabled, clang++)..."
        )

        # Get object files that correspond to our source files
        all_source_files = self.find_source_files()
        obj_files = []
        for src_file in all_source_files:
            base_name = os.path.splitext(os.path.basename(src_file))[0]
            obj_file = os.path.join(self.config.obj_dir, f"{base_name}.o")
            if os.path.exists(obj_file):
                obj_files.append(obj_file)

        if not obj_files:
            self.log("No object files found for linking", "ERROR")
            return False

        executable_path = os.path.join(self.config.bin_dir, "NewBCPL")

        # macOS system frameworks for static SDL2
        extra_libs = [
            "-lstdc++",
            "-framework",
            "Cocoa",
            "-framework",
            "CoreVideo",
            "-framework",
            "IOKit",
            "-framework",
            "Carbon",
            "-framework",
            "AudioToolbox",
            "-framework",
            "ForceFeedback",
            "-framework",
            "CoreAudio",
            "-framework",
            "CoreFoundation",
            "-framework",
            "Foundation",
            "-framework",
            "GameController",
            "-framework",
            "CoreHaptics",
            "-framework",
            "Metal",
            "-lpthread",
        ]

        link_cmd = (
            ["clang++", "-g", "-std=c++17"]
            + defines.split()
            + [
                "-I.",
                "-I./include",
                "-I./HeapManager",
                "-I./runtime",
                "-o",
                executable_path,
            ]
            + obj_files
            + [runtime_lib]
            + extra_libs
        )

        if cairo_libs:
            link_cmd.extend(cairo_libs.split())
        if sdl2_libs:
            link_cmd.extend(sdl2_libs.split())

        try:
            self.run_command(link_cmd, capture_output=False)
            return True
        except subprocess.CalledProcessError:
            self.log("BUILD FAILED: Linking errors detected!", "ERROR")
            self.log("Please check errors.txt and fix errors", "ERROR")
            return False

    def codesign_executable(self):
        """Codesign the executable for JIT entitlements"""
        self.log("Codesigning the binary with JIT entitlement...")
        executable_path = os.path.join(self.config.bin_dir, "NewBCPL")

        try:
            self.run_command(
                [
                    "codesign",
                    "--entitlements",
                    "entitlements.plist",
                    "--sign",
                    "-",
                    executable_path,
                ],
                capture_output=False,
            )
            return True
        except subprocess.CalledProcessError:
            self.log("BUILD FAILED: Codesigning errors detected!", "ERROR")
            self.log("Please check errors.txt and fix errors", "ERROR")
            return False

    def copy_executable(self):
        """Copy executable to target location"""
        src_path = os.path.join(self.config.bin_dir, "NewBCPL")
        dst_path = "../NewBCPL/NewBCPL"

        try:
            shutil.copy2(src_path, dst_path)
            self.log(f"Executable copied to {dst_path}")
            return True
        except Exception as e:
            self.log("BUILD FAILED: Failed to copy executable!", "ERROR")
            self.log("Please check errors.txt and fix errors", "ERROR")
            with open(self.config.errors_file, "a") as f:
                f.write(f"Copy error: {e}\n")
            return False

    def run_combiner(self):
        """Run the combiner script"""
        self.log("Running file combiner...")

        # Remove old combined file
        if os.path.exists("combined.txt"):
            os.remove("combined.txt")

        try:
            self.run_command(
                ["./combiner.sh", ".", "combined.txt"], capture_output=False
            )
            return True
        except subprocess.CalledProcessError:
            self.log("Warning: Combiner failed, but build succeeded")
            return True  # Don't fail the entire build for combiner issues

    def build(self):
        """Main build process"""
        try:
            # Setup phase
            self.clean_build_artifacts()
            self.setup_build_directories()
            self.increment_version()

            # Compilation phase
            all_source_files = self.find_source_files()
            files_to_compile = self.get_files_to_compile(all_source_files)

            # Ensure errors file exists and is empty at start
            open(self.config.errors_file, "w").close()

            if not self.compile_sources(files_to_compile):
                return False

            if not self.check_compilation_errors():
                return False

            # Linking phase
            if not self.link_executable():
                return False

            # Post-build steps
            if not self.codesign_executable():
                return False

            self.log(
                f"Build complete. Executable is at {os.path.join(self.config.bin_dir, 'NewBCPL')}"
            )

            if not self.copy_executable():
                return False

            self.run_combiner()

            print("----------------------------------------")
            self.log("Build process finished successfully.", "SUCCESS")
            self.log(f"Runtime mode: {self.config.runtime_mode}")
            runtime_lib, _, _ = self.determine_runtime_library()
            self.log(f"Runtime library: {runtime_lib}")

            return True

        except Exception as e:
            self.log(f"Build failed with exception: {e}", "ERROR")
            return False


def main():
    parser = argparse.ArgumentParser(description="Build NewBCPL compiler")
    parser.add_argument(
        "--incr", action="store_true", help="Incremental build (faster for development)"
    )
    parser.add_argument(
        "--runtime",
        choices=["jit", "standalone", "unified"],
        default="jit",
        help="Runtime mode",
    )

    args = parser.parse_args()

    config = BuildConfig()
    config.clean_build = not args.incr
    config.runtime_mode = args.runtime

    if args.incr:
        print("Performing incremental build (--incr)...")
    else:
        print("Performing clean build (default)...")

    builder = NewBCPLBuilder(config)
    success = builder.build()

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
