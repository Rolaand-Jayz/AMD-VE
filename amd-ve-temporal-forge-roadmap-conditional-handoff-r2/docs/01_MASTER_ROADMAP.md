# Master Roadmap

## Roadmap overview

The roadmap is split into two major programs.

```text
Program A: Refined AMD-VE
  Goal: make the app clean, usable, confidence-building, and model-family-driven.

Program B: Temporal Forge ML
  Goal: build the FSR-class video-native temporal reconstruction hero path.
```

These programs must not be implemented in parallel as one giant rewrite. Phase 1 creates the product surface and architecture discipline that Phase 2 needs.

---

# Phase 0 — Baseline audit and safety rails

## Objective

Create a clean starting point before changing UI, models, or pipeline behavior.

## Required tasks

1. Record current repo state:
   - commit hash
   - default branch
   - build options
   - tested hardware
   - tested OS
   - available backends
   - model catalog count
   - current GUI screenshots

2. Run baseline validation:
   - full build
   - unit tests
   - CLI dry-run smoke test
   - GUI launch smoke test
   - one short video preview job
   - one MiGraphX model compile/reuse check

3. Document known limitations:
   - unsupported backends
   - broken models
   - model compile failures
   - packaging limitations
   - GUI rough edges

## Exit criteria

- `BASELINE_AUDIT.md` exists.
- All failing tests are categorized as either blocker, known limitation, or unrelated existing debt.
- No redesign work starts until baseline status is recorded.

---

# Phase 1 — Refined AMD-VE redesign

## Objective

Turn AMD-VE from a broad model/stage control panel into a refined preview-first restoration workstation.

## Milestones

### M1.1 — New information architecture

Deliverables:

- Define app modes:
  - Restore Video
  - Upscale Video
  - Anime / Line Art
  - 2D→3D SBS
  - Interpolate / Smooth Motion
  - Advanced / Legacy
- Define model-family cards.
- Define advanced drawers for backend/model details.
- Remove raw model dropdowns from the primary workflow.

Exit criteria:

- Users can select intent before model.
- Expert model selection still exists, but is not the default path.

### M1.2 — Large preview workspace

Deliverables:

- Replace small Original/Processed preview labels with a single large comparison viewer.
- Add preview modes:
  - Split wipe
  - Side-by-side
  - Original only
  - Enhanced only
  - Freeze frame
- Add manual divider drag.
- Add auto-sweep divider.
- Add zoom/pan lock.

Exit criteria:

- A user can judge enhancement quality without exporting the full video.
- Preview occupies the visual center of the app.

### M1.3 — Cache-aware timeline

Deliverables:

- Seekable preview timeline.
- Mark-in / mark-out segment selection.
- Rendered-cache status lane.
- Preview segments playable as soon as frames exist.
- Playback speed limited by cached segment availability.

Exit criteria:

- User can render and preview a segment, scrub it, loop it, and compare it.
- Seeking outside rendered range gives a clear “render here” action.

### M1.4 — Model consolidation

Deliverables:

- Visible curated model families.
- Advanced/legacy model list.
- Default family selection rules.
- Model-family capability chips.
- Duplicate/legacy variants hidden from primary UI.

Exit criteria:

- Main workflow exposes no more than 5–7 visible families.
- Every visible family has a plain-language purpose.

### M1.5 — Fused enhancement behavior

Deliverables:

- All supported capabilities in a selected family are enabled by default.
- Fused-capable families run in one pass when valid.
- Strength sliders remain available.
- Planner collapses consecutive compatible stages into a fused group.

Exit criteria:

- A fused family can satisfy denoise + deblur + upscale in one AI invocation when manifest metadata permits.
- The user does not need to stack multiple stages manually for normal use.

### M1.6 — Product polish and release hardening

Deliverables:

- Settings persistence for new UI state.
- Profile save/load compatibility.
- Updated docs.
- Updated screenshots.
- Beta migration notes.
- Regression tests.

Exit criteria:

- Phase 1 can ship as a coherent beta release before Phase 2 lands.

---

# Phase 2 — Temporal Forge ML hero path

## Objective

Build a clean-room, video-native temporal reconstruction system inspired by FSR-class temporal reconstruction but not dependent on redacted internals.

## Milestones

### M2.1 — Temporal Forge placeholder and UX contract

Deliverables:

- Add `Temporal Forge` as a visible future mode.
- Add disabled/experimental flag gate.
- Define UI controls:
  - Accurate
  - Balanced
  - Recover
  - Anime / Line Art
  - Film Preserve
  - Clean Digital
- Define debug overlays:
  - motion confidence
  - proxy depth
  - reactive mask
  - occlusion mask
  - ghost-risk map
  - temporal cache status

Exit criteria:

- UI can represent the mode before the engine is complete.
- No fake quality claims.

### M2.2 — Temporal data model

Deliverables:

- Define `TemporalFramePacket`.
- Define buffer formats for:
  - source color
  - pre-clean color
  - motion vectors
  - proxy depth
  - reactive mask
  - occlusion mask
  - confidence map
  - exposure/luma stats
  - reconstructed output
- Define temporal cache ownership and lifetime.

Exit criteria:

- Backends can consume a consistent packet API.
- Timeline cache can display packet state.

### M2.3 — Motion provider

Deliverables:

- Implement provider interface.
- Implement first provider using AMD FidelityFX Optical Flow where technically feasible.
- Implement fallback provider using existing interpolation/flow model or CPU reference.
- Expose scene-change flag into cache reset logic.

Exit criteria:

- The engine can produce motion data for a short clip and visualize it.

### M2.4 — Depth/proxy-depth provider

Deliverables:

- Use existing 2D→3D/depth path as first proxy-depth provider.
- Normalize depth into reconstruction-friendly format.
- Add edge/discontinuity extraction.
- Add fallback edge-aware pseudo-depth when depth model is unavailable.

Exit criteria:

- Depth overlay is stable enough for edge/occlusion guidance.
- Pipeline does not depend on perfect metric depth.

### M2.5 — Reactive/confidence masks

Deliverables:

- Scene cut detection.
- Occlusion/disocclusion detection.
- Subtitle/UI/text protection.
- Grain/noise classifier.
- Motion failure confidence.
- Temporal confidence map.

Exit criteria:

- Engine can identify where history should be trusted, reduced, or reset.

### M2.6 — Non-ML temporal reconstruction baseline

Deliverables:

- Reprojection + accumulation prototype.
- History clamp.
- Disocclusion handling.
- RCAS/CAS-style final sharpening.
- Debug overlays.

Exit criteria:

- There is a measurable temporal baseline before ML is introduced.

### M2.7 — Confidence-guided AI refiner

Deliverables:

- Apply existing model family as a masked refiner.
- AI only runs strongly in low-confidence regions.
- Add “Temporal ↔ AI” policy slider.
- Compare temporal-only vs AI-only vs hybrid.

Exit criteria:

- Hybrid output beats spatial-only output in temporal stability and beats temporal-only output in weak regions.

### M2.8 — ML Temporal Forge model research

Deliverables:

- Training dataset spec.
- Degradation pipeline.
- Ground truth generation.
- Architecture candidates.
- Export path to MiGraphX/ONNX/Vulkan where possible.
- Benchmark suite.

Exit criteria:

- A first model can be trained/evaluated or a candidate architecture can be selected with evidence.

### M2.9 — AMD-first production backend

Deliverables:

- AMD GPU performance path.
- MiGraphX refiner integration.
- Vulkan/compute reconstruction path.
- Cache-aware preview pipeline.
- Packaging controls.

Exit criteria:

- Temporal Forge can process preview segments without destroying normal app reliability.

### M2.10 — Hardware-agnostic expansion

Deliverables:

- Backend abstraction stable enough for non-AMD providers.
- CPU reference path.
- Vulkan portable path.
- Optional ONNX Runtime path.
- Documentation separating AMD Golden Path from portable implementations.

Exit criteria:

- AMD remains first-class without trapping the engine into AMD-only architecture.

---

# Phase 3 — AMD collaboration and ecosystem push

## Objective

Make the project credible to AMD, open-source users, Linux creators, and external contributors.

## Milestones

### M3.1 — AMD outreach package

Deliverables:

- 1-page pitch.
- Demo video.
- Technical whitepaper.
- IP-cleanliness statement.
- Benchmark clips.
- Ask list:
  - guidance on official SDK use
  - feedback on FidelityFX integration
  - possible GPUOpen collaboration
  - possible MiGraphX/ROCm support

### M3.2 — Public technical paper

Deliverables:

- Explain video-generated FSR-style buffers.
- Explain temporal-first / AI-second design.
- Show failure cases honestly.
- Publish metrics.

### M3.3 — Community testing program

Deliverables:

- Known-good test clips.
- Test matrix for RDNA 2 / RDNA 3 / RDNA 4.
- Issue templates.
- Benchmark submission format.

---

# Success metrics

## Phase 1 success

- New user can run a sane job without understanding model names.
- Preview is large enough to judge quality.
- Model selection is reduced and purpose-based.
- Fused families reduce unnecessary multi-pass work.
- Advanced users still have access to legacy/custom models.

## Phase 2 success

- Temporal reconstruction baseline visibly reduces aliasing/shimmer compared to spatial-only upscale.
- Hybrid temporal+AI output has fewer hallucinations than AI-only output.
- Scene cuts and subtitles/text do not smear.
- Debug overlays make failures explainable.
- Performance is good enough for preview-first workflows.

## Phase 3 success

- AMD-VE is credible as an AMD-first open-source creator showcase.
- AMD can understand and evaluate the work without legal/IP confusion.
- External users can reproduce benchmarks.
