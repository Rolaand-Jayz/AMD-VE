# Baseline Audit

## Target
- Repository: <repo-root> (see PHASE0_COMMAND_OUTPUTS.log for resolved path)
- Branch: main
- Commit: fef8c8adfbe61da0b05d154ed34fcdee9b0298d1
- Audit date: 2026-05-07
- Auditor: Crush (automated agent)
- Scope: Phase 0 verified handoff — repo state, build, tests, seams, gaps, limitations

## Git State

Command:
    git status --short

Raw output location:
    docs/implementation/PHASE0_COMMAND_OUTPUTS.log

Summary:
- Clean/dirty: dirty (untracked files only, no modified tracked files in captured local audit state)
- Modified files: none (in captured local audit state)
- Untracked files:
  - `amd-ve-temporal-forge-roadmap-conditional-handoff-r2/` (R2 archive)
  - `docs/implementation/` (Phase 0 output files)
- Ignored/not inspected: build/ directory (CMake build output)

Captured local audit state:
The Phase 0 audit recorded the local repository state at the time of audit.

Transfer archive state:
Transfer archives may show benchmarks/ave_benchmark_960x540_20s.mp4 as modified, missing, or altered because benchmark media was removed or changed to save space. This is a media-transfer artifact, not a source-code handoff blocker.

Benchmark reproducibility:
The transferred benchmark media state must not be used as proof of benchmark reproducibility. Reproducible benchmark claims require restoring or regenerating the fixture and recording fixture path, source or generation method, hash, command, environment, output log, date, and verification label.

## Build/Test State

Build status: passed.

Command:
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
      -DAVE_HAVE_CURL=ON -DAVE_HAVE_MIGRAPHX=ON -DAVE_HAVE_HIP=ON \
      -DAVE_HAVE_VULKAN=ON -DAVE_HAVE_NCNN=ON
    cmake --build build -j

Evidence: full build completed at 100%, all targets built (ave, ave_gui, 14 test executables, ave_model_compile_sweep). No compilation errors. GCC 16.1.1, CMake 4.3.2.

Test status: passed.

Command:
    ctest --test-dir build --output-on-failure

Result: 19/19 tests passed, 0 failed. Total test time: 32.93s–34.87s (two runs verified).

Test detail:

| # | Test | Time | Status |
|---|------|------|--------|
| 1 | planner_tests | — | passed |
| 2 | tensor_contract_tests | 0.00s | passed |
| 3 | runtime_diagnostics_tests | 0.02s | passed |
| 4 | runtime_paths_tests | 0.00s | passed |
| 5 | model_manager_profile_tests | 15.97s | passed |
| 6 | video_probe_tests | 0.00s | passed |
| 7 | process_loop_tests | 0.00s | passed |
| 8 | process_observer_tests | 0.11s | passed |
| 9 | process_progress_tests | 0.00s | passed |
| 10 | rgb_video_loop_tests | 1.66s | passed |
| 11 | frame_io_vulkan_tests | 1.19s | passed |
| 12 | frame_io_pipe_tests | 2.06s | passed |
| 13 | frame_io_direct_pack_tests | 1.11s | passed |
| 14 | ffmpeg_runner_tests | 1.19s | passed |
| 15 | job_recovery_tests | 0.00s | passed |
| 16 | job_queue_tests | 0.00s | passed |
| 17 | telemetry_tests | 0.00s | passed |
| 18 | migraphx_backend_tests | 10.29s | passed |
| 19 | interop_bridge_tests | 1.23s | passed |

## Environment

- OS/distro: CachyOS Linux (Arch-derivative), rolling
- Kernel: 7.0.3-1-cachyos (SMP PREEMPT)
- CPU: AMD Ryzen 7 7800X3D 8-Core (16 threads)
- GPU: AMD Radeon RX 7900 GRE
- RAM: 30 GiB
- ROCm: 7.2.2 (/opt/rocm)
- MiGraphX: 2.15.0
- Vulkan: 1.4.341
- FFmpeg: n8.1.1 (with libplacebo, Vulkan, vapoursynth)
- Qt: 6.11.0
- Compiler: g++ 16.1.1
- CMake: 4.3.2

## Backend Probe

Command: `./build/ave --list-backends`

| Backend | Available | Notes |
|---|---|---|
| MiGraphX | yes | Driver + library detected |
| ROCm/HIP | partial | ONNX Runtime found but ROCMExecutionProvider not available |
| Vulkan Compute | yes | Loader functional |
| NCNN Vulkan | yes | Runtime detected and ready |
| GLSL Shader | yes | Via libplacebo and mpv |
| VapourSynth | yes | Via vspipe |
| FFmpeg | yes | Always-available fallback |

## Existing Source Seams

### Model manager
- File: `src/model_manager.cpp` (3445 lines), `include/ave/model_manager.hpp` (189 lines)
- Class: `ModelManager`
- Key functions: model lifecycle (download, prepare, compile, validate), artifact fingerprinting, state tracking (NotDownloaded → Downloaded → Converted → Error)

### Model manifest parsing
- File: `src/model_catalog.cpp` (1143 lines), `include/ave/model_catalog.hpp` (193 lines)
- Class: `ModelCatalog`
- Key functions: `.avemodel` manifest parsing, model metadata, capabilities, format/precision resolution

### MiGraphX path
- File: `src/backends/migraphx_backend.cpp` (5528 lines)
- Class: `MigraphxBackend` (PIMPL)
- Key functions: `isAvailable()`, `initialize()`, `runStage()`, `processVideoFile()`, artifact validation, runtime fingerprinting, tile planning, tensor contracts, `model_capabilities` / `fused_requested_capabilities` parameter handling

### Vulkan / preview path
- File: `src/backends/vulkan_compute_backend.cpp` (1313 lines), `src/vulkan_runtime.cpp`, `src/frame_io_vulkan.cpp` (1002 lines), `src/interop_bridge.cpp` (375 lines)
- Classes: `VulkanComputeBackend`, `VulkanRuntime`, `InteropBridge`
- Note: no preview timeline/cache system exists. Preview is `--preview --preview-duration` only.

### CLI entrypoint
- File: `src/main.cpp` (86 lines), `src/cli.cpp` (291 lines), `include/ave/cli.hpp` (20 lines)
- Function: `parseCli()` — handles `--input`, `--output`, `--backend`, `--stage`, `--codec`, `--preview`, `--dry-run`, `--list-backends`

### GUI entrypoint
- File: `src/gui/main_gui.cpp` (125 lines), `src/gui/main_window.cpp` (4395 lines), `src/gui/main_window.hpp`
- Class: `MainWindow` (QMainWindow subclass)
- Note: single-window layout with stage controls, filter browser, settings, model manager dialogs

### Backend selection
- File: `src/backend_manager.cpp` (219 lines), `include/ave/backend_manager.hpp` (19 lines)
- Function: `BackendManager::createBackend()` — probes in order: MiGraphX → ROCm/HIP → Vulkan → NCNN

### Model preparation
- File: `src/model_manager.cpp` (contains ONNX preparation, MiGraphX compilation via `migraphx-driver`)
- Subprocess dependencies: `python3`, `unzip` required for source-build model preparation

### Subprocess execution

Full inventory: see `docs/implementation/SUBPROCESS_AUDIT.log`

- `std::system()` calls (6 locations):
  - `src/backends/glsl_shader_backend.cpp:39,411` (FFmpeg libplacebo check + pipeline)
  - `src/backends/vapoursynth_backend.cpp:82` (VapourSynth vspipe probe)
  - `tests/frame_io_pipe_tests.cpp:22` (test helper)
  - `tests/process_observer_tests.cpp:20` (test helper)
  - `tests/rgb_video_loop_tests.cpp:21` (test helper)
- `popen()` calls (9 locations):
  - `src/frame_io.cpp:824,1241,1442,1492,1532` — FFmpeg pipe I/O (decode + encode)
  - `src/ffmpeg_runner.cpp:213,307` — FFmpeg probing and filter execution
  - `src/process_observer.cpp:173,203` — subprocess output capture
- Shell safety measures:
  - `process_observer::quoteShellArg()` — used for all user-supplied values in frame_io
  - `isSafeFfmpegToken()` in `src/ffmpeg_runner.cpp:442` — validates codec/preset/profile tokens
  - `process_observer::commandInPath()` — checks for required tools before invocation
  - `process_observer::normalizeShellExitCode()` — wraps exit codes
- Model download:
  - `src/model_manager.cpp:1436-1555` — libcurl HTTPS-only with TLS verification, timeout, redirect limits
- PATH dependencies:
  - `python3` — required for ONNX preparation and PyTorch export (`model_manager.cpp:911,1964`)
  - `unzip` — required for archive extraction (`model_manager.cpp:1568`)
  - `ffmpeg` / `ffprobe` — required for all media operations
  - All missing-tool cases emit explicit error messages

### Tests
- Directory: `tests/` — 18 test files, 19 CTest entries
- All passing: planner, tensor_contract, runtime_diagnostics, runtime_paths, model_manager_profile, video_probe, process_loop, process_observer, process_progress, rgb_video_loop, frame_io_vulkan, frame_io_pipe, frame_io_direct_pack, ffmpeg_runner, job_recovery, job_queue, telemetry, migraphx_backend, interop_bridge

### Packaging/release docs
- `tools/build_native_packages.py` — native package generation for Arch, Ubuntu, Debian, Fedora, openSUSE, Rocky, Alma
- `docs/PACKAGING.md`, `docs/RELEASING.md`, `docs/RELEASE_STATUS.md`, `docs/SUPPORT_TIERS.md`
- `cmake/` install templates for bundled deployment

## Model Inventory

- Model directory: `~/.local/share/ave/models/`
- Downloaded: 43 source files (ONNX, NCNN, PyTorch)
- MiGraphX compiled: 50 `.mxr` artifacts + 50 `.mxr.manifest` sidecars
- Prepared: 3 ONNX variants
- Optimized: 0 files

## Known Repo Facts

1. The repo is a working beta with a published GitHub prerelease `v0.1.0-beta.1`.
2. Only verified on Arch Linux + 7800X3D + 7900 GRE.
3. MiGraphX backend is the most mature code (5528 lines) with artifact discipline.
4. All five backend paths (MiGraphX, ROCm/HIP, Vulkan, NCNN, FFmpeg) are functional in the build.
5. ROCm/HIP ONNX Runtime EP is not available on the reference system (partial backend).
6. Bundled MiGraphX runtime is required because upstream does not yet provide the needed behavior.
7. Vulkan↔HIP interop bridge exists but default MiGraphX path uses host staging.
8. No mode/family/preset metadata system exists.
9. No fused stage planner exists.
10. No preview cache or timeline UI exists.
11. No portable profile serialization exists (GUI uses QSettings; CLI has no profile concept).

## Unknowns

1. Whether ROCm/HIP ONNX Runtime EP works on any external system.
2. Whether the bundled MiGraphX runtime behaves identically across distros.
3. Actual visual quality comparison against commercial tools (no corpus exists).
4. Whether the Vulkan↔HIP interop bridge works on any non-reference GPU.
5. Whether packaging scripts produce working packages on any non-Arch system.

## Resource Constraints

1. Single maintainer, single reference system.
2. No multi-machine CI/CD — build verification is manual.
3. No external tester corpus yet.
4. No comprehensive visual-quality benchmark suite.
5. No full public website (specification pack exists but site is not deployed).
6. No independent release artifact reproduction.

## Raw Build/Test Evidence

- Path: `build/Testing/Temporary/LastTest.log`
- Reported result: 19/19 tests passed

This proves the audited local Phase 0 state only. It does not prove broad environment compatibility, release artifact reproducibility, visual quality, or Temporal Forge readiness.

## Do Not Infer

1. Do not assume Temporal Forge exists in the codebase. It does not.
2. Do not assume mode/family/preset metadata exists. It does not.
3. Do not assume a preview cache/timeline exists. It does not.
4. Do not assume fused stage execution exists. It does not.
5. Do not assume profile serialization is portable. It is not.
6. Do not assume the Vulkan↔HIP interop is the default inference path. It is not.
7. Do not assume broad distro compatibility. Only Arch on the reference system is verified.
8. Do not assume the bundled MiGraphX is identical to upstream stock MiGraphX. It is not.
9. Do not create fictional source seams. If something is listed as "not found", it genuinely does not exist in the repo.
