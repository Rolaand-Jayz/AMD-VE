# File Map

## Purpose

This file maps the current repo to the AMD-VE to Temporal Forge handoff work.

It must not invent files, APIs, classes, or seams.

## Repo Entry Points

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| CLI entry point | `src/main.cpp` (86 lines) | `main()` | Parses args via `parseCli()`, dispatches to `VideoProcessor` |
| CLI arg parsing | `src/cli.cpp` (291 lines), `include/ave/cli.hpp` | `parseCli()` | `--input`, `--output`, `--backend`, `--stage`, `--codec`, `--preview`, `--dry-run`, `--list-backends` |
| GUI entry point | `src/gui/main_gui.cpp` (125 lines) | `main()` | Creates `MainWindow`, enters Qt event loop |
| GUI main window | `src/gui/main_window.cpp` (4395 lines), `src/gui/main_window.hpp` | `MainWindow` | QMainWindow subclass; stage selection, profile save/load, settings, model management, queue/preview flows |

## Model Management

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| Model lifecycle | `src/model_manager.cpp` (3445 lines), `include/ave/model_manager.hpp` (189 lines) | `ModelManager` | Download, prepare ONNX, compile MiGraphX `.mxr`, validate manifests, artifact fingerprinting |
| Model catalog/manifest | `src/model_catalog.cpp` (1143 lines), `include/ave/model_catalog.hpp` (193 lines) | `ModelCatalog` | Parses `.avemodel` manifests; metadata, capabilities, formats, precision |
| Model states | `include/ave/model_manager.hpp` | `ModelState` enum | NotDownloaded → Downloading → Downloaded → Converting → Converted → Error |

## Backend Selection

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| Backend interface | `include/ave/backend.hpp` (112 lines), `src/backend.cpp` | `IAcceleratorBackend` | Virtual: `type()`, `name()`, `isAvailable()`, `initialize()`, `runStage()`, `processVideoFile()` |
| Backend manager | `src/backend_manager.cpp` (219 lines), `include/ave/backend_manager.hpp` (19 lines) | `BackendManager::createBackend()` | Probes: MiGraphX → ROCm/HIP → Vulkan → NCNN |
| Backend type enum | `include/ave/types.hpp` (24 lines) | `BackendType` | MiGraphX, RocmHip, VulkanCompute, NcnnVulkan |

## MiGraphX Path

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| MiGraphX backend | `src/backends/migraphx_backend.cpp` (5528 lines) | `MigraphxBackend` (PIMPL) | Gold-standard: artifact validation, runtime fingerprinting, tile planning, tensor contracts |
| Precision handling | `src/backends/migraphx_backend.cpp:88` | `MiGraphXPrecision` enum, `compilePrecisionTag()`, `modelCompilePrecision()` | Fp32, Fp16, Int8 |
| Runtime env config | `src/backends/migraphx_backend.cpp:483` | `applyDefaultMiGraphXProcessEnv()` | Sets MIGRAPHX_ENABLE_NHWC, MIGRAPHX_ENABLE_CK |
| Artifact discovery | `src/backends/migraphx_backend.cpp:222` | `hasAnyMiGraphXArtifact()` | Checks bundled + system paths |
| Version detection | `src/backends/migraphx_backend.cpp:269` | `getMiGraphXVersion()` | Used for manifest key construction |
| Capability tracking | `src/backends/migraphx_backend.cpp:2059–2127` | `parseStageKindsCsvLocal()`, `stageControlScopes()`, `stageCapabilityRequested()` | `model_capabilities` and `fused_requested_capabilities` parameters — partial fusion surface |
| Process video file | `src/backends/migraphx_backend.cpp:4947` | `processVideoFile()` | Frame-by-frame MiGraphX inference with encode session |

## Vulkan / Preview Path

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| Vulkan compute backend | `src/backends/vulkan_compute_backend.cpp` (1313 lines) | `VulkanComputeBackend` (PIMPL) | Embedded GLSL compute shaders, Vulkan pipeline management |
| Vulkan runtime | `src/vulkan_runtime.cpp`, `include/ave/vulkan_runtime.hpp` (50 lines) | `VulkanRuntime` | Vulkan device management |
| Frame I/O (Vulkan) | `src/frame_io_vulkan.cpp` (1002 lines), `include/ave/frame_io.hpp` (468 lines) | `frame_io::RgbVideoSourceMode` | RawPipe, VulkanTransfer, VulkanHardware modes |
| Vulkan↔HIP bridge | `src/interop_bridge.cpp` (375 lines), `include/ave/interop_bridge.hpp` (155 lines) | `InteropBridge` | Vulkan external memory/semaphore import into HIP |
| Preview (CLI) | `src/cli.cpp` | `--preview`, `--preview-duration` | Duration-limited encode; no cache, no timeline |
| Preview cache | — | not found | Does not exist. Phase 1C target. |
| Timeline UI | — | not found | Does not exist. Phase 1C target. |

## CLI Surface

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| Stage taxonomy | `include/ave/types.hpp`, `src/types.cpp` | `StageKind` enum | RestoreCompression, RemoveArtifacts, Denoise, Deblur, Dehalo, ColorFix, Upscale, Sharpen, Stereo3D, Interpolate |
| Stage parameters | `include/ave/stage.hpp` (54 lines), `src/stage.cpp` (290 lines) | `EnhancementStage`, `ParameterValue` | Type-safe parameter extraction |
| Stage planner | `src/planner.cpp` (42 lines), `include/ave/planner.hpp` (14 lines) | `PipelinePlanner::plan()` | Deterministic group-based ordering |
| Filter catalog | `src/filter_catalog.cpp`, `include/ave/filter_catalog.hpp` (103 lines) | `FilterCatalog` | FFmpeg filter definitions per stage |
| CLI help | `src/cli.cpp` | Usage output | Backends, stages, codec/CRF/preset, preview, dry-run |
| Profile serialization | — | not found | Does not exist. Phase 1E target. |

## GUI Surface

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| Main window | `src/gui/main_window.cpp` (4395 lines) | `MainWindow` | Stage controls, filter browser, profile save/load, settings |
| Filter browser | `src/gui/filter_browser.cpp` (398 lines), `src/gui/filter_browser.hpp` | `FilterBrowser` | Filter browsing widget |
| Model manager dialog | `src/gui/model_manager_dialog.cpp` (496 lines), `src/gui/model_manager_dialog.hpp` | `ModelManagerDialog` | Model download/management dialog |
| Settings dialog | `src/gui/settings_dialog.cpp` (552 lines), `src/gui/settings_dialog.hpp` | `SettingsDialog` | App settings persistence (QSettings) |
| Toggle switch | `src/gui/toggle_switch.cpp` (122 lines), `src/gui/toggle_switch.hpp` | `ToggleSwitch` | Custom toggle widget |
| Resources | `src/gui/ave_gui_resources.qrc` | Qt resource file | Icons/assets |

## Tests

| Area | File | Real symbol/class/function | Notes |
|---|---|---|---|
| Planner tests | `tests/planner_tests.cpp` | assertion-based | Stage ordering verification |
| Tensor contract tests | `tests/tensor_contract_tests.cpp` | assertion-based | Tensor shape/type validation |
| Runtime diagnostics | `tests/runtime_diagnostics_tests.cpp` | assertion-based | Platform probing |
| Model manager profiles | `tests/model_manager_profile_tests.cpp` | assertion-based | Model lifecycle (15.97s — longest test) |
| Video probe | `tests/video_probe_tests.cpp` | assertion-based | Media metadata |
| Frame I/O (pipe) | `tests/frame_io_pipe_tests.cpp` | assertion-based | Pipe-based frame transfer |
| Frame I/O (Vulkan) | `tests/frame_io_vulkan_tests.cpp` | assertion-based | Vulkan frame transfer |
| Frame I/O (direct pack) | `tests/frame_io_direct_pack_tests.cpp` | assertion-based | Direct frame packing |
| FFmpeg runner | `tests/ffmpeg_runner_tests.cpp` | assertion-based | FFmpeg orchestration |
| MiGraphX backend | `tests/migraphx_backend_tests.cpp` | assertion-based | Backend behavior (10.29s) |
| Interop bridge | `tests/interop_bridge_tests.cpp` | assertion-based | Vulkan↔HIP bridge |
| Process loop | `tests/process_loop_tests.cpp` | assertion-based | Frame processing loop |
| Process observer | `tests/process_observer_tests.cpp` | assertion-based | Subprocess management |
| Process progress | `tests/process_progress_tests.cpp` | assertion-based | Progress tracking |
| RGB video loop | `tests/rgb_video_loop_tests.cpp` | assertion-based | RGB frame iteration |
| Job recovery | `tests/job_recovery_tests.cpp` | assertion-based | Crash recovery |
| Job queue | `tests/job_queue_tests.cpp` | assertion-based | Job queuing |
| Telemetry | `tests/telemetry_tests.cpp` | assertion-based | rocm-smi telemetry |
| Runtime paths | `tests/runtime_paths_tests.cpp` | assertion-based | Path resolution |

## Packaging / Release Docs

| Area | File | Purpose | Notes |
|---|---|---|---|
| Package builder | `tools/build_native_packages.py` | Native package generation for Arch, Ubuntu, Debian, Fedora, openSUSE, Rocky, Alma | Not validated on non-Arch systems |
| CMake install templates | `cmake/*.cmake.in`, `cmake/*.sh.in` | Install/runtime deployment templates | Bundled app-private runtime payloads |
| Release status | `docs/RELEASE_STATUS.md` | Current release state documentation | Consistent beta prerelease wording |
| Support tiers | `docs/SUPPORT_TIERS.md` | Explicit support promise | Four-tier system |
| Limitations | `docs/LIMITATIONS.md` | Known limitations | Honest, blunt |
| Validation/evidence | `docs/VALIDATION_AND_EVIDENCE.md` | Evidence tier definitions | Maintainer-verified vs community-submitted |
| Benchmarks | `docs/BENCHMARKS.md` | Maintainer-verified benchmarks | Scoped to reference system |
| Packaging doc | `docs/PACKAGING.md` | Packaging process and scope | Reach ≠ verified compatibility |
| Releasing doc | `docs/RELEASING.md` | Release process and checks | Beta prerelease workflow |
| Gold standard | `docs/GOLD_STANDARD_FOR_IMPLEMENTATION.md` | Honest implementation reference | "No fantasy football" edition |
| Contributing | `CONTRIBUTING.md` | Contributor guide | Beta prerelease notice, support boundary |
| Issue templates | `.github/ISSUE_TEMPLATE/` | Bug report, feature request, benchmark, experimental distro, config | Support tier link in config.yml |

## Missing or Unclear Seams

| Needed seam | Current finding | Impact | Required next action |
|---|---|---|---|
| Mode definition schema + loader | not found | Phase 1A cannot start without it | Import R2 schemas; implement `ModeDefinition` loader |
| Model family metadata + loader | not found | Phase 1A cannot start without it | Import R2 schemas; implement `ModelFamily` loader |
| Fused stage planner | not found | Phase 1B target | Add `FusionPlanner` class that operates on ordered pipeline output |
| Preview cache manager | not found | Phase 1C target | New `PreviewCacheManager` with disk-based cache + manifest |
| Timeline widget | not found | Phase 1C target | New Qt widget for frame scrubbing |
| Workstation UI layout | not found | Phase 1D target | Restructure MainWindow into panels |
| Portable profile format | not found | Phase 1E target | JSON profile format with `--profile` CLI flag |
| Temporal engine abstractions | not found | Phase 2A target (experimental only) | New interfaces: `TemporalEngine`, `BufferProvider`, `MaskGenerator` |
