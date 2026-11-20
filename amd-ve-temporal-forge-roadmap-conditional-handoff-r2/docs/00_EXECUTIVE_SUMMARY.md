# Executive Summary

## Product direction

AMD-VE should evolve from an AMD-first AI video enhancer into a **preview-first restoration workstation** with a flagship temporal reconstruction path.

The work should be split into two deliberate phases:

1. **Phase 1 — Refined AMD-VE**
   - redesign the app around visual inspection and confidence
   - reduce visible model clutter
   - enable fused family enhancements by default
   - make preview large, interactive, and timeline-driven
   - harden the existing AMD-first backend/model infrastructure

2. **Phase 2 — Temporal Forge ML**
   - build a clean-room FSR-class video reconstruction engine
   - synthesize the game-engine buffers that FSR-style algorithms normally require
   - use temporal evidence first and AI only where temporal evidence is weak
   - start AMD-first, then preserve a hardware-agnostic architecture

## Why this matters

Most video enhancers expose too many models and rely too heavily on single-frame or weakly temporal AI inference. This creates plausible detail, but not always accurate detail.

The proposed hero path is different:

> Temporal reconstruction should recover truth from neighboring frames. AI should refine uncertainty, not repaint the whole image.

That design can position AMD-VE as something more than a free/open alternative to existing tools. It becomes a new category: **offline video temporal reconstruction from ordinary video, using generated motion/depth/reactive/confidence buffers.**

## Phase 1 outcome

At the end of Phase 1, AMD-VE should feel polished, smaller, and more intentional.

The user experience should revolve around:

- one large central viewer
- split-wipe comparison between original and enhanced frames
- seekable timeline with rendered/cache status
- simple purpose-based enhancement modes
- model families instead of raw model chaos
- one-pass fused enhancements wherever the selected family supports it
- advanced/legacy access for experts without burdening normal users

## Phase 2 outcome

At the end of Phase 2, AMD-VE should have a new hero path:

**Temporal Forge ML**

A clean-room, AMD-first, video-native temporal reconstruction engine that:

- estimates motion from ordinary video
- creates proxy depth from the existing 2D→3D/depth system
- detects scene cuts, occlusions, reactive regions, subtitles/UI/text, and grain
- builds a temporal cache
- reconstructs high-resolution output through time
- applies AI refinement only where the confidence system permits it

## Current repo strengths to preserve

- Linux-first C++20 application core
- CLI + Qt GUI sharing the same backend pipeline
- deterministic stage planner
- FFmpeg as media spine
- ModelManager artifact discipline
- MiGraphX-first AMD inference path
- ROCm/HIP, Vulkan, NCNN, FFmpeg fallback layers
- custom model manifest support
- fused capability metadata
- packaging and diagnostic tooling

## Current repo areas to reduce or refactor

- Main UI model sprawl
- Wizard/control-panel feeling
- Small preview area
- Separate model entries presented as equal user choices
- Backend complexity exposed too directly to the user
- Redundant/deprecated model variants in the main workflow
- Any future Phase 2 implementation that tries to force FSR/temporal reconstruction into the current `StageKind::Upscale` only

## Strategic licensing/IP posture

AMD-VE can and should remain open source. The safest mainline path is clean-room and public-source-based. Official AMD SDKs may be used where their licensing/platform constraints fit. Accidentally published or redacted FSR4 materials should not become a public dependency without AMD permission or legal review.

The best AMD-facing posture is collaborative:

> AMD-VE is an AMD-first open-source creator tool that showcases AMD hardware, ROCm/MiGraphX, Vulkan, and FidelityFX-inspired temporal reconstruction for video.

## Recommended north-star slogan

> AMD-VE does not merely upscale frames. It reconstructs video through time.
