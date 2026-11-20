# Backend, Data Flow, and Stage System Spec

## Problem

The existing stage system is good for linear enhancement passes. Temporal Forge needs multi-frame packet processing, history, side buffers, and debug outputs. Do not force the entire hero path into a normal `StageKind::Upscale` model pass.

## Existing useful pieces

- `StageKind` taxonomy already includes cleanup, upscale, sharpen, stereo 3D, and interpolation.
- Planner already gives deterministic ordering.
- Backends already implement `runStage` and `processVideoFile` patterns.
- Model catalog already supports capabilities/fusion.
- FFmpeg already acts as the media spine.

## Required new concepts

### New stage kind or mode

Add one of the following:

Preferred:

```cpp
enum class EnhancementMode {
    StandardPipeline,
    TemporalForge
};
```

Alternative:

```cpp
StageKind::TemporalReconstruct
```

Recommendation: use **mode + internal temporal plan**, not only a new stage. Temporal Forge is a compound pipeline, not a simple stage.

### Temporal plan

```cpp
struct TemporalPlan {
    PrecleanPolicy preclean;
    MotionPolicy motion;
    DepthPolicy depth;
    MaskPolicy masks;
    ReconstructionPolicy reconstruction;
    RefinerPolicy refiner;
    OutputPolicy output;
};
```

### Pipeline hash

Every preview/export must generate a deterministic hash from:

- input fingerprint
- mode
- model/family ids
- parameters
- backend choices
- output resolution
- temporal settings
- relevant runtime versions

## Planner change

Add a planner layer:

```text
User intent
  → mode resolver
  → family resolver
  → deterministic stage planner
  → fused family group planner
  → temporal plan builder if Temporal Forge enabled
```

## Backend change

Existing backends remain.

Add:

```text
TemporalReconstructionBackend
TemporalMotionProvider
TemporalDepthProvider
TemporalMaskProvider
MLRefinerBackend adapter
```

Do not make MiGraphX responsible for non-ML temporal reconstruction. MiGraphX should be the refiner/inference backend, not the entire temporal engine.

## Frame loop change

Standard pipeline:

```text
frame → stage backend → frame
```

Temporal Forge pipeline:

```text
frame window → packet providers → temporal backend → refiner backend → frame
```

## Preview/export consistency

Preview and full render must share the same plan builder.

Differences allowed:

- preview may use lower resolution
- preview may use shorter warmup
- preview may cache intermediate overlays

Differences not allowed:

- preview uses a different algorithm by accident
- preview ignores masks that export uses
- export lacks settings used in preview

## Backend priority

Phase 1 standard pipeline remains:

1. MiGraphX
2. ROCm/HIP ONNX Runtime
3. Vulkan Compute
4. NCNN Vulkan
5. FFmpeg fallback

Temporal Forge priority:

1. AMD Golden Path: FidelityFX-inspired motion + Vulkan/compute reconstruction + MiGraphX refiner
2. Vulkan portable reconstruction + available refiner
3. CPU/reference reconstruction + available refiner
4. fallback to standard pipeline

## Direct FSR/Redstone integration decision

Official newest AMD FSR SDK 2.x / Redstone path should be evaluated but not assumed as directly usable for Linux-native AMD-VE because current public positioning is DirectX 12 / Unreal/RDNA4 focused. If AMD grants permission or provides guidance, create a separate backend. Otherwise build clean-room Temporal Forge.

## Acceptance tests

- Standard pipeline behavior remains unchanged when Temporal Forge is disabled.
- Temporal Forge mode can be enabled without breaking CLI/GUI startup.
- Pipeline hashes change when temporal settings change.
- Preview and export resolve to equivalent plans.
- Fallback to standard pipeline is explicit and logged.
