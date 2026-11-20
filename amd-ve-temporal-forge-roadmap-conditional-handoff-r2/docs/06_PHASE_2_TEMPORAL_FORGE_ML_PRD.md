# PRD — Phase 2: Temporal Forge ML

## Product thesis

Temporal Forge ML is an AMD-first, clean-room, video-native temporal reconstruction path.

It should adapt the conceptual strengths of FSR-class temporal upscaling to ordinary offline video by generating the buffers games normally provide:

- motion vectors
- proxy depth
- reactive masks
- transparency/composition masks
- exposure/luminance data
- scene-cut/reset signals
- confidence maps

Then it should use AI only where temporal reconstruction lacks evidence.

## Why this exists

Single-frame AI enhancement can produce plausible detail, but it may hallucinate. Temporal reconstruction can recover accurate detail from neighboring frames when real evidence exists across time.

Temporal Forge ML should follow this rule:

> Temporal first. AI assist. Never AI dominate by default.

## Goals

1. Improve accuracy over spatial-only AI enhancement.
2. Reduce shimmer and temporal inconsistency.
3. Recover subpixel detail from motion when possible.
4. Avoid AI hallucination in high-confidence regions.
5. Make failure modes inspectable through overlays.
6. Preserve an AMD-first path without making the engine impossible to port.

## Non-goals

- Do not claim to be FSR4 unless AMD grants explicit permission.
- Do not depend on redacted/leaked internals.
- Do not make this a hidden mode that users cannot understand.
- Do not discard existing MiGraphX/model infrastructure.
- Do not replace Phase 1 model families immediately.

## User-facing mode name

**Temporal Forge**

Suggested modes:

| Mode | Behavior |
|---|---|
| Accurate | temporal-heavy, minimal AI |
| Balanced | temporal base + moderate AI refinement |
| Recover | stronger AI where evidence is weak |
| Anime / Line Art | edge/line preservation |
| Film Preserve | protect grain and natural texture |
| Clean Digital | stronger denoise/deblock, less grain |

## Core user story

As a user, I want AMD-VE to use nearby frames to reconstruct accurate detail, then use AI only where the video lacks recoverable evidence.

Acceptance:

- User can preview temporal-only, AI-only, and hybrid comparison.
- User can see confidence overlays.
- User can adjust temporal-vs-AI behavior.
- Scene cuts reset history.
- Text/subtitles are protected.

## High-level pipeline

```text
source frames
  → optional pre-clean
  → motion estimation
  → proxy depth / edge discontinuity
  → reactive/confidence/occlusion masks
  → temporal cache
  → temporal reconstruction
  → confidence-guided AI refiner
  → final sharpen/color/export
```

## Core blending principle

```text
final =
  temporal_result * temporal_confidence
+ ai_refinement   * ai_allowed_mask
+ original_guard  * preservation_mask
```

## Required outputs

- Enhanced video frame
- Debug overlays
- Metrics per segment
- Cache products for preview
- Failure markers in timeline

## MVP definition

Temporal Forge MVP is **not** a full ML model yet.

MVP must include:

- temporal packet model
- motion provider
- proxy depth provider
- scene cut reset
- confidence map
- non-ML temporal baseline
- preview overlays
- masked existing-model refiner

Only after this baseline exists should custom ML training begin.

## Full v1 definition

Temporal Forge v1 includes:

- AMD-first reconstruction backend
- confidence-guided ML refiner
- training/eval pipeline
- reproducible benchmark set
- preview integration
- advanced debug overlays
- packaging flags
- fallback path for systems without the hero backend

## Success criteria

Temporal Forge v1 should beat:

1. spatial-only model output on temporal stability
2. temporal-only baseline on low-confidence/missing-detail regions
3. naive AI output on hallucination/flicker risk

## Failure conditions

Stop/rollback if:

- text smears regularly
- scene cuts ghost
- fast motion creates unacceptable trails
- output quality cannot beat current model pipeline on benchmark clips
- performance makes preview unusable
- implementation depends on legally uncertain code

## Phase 2 feasibility envelope

Temporal Forge is research until it fits inside a measurable envelope. A prototype cannot be called successful because it merely renders frames.

### Target hardware tiers

| Tier | Hardware target | Purpose | Required report fields |
|---|---|---|---|
| H0 reference CPU | CPU-only or low-end fallback | correctness/debug only | runtime/frame, thread count, clip, resolution |
| H1 AMD baseline | RDNA2/RDNA3-class AMD GPU where available | broad AMD-user sanity | GPU model, driver, backend, VRAM use |
| H2 AMD preferred | Radeon 7900-class / RDNA3 high-end | first serious AMD-VE target | GPU model, ROCm/Vulkan path, VRAM/system RAM |
| H3 future AMD | RDNA4/Radeon RX 9000-class where available | Redstone-adjacent investigation only | source of support, SDK/API path, ML feature availability |

### Clip and resolution envelope

| Prototype class | Source duration | Source resolution | Working output | Goal |
|---|---:|---:|---:|---|
| unit fixture | 8-32 frames | synthetic | native | prove math and scene reset |
| preview smoke | 3-5 seconds | 480p/720p | 720p/1080p | prove preview usability |
| stress preview | 5-10 seconds | 1080p | 1080p/1440p | measure memory and cancel behavior |
| demo candidate | 10-30 seconds | 720p/1080p | 1080p/1440p/4K | only after metrics beat baselines |

### Initial performance budgets

These are prototype budgets, not marketing claims.

| Measure | Green | Yellow | Red / stop condition |
|---|---:|---:|---:|
| Preview startup latency for 3-5s segment | < 5s | 5-15s | > 15s without progress feedback |
| Runtime per frame at 720p preview | < 250ms | 250-750ms | > 750ms unless marked reference-only |
| Runtime per frame at 1080p preview | < 500ms | 500-1500ms | > 1500ms unless marked reference-only |
| VRAM use on H2 target | < 8GB | 8-12GB | > 12GB without tiling/fallback plan |
| System RAM use on H2 target | < 16GB | 16-24GB | > 24GB for short preview segment |
| Text/subtitle protection failures | rare and documented | recurring but avoidable | regular smearing/ghosting |
| Scene-cut ghosting | none in fixtures | isolated with warning | recurring across ordinary clips |

### Required prototype report

Every Temporal Forge prototype report must include:

- commit hash
- backend path
- GPU/CPU/RAM/driver stack
- clip ID and license/provenance
- source duration/resolution/frame rate
- output resolution
- runtime/frame
- preview startup latency
- peak VRAM and system RAM if available
- temporal-only vs spatial-only vs hybrid comparison
- failure cases and debug artifact paths

### Stop criteria

Stop and re-scope before adding more ML if:

1. temporal-only baseline cannot beat spatial-only on any meaningful temporal-consistency metric,
2. scene cuts or text protection fail on basic fixtures,
3. preview performance misses red thresholds without a clear fallback,
4. debug artifacts cannot explain failures,
5. implementation requires legally uncertain source.
