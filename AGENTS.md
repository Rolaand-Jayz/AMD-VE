# AGENTS.md

Guide for AI agents working in the AMD Video Enhancer C++ codebase.

## Project Overview

AMD-exclusive C++ video enhancement pipeline. No Python, no CUDA, no NVIDIA.

- **Primary backend**: MiGraphX (ROCm) — ONNX load, compile, GPU inference
- **ROCm/HIP backend**: ONNX Runtime ROCm fallback via HIP
- **Vulkan compute backend**: Vulkan SPIR-V compute shaders for GPU processing
- **Fallback backend**: NCNN Vulkan — Vulkan-accelerated inference
- **Script pipelines**: GLSL shader (mpv/libplacebo) and VapourSynth backends
- **Always available**: FFmpeg filter chain — no model required

## No-stub policy

- Stubs, placeholder behavior, fake completion paths, mock implementations presented as real, TODO-backed user flows, and `NotImplemented`-style stand-ins are forbidden in app code.
- This same rule applies to repo guidance artifacts: instructions, prompts, agents, skills, plans, and remediation docs must not normalize or recommend stub use as an acceptable path.
- If a feature cannot be implemented end-to-end, state the blocker plainly instead of pretending the feature exists.
- When auditing for stubs, document them accurately; do not silently convert the audit into speculative fixes unless the task is specifically remediation.

## Essential Commands

### Build

**IMPORTANT**: Always enable ALL available backends. Never build without them.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DAVE_HAVE_CURL=ON \
  -DAVE_HAVE_MIGRAPHX=ON \
  -DAVE_HAVE_HIP=ON \
  -DAVE_HAVE_VULKAN=ON \
  -DAVE_HAVE_NCNN=ON
cmake --build build -j
```

> **Never** use a bare `cmake -S . -B build` without backend flags.
> MiGraphX, HIP, Vulkan, and NCNN must always be explicitly enabled.

### Test

```bash
ctest --test-dir build --output-on-failure
```

Tests are **per-component executables** (not a framework). Each test file compiles to its own binary with the minimum source dependencies it needs. There are 17 test executables registered via `add_test()`. Some tests link only selected `.cpp` files (not `ave_core`) to keep compile times low and test boundaries explicit.

### Run

```bash
./build/ave --input input.mp4 --output output.mp4 --backend auto --stage upscale:width=3840,height=2160
./build/ave_gui

./build/ave --list-backends     # Probe backends + runtime diagnostics + telemetry
./build/ave --dry-run ...       # Plan only, no processing
```

### Portable bundle

```bash
cmake --build build --target portable_stage   # Stage only (no archive)
cmake --build build --target portable_bundle  # Full compressed bundle
```

Output lands in `build/dist/`. Requires Python 3 (for `tools/create_portable_bundle.py`).

### Benchmarks

```bash
./tools/bench_ave.sh <binary> <input> <output> <width> <height> <model>
```

Benchmark clip: `benchmarks/ave_benchmark_960x540_20s.mp4`. Results stored in `benchmarks/history/`.

## Build Targets

| Target | Description |
| ------ | ----------- |
| `ave` | CLI executable |
| `ave_gui` | Qt6 GUI executable (conditional on Qt6) |
| `ave_core` | Core static library (all sources, all backends) |
| `ave_model_compile_sweep` | ONNX→MXR batch compiler tool |
| `portable_stage` / `portable_bundle` | Self-contained distribution bundles |

## CMake Options

| Option | Default | Description |
| --- | --- | --- |
| `AVE_STRICT_WARNINGS` | ON | `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion` |
| `AVE_BUILD_GUI` | ON | Build Qt6 GUI |
| `AVE_HAVE_CURL` | ON | libcurl for model downloads |
| `AVE_HAVE_VULKAN` | ON | Vulkan runtime (compute shaders, interop) |
| `AVE_HAVE_MIGRAPHX` | OFF | MiGraphX ROCm runtime — **always pass ON** |
| `AVE_HAVE_HIP` | OFF | HIP GPU headers (ROCm) — **always pass ON** |
| `AVE_HAVE_ONNXRUNTIME_ROCM` | ON | ONNX Runtime ROCm/HIP fallback |
| `AVE_HAVE_NCNN` | OFF | NCNN Vulkan runtime — **always pass ON** |
| `AVE_HAVE_ROCTX` | OFF | ROCTx markers for `rocprof --roctx-trace` |
| `AVE_INSTALL_BUNDLED_MIGRAPHX` | OFF | Bundle MiGraphX runtime in install |
| `AVE_INSTALL_BUNDLED_MODELS` | ON | Install full model catalog |

> CMake defaults for MIGRAPHX, HIP, and NCNN are OFF, but **agents must always explicitly set them ON**.

Additional cache variables:
- `AVE_BUNDLED_MIGRAPHX_PREFIX` — path to a custom MiGraphX install for bundling
- `AVE_PORTABLE_BUNDLE_NAME` — output folder/archive name for portable bundles

## Architecture and Control Flow

### Processing pipeline

1. **CLI parse** (`cli.cpp`) → `CliResult` containing optional `VideoJob`
2. **`VideoProcessor::process()`** orchestrates the full pipeline:
   - `ModelManager::refresh()` — scan model directories
   - `PipelinePlanner::plan()` — reorder user stages into deterministic execution order
   - `BackendManager::createBackend()` — probe and select best available backend
   - Per-stage `resolveModelPath()` — inject `model_path` into each stage based on backend type
   - **Backend pre-load pass** — for each non-Interpolate/Sharpen stage: `backend->runStage()` returns `Processed`/`Deferred`/`Error`/`Cancelled`
   - **FFmpeg encode pass** — `FfmpegRunner::encode()` runs the actual frame-by-frame pipeline, integrating AI-processed stages and FFmpeg filter fallbacks
3. Script backends (GLSL/VapourSynth) defer all work to the FFmpeg encode phase

### Backend selection (`BackendManager::createBackend`)

Priority when `--backend auto`:
1. **MiGraphX** — requires `/opt/rocm`, `rocminfo`, `libmigraphx.so`
2. **ROCm/HIP** — ONNX Runtime ROCm provider
3. **NCNN Vulkan** — requires `vulkaninfo` or `libvulkan.so`
4. **Vulkan Compute** — SPIR-V compute shaders
5. **FFmpeg filters** — always available

### Model resolution (`resolveModelPath`)

- MiGraphX backends prefer compiled `.mxr` artifacts from `migraphx/` subdir
- Non-MiGraphX backends get the original downloaded model (not `.mxr`)
- Stages without an explicit model fall back to the catalog default for their `StageKind`
- Explicit user `model_path` is preserved as-is

### Frame I/O pipeline (`frame_io`)

Multiple source modes with automatic fallback:
- **RawPipe** — FFmpeg raw RGB24 pipe (default, most compatible)
- **VulkanTransfer** — Vulkan hardware frames transferred to CPU
- **VulkanHardware** — Vulkan AVVkFrame kept on GPU for interop

Key classes:
- `VideoFrameSource` — unified reader with mode selection
- `AsyncRgbVideoPipeEncoder` — threaded pipe encoder for write-back
- `VideoFramePacketMaterializer` — reusable packet-to-CPU conversion without per-frame allocations

### Vulkan↔HIP interop (`InteropBridge`)

Zero-copy GPU memory sharing for same-process inference:
1. Vulkan exports `VkDeviceMemory` as POSIX FD (OPAQUE_FD)
2. HIP imports FD → device pointer passed directly to MiGraphX
3. Semaphore handshake: Vulkan signals `BufferReady` → HIP runs eval → signals `InferenceDone`

Required extensions: `VK_KHR_external_memory_fd`, `VK_KHR_external_semaphore_fd`, `VK_KHR_synchronization2` (or Vulkan 1.3).

### Job management

- `JobQueueStore` — persistent JSON-backed queue with retryable failure tracking
- `JobRecoveryStore` — crash recovery for in-progress jobs
- `VideoJob` — carries input/output paths, stages, encode settings, preview options, cancel/pause flags

### Observability

- `obs::logVersionTuple()` — ROCm, MiGraphX, Vulkan, FFmpeg, GPU gfx target, kernel
- `obs::logMiGraphXEnvironment()` — all `MIGRAPHX_*` env vars that affect compilation
- `ArtifactManifestFields` — cache key for compiled `.mxr` artifacts (version, GPU arch, source fingerprint, precision, etc.)
- `AVE_ROCTX_RANGE()` / `AVE_ROCTX_RANGE_END()` — zero-overhead when `AVE_HAVE_ROCTX` is off

### Error taxonomy (`error_taxonomy.hpp`)

Every cross-boundary failure is classified as `InferenceErrorKind`:
- **Fail-fast**: `ModelIncompatible`, `SyncHazard`, `CompileFailure`, `ArtifactInvalid`, `InteropFailure`, `VulkanDeviceFailure`
- **Log + fallback**: `FFmpegInteropFailure`
- **Log + optional retry**: `RuntimeFailure`

Factory helpers: `InferenceError::modelIncompatible(msg, ctx)`, etc.

## Code Organization

```text
├── include/ave/
│   ├── backends/              # Backend-specific headers
│   │   ├── glsl_shader_backend.hpp
│   │   ├── migraphx_backend.hpp
│   │   ├── ncnn_vulkan_backend.hpp
│   │   ├── rocm_hip_backend.hpp
│   │   ├── vapoursynth_backend.hpp
│   │   └── vulkan_compute_backend.hpp
│   ├── app_settings.hpp       # Persistent user settings
│   ├── backend.hpp            # IAcceleratorBackend, BackendType, StageResult, ProcessVideoOptions
│   ├── backend_manager.hpp    # Backend probe/create factory
│   ├── cli.hpp                # CLI parser → CliResult
│   ├── error_taxonomy.hpp     # InferenceErrorKind, InferenceError
│   ├── ffmpeg_runner.hpp      # FFmpeg encode pipeline
│   ├── filter_catalog.hpp     # Embedded GLSL/VapourSynth filter library
│   ├── frame_io.hpp           # Frame I/O: Vulkan readers/writers, RGB pipes, tensor conversion
│   ├── interop_bridge.hpp     # Vulkan↔HIP zero-copy memory bridge
│   ├── job.hpp                # VideoJob, EncodeSettings
│   ├── job_queue.hpp          # Persistent job queue (QueuedJobRecord, JobQueueStore)
│   ├── job_recovery.hpp       # Crash recovery (RecoveredJobState, JobRecoveryStore)
│   ├── model_catalog.hpp      # ModelEntry, model registry, family/capability helpers
│   ├── model_manager.hpp      # Model download/discovery/path resolution
│   ├── observability.hpp      # ROCTx macros, version logging, artifact manifests
│   ├── planner.hpp            # PipelinePlanner — deterministic stage ordering
│   ├── process_loop.hpp       # Frame processing loop utilities
│   ├── process_observer.hpp   # Processing state observation
│   ├── process_progress.hpp   # FFmpeg progress parsing and reporting
│   ├── rgb_video_loop.hpp     # RGB frame-by-frame processing loop with multiple modes
│   ├── runtime_diagnostics.hpp # AmdRuntimeSnapshot, RuntimeDiagnosticsReport
│   ├── runtime_paths.hpp      # XDG/portal model and runtime path resolution
│   ├── scene_detector.hpp     # Scene cut detection for interpolation
│   ├── stage.hpp              # EnhancementStage, ParameterValue, parseStageSpec
│   ├── telemetry.hpp          # AMD GPU telemetry (utilization, VRAM, temps, clocks)
│   ├── tensor_contract.hpp    # Tensor shape/dtype assertions
│   ├── types.hpp              # StageKind enum
│   ├── video_probe.hpp        # FFprobe wrapper (VideoStreamProbe)
│   ├── video_processor.hpp    # Top-level pipeline orchestrator
│   └── vulkan_runtime.hpp     # Vulkan device/compute setup
├── src/
│   ├── backends/              # Backend implementations
│   ├── gui/                   # Qt6 GUI (conditional)
│   │   ├── main_gui.cpp       # GUI entry point
│   │   ├── main_window.cpp/hpp
│   │   ├── filter_browser.cpp/hpp
│   │   ├── model_manager_dialog.cpp/hpp
│   │   ├── settings_dialog.cpp/hpp
│   │   └── toggle_switch.cpp/hpp
│   ├── main.cpp               # CLI entry point
│   └── ...                    # One .cpp per header
├── tests/                     # One test file per component, each builds its own executable
├── tools/                     # Benchmark, packaging, and model compilation scripts
├── benchmarks/                # Benchmark clips and historical results
├── cmake/                     # Install templates (launch wrappers, bundling scripts)
├── packaging/                 # DEB/RPM/PKGBUILD templates
├── docs/
│   ├── BENCHMARKS.md          # Benchmark methodology and current numbers
│   ├── FEATURE_PARITY_MATRIX.md
│   ├── GOLD_STANDARD_FOR_IMPLEMENTATION.md
│   ├── LIMITATIONS.md
│   ├── RELEASE_STATUS.md
│   ├── RELEASING.md
│   ├── SUPPORT_TIERS.md
│   └── migraphx_debugging_playbook.md
└── .github/
    ├── instructions/          # Copilot/MCP instruction files
    ├── skills/                # Stub audit and destub skills
    ├── workflows/             # CI: release-native-packages.yml
    └── ISSUE_TEMPLATE/        # Bug, feature, benchmark report templates
```

## Enhancement Stages

Stages execute in deterministic order (enforced by `PipelinePlanner`):

1. **Group 0**: `RestoreCompression`, `RemoveArtifacts`, `Denoise`, `Deblur`, `Dehalo`
2. **Group 1**: `ColorFix`
3. **Group 2**: `Upscale`
4. **Group 3**: `Sharpen`
5. **Group 4**: `Interpolate` (always last)

### Stage aliases (CLI)

| Stage | Aliases |
| --- | --- |
| `restore_compression` | `decompress`, `deh264` |
| `remove_artifacts` | `deartifact`, `deblock` |

### Stage parameters

Stage params use `ParameterValue = std::variant<int64_t, double, bool, string>`. CLI format is `stage:key1=val1,key2=val2`. Helper accessors: `tryGetDouble()`, `tryGetInt()`, `tryGetBool()`, `tryGetString()` — each takes an optional `StageKind scope` for scoped key lookup.

## Backend Interface

All accelerators implement `IAcceleratorBackend` (see `backend.hpp`):

```cpp
virtual BackendType type() const = 0;
virtual std::string name() const = 0;
virtual bool isAvailable(std::string& reason) const = 0;
virtual bool initialize(std::string& error) = 0;
virtual bool supportsDirectOutputEncode() const;  // default: false
virtual StageResult runStage(const EnhancementStage& stage, std::string& error) = 0;
virtual StageResult processVideoFile(
    const EnhancementStage& stage,
    const std::string& inputVideo, const std::string& outputVideo,
    const FrameProgressCb& progressCb, std::string& error,
    const ProcessVideoOptions& opts = {}) = 0;
```

`StageResult` values: `Processed` (AI done, skip FFmpeg filter), `Deferred` (fall back to FFmpeg), `Error`, `Cancelled`.

`ProcessVideoOptions` carries: preview duration, frame preview callback, cancel/pause atomics, direct-output-encode flag, and output encode settings (codec, CRF, preset, threads).

## Filter Catalog

`filter_catalog.hpp` defines an embedded library of GLSL shaders and VapourSynth scripts. Filters ship as source strings inside the binary. Users toggle filters and tweak exposed parameters via the GUI filter browser. Backends stitch selected filters into the processing pipeline.

Key types: `EmbeddedFilter` (source + params), `ActiveFilter` (user selection with param overrides), `FilterRuntime` (`Glsl`/`VapourSynth`), `FilterCategory`.

## Model Catalog and Storage

Models in `~/.local/share/ave/models/`:

| Directory | Contents |
| --- | --- |
| `downloaded/` | Raw ONNX / PyTorch source files |
| `migraphx/` | MiGraphX `.mxr` compiled programs |
| `optimised/` | Hardware-optimised compiled programs |

`ModelEntry` metadata includes: `id`, `displayName`, `stage`, `capabilities`, `familyId`, `sourceFormat` (`Onnx`/`NcnnBin`/`Pytorch`), `precision` (`Fp32`/`Fp16`/`Int8`), `scale`, `fpsMul`, `downloadUrl`, `minVramMib`, optional `migraphxCompileWidth`/`migraphxCompileHeight`, and optional `controlInputBindings` for fused models.

Key helpers: `builtinModelCatalog()`, `catalogEntriesForStage()`, `preferredBackendModelForStage()`, `modelSupportsBackend()`.

## Naming Conventions

- **Headers**: `#pragma once` guards, `ave/xxx.hpp` paths
- **Namespace**: All code in `namespace ave { ... }`
- **Types**: PascalCase (`StageKind`, `EnhancementStage`, `PipelinePlanner`)
- **Functions/Methods**: camelCase (`parseCli`, `runStage`, `buildUi`)
- **Private members**: `name_` suffix (`inputPathEdit_`, `impl_`)
- **Enums**: `EnumClass` with PascalCase values (`StageKind::RestoreCompression`)
- **Internal helpers**: Anonymous namespace `namespace { ... }`
- **Sub-namespaces**: `ave::obs` (observability), `ave::frame_io` (frame I/O), `ave::process_progress`

## Code Patterns

### Error handling

- Output string parameters: `bool doSomething(std::string& error)`
- `std::optional<T>` for fallible value returns
- `InferenceError` factory helpers for structured cross-boundary errors
- Availability checks: `bool isAvailable(std::string& reason) const`

### Conditional compilation

Backend code uses preprocessor guards. The `#else` branch must report unavailability via a real error path, not a stub:

```cpp
#ifdef AVE_HAVE_NCNN
#  include <ncnn/ncnn/net.h>
#endif

#ifdef AVE_HAVE_NCNN
    // Full implementation
#else
    // Explicitly report unavailability via a real error path
#endif
```

Define ordering: `AVE_HAVE_CURL`, `AVE_HAVE_MIGRAPHX`, `AVE_HAVE_HIP`, `AVE_HAVE_ONNXRUNTIME_ROCM`, `AVE_HAVE_NCNN`, `AVE_HAVE_VULKAN`, `AVE_HAVE_ROCTX`.

### PIMPL pattern

Backends and I/O classes use PIMPL to hide implementation details:

```cpp
class VulkanVideoReader {
  public:
    VulkanVideoReader();
    ~VulkanVideoReader();
  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
```

### Header template

```cpp
#pragma once

#include <string>
#include "ave/types.hpp"

namespace ave {

class MyClass {
  public:
    explicit MyClass();
    ~MyClass() = default;
  private:
    int value_ = 0;
};

}  // namespace ave
```

## Testing

Tests use simple assertion-based approach (no framework). Each test file is a standalone executable registered with CTest:

```cpp
#include <cassert>
#include "ave/planner.hpp"

namespace {
void testSomething() {
    PipelinePlanner planner;
    const auto result = planner.plan({...});
    assert(!result.empty());
}
}  // namespace

int main() {
    testSomething();
    return 0;
}
```

Some tests link against `ave_core` for integration coverage. Others compile only the specific `.cpp` files they need to keep dependencies minimal and test boundaries explicit.

Test executables (17 total):
`planner_tests`, `tensor_contract_tests`, `runtime_diagnostics_tests`, `runtime_paths_tests`, `model_manager_profile_tests`, `video_probe_tests`, `process_loop_tests`, `process_observer_tests`, `process_progress_tests`, `rgb_video_loop_tests`, `frame_io_vulkan_tests`, `frame_io_pipe_tests`, `frame_io_direct_pack_tests`, `ffmpeg_runner_tests`, `job_recovery_tests`, `job_queue_tests`, `telemetry_tests`, `migraphx_backend_tests`, `interop_bridge_tests`

## Qt GUI Notes

- Requires Qt 6.2+ with Widgets module
- Uses `Q_OBJECT` macro for signal/slot
- CMake handles MOC/UIC/RCC automatically
- GUI files in `src/gui/` with local includes
- `filter_browser` — embedded filter browsing and parameter adjustment
- `model_manager_dialog` — model download/management UI
- `settings_dialog` — application settings
- `main_window` — primary window with pipeline configuration

## Important Gotchas

1. **Stage ordering is automatic**: User-specified order is ignored; `PipelinePlanner` reorders stages deterministically. Tests verify this behavior.

2. **Backends gracefully degrade**: Missing libraries don't cause build failures; features are disabled via CMake checks. At runtime, backends report unavailability and the pipeline falls back.

3. **FFmpeg required at runtime**: `ffmpeg` and `ffprobe` must be in PATH for all operations. FFmpeg libraries (`libavcodec`, `libavformat`, `libavutil`, `libavfilter`, `libswscale`) are required at build time.

4. **Model path resolution is backend-aware**: MiGraphX consumes `.mxr` compiled artifacts; non-MiGraphX backends get the original source model. `resolveModelPath()` in `video_processor.cpp` handles this split. Never hand a `.mxr` artifact to a non-MiGraphX backend.

5. **`backendProcessed` flag on stages**: When a backend returns `StageResult::Processed`, the stage's `backendProcessed` is set to `true`. The FFmpeg encode pass checks this flag to avoid double-applying filters.

6. **Thread safety**: Backend implementations use `std::mutex` for model loading; GUI uses `std::atomic<bool>` for cancel/pause flags (`cancelFlag`, `pauseFlag` on `VideoJob`). Ctrl+C handler uses `std::atomic<bool>` — second SIGINT force-exits.

7. **Warning flags are strict by default**: `AVE_STRICT_WARNINGS=ON` enables `-Wconversion -Wsign-conversion` which catch subtle type issues.

8. **`AVE_HAVE_ONNXRUNTIME_ROCM` defaults to ON** but is separate from `AVE_HAVE_HIP`. The ROCm/HIP backend needs both.

9. **Progress reporting is two-phase**: 5% for backend pre-load, 95% for FFmpeg encode. `JobProgressCb` takes `(overallPct, taskPct, msg)`.

10. **Script backends (GLSL, VapourSynth) defer all work to encode phase**: Their `runStage()` returns `Deferred`; actual processing happens inside `FfmpegRunner::encode()`. Catalog filters are passed via `dynamic_cast` to the backend after initialization.

11. **Artifact manifests for `.mxr` caching**: Compiled artifacts are validated against a manifest containing ROCm version, GPU arch, source fingerprint, precision, and env vars. Mismatch triggers recompilation.

12. **`-march=native -O3`** is applied automatically to `ave_core` and `ave_model_compile_sweep` on x86 (via `ave_add_native_optimizations()`).

13. **Portable bundles use wrapper scripts**: Installed binaries live in `libexec/ave/`; shell wrappers in `bin/` set `LD_LIBRARY_PATH` for runtime resolution.

## C++ Standard

C++20 (required). No extensions (`CMAKE_CXX_EXTENSIONS OFF`). `std::filesystem`, `std::variant`, `std::optional` used throughout.
