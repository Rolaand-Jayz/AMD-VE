# Buffer Providers and Masks

## Purpose

FSR-style temporal reconstruction expects buffers that games normally provide. Ordinary video does not have those buffers. Temporal Forge must synthesize them.

## Provider registry

```text
BufferProviderRegistry
├── MotionProvider
├── DepthProvider
├── ReactiveMaskProvider
├── OcclusionProvider
├── ConfidenceProvider
├── ExposureProvider
└── SceneResetProvider
```

## MotionProvider

### Inputs

- current frame
- previous frame
- optional future frame
- frame timing
- current/precleaned luma

### Outputs

- current-to-previous motion vectors
- motion confidence
- scene-change hint if provider supports it

### Providers

| Provider | Priority | Notes |
|---|---:|---|
| AMD FidelityFX Optical Flow | 1 | AMD-first, block-based, scene-change signal |
| RIFE/flow-model provider | 2 | useful fallback / higher detail if available |
| CPU block matcher | 3 | slow reference path |

### Requirements

- Handle scene cuts.
- Expose confidence.
- Normalize vector scale.
- Document coordinate convention.

## DepthProvider

### Inputs

- current frame
- optional temporal frames
- existing 2D→3D depth/disparity model output

### Outputs

- proxy depth
- edge/discontinuity map
- depth confidence

### Important rule

Depth does not need to be metric. It needs to help with:

- edges
- occlusion
- disocclusion
- foreground/background separation
- history safety

## ReactiveMaskProvider

### Purpose

Mark regions where current-frame data should dominate over history.

### Common reactive regions in video

- subtitles
- UI overlays
- scene cuts
- flashing lights
- specular highlights
- fast moving objects
- particles/smoke/water
- motion-vector failure zones

## OcclusionProvider

### Purpose

Detect where history is not safe because a region was hidden in previous frames or newly revealed.

Inputs:

- motion vectors
- proxy depth
- forward/backward consistency
- color difference

Outputs:

- occlusion mask
- disocclusion mask

## ConfidenceProvider

### Purpose

Compute how much the engine should trust temporal reconstruction.

Inputs:

- motion confidence
- depth confidence
- occlusion mask
- scene-cut state
- local color/luma error
- temporal variance
- text/grain protection masks

Outputs:

- temporal confidence map
- AI allowed mask
- preservation mask

## ExposureProvider

### Purpose

Provide luminance/exposure stability for temporal reconstruction.

Outputs:

- average luminance
- histogram
- exposure scalar
- flicker marker

## SceneResetProvider

### Reset conditions

- hard scene cut
- large histogram shift
- missing frames
- seek jump
- user starts a new preview segment without warmup
- resolution/format change
- model/settings change

## AI allowed mask

AI should be allowed where:

- temporal confidence is low
- compression damage is high
- blur is high
- disocclusion occurs
- detail is unrecoverable from neighboring frames

AI should be reduced or blocked where:

- text/subtitles/UI are detected
- temporal confidence is high
- edges are stable
- grain preservation is enabled
- scene cut just occurred

## Debug overlays

Required overlays:

- motion vectors / motion confidence
- proxy depth
- reactive mask
- occlusion/disocclusion
- temporal confidence
- AI allowed mask
- ghost-risk map

## Acceptance tests

- Scene cuts reset history.
- Subtitle/text masks reduce AI and temporal smear.
- Motion confidence drops on flow failure.
- Proxy depth edges align reasonably with foreground/background boundaries.
- AI allowed mask is not uniform across the frame except in explicit debug/test cases.
