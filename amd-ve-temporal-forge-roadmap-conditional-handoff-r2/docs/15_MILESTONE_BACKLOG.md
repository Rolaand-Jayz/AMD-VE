# Milestone Backlog

## Phase 0 backlog

- [ ] Create `BASELINE_AUDIT.md`.
- [ ] Capture current GUI screenshots.
- [ ] Count visible models by stage.
- [ ] Record current backend availability on reference machine.
- [ ] Run full test suite.
- [ ] Run one MiGraphX compile/reuse test.
- [ ] Record known broken models.

## Phase 1 / M1.1 — Information architecture

- [ ] Add mode definitions.
- [ ] Add family-card metadata schema.
- [ ] Add visible/advanced/legacy classification.
- [ ] Update model manager to group by family.
- [ ] Add UI mock shell for mode-first workflow.

## Phase 1 / M1.2 — Preview workspace

- [ ] Create large preview widget.
- [ ] Implement split-wipe compositor.
- [ ] Implement side-by-side mode.
- [ ] Implement divider drag.
- [ ] Implement auto-sweep.
- [ ] Implement zoom/pan lock.

## Phase 1 / M1.3 — Timeline/cache

- [ ] Add preview cache data model.
- [ ] Add cache path/settings.
- [ ] Add segment model.
- [ ] Add timeline cache lane.
- [ ] Add render preview segment action.
- [ ] Add cache invalidation.
- [ ] Add playback speed selector.

## Phase 1 / M1.4 — Model consolidation

- [ ] Define default visible family set.
- [ ] Move duplicate models to Advanced/Legacy.
- [ ] Add family purpose descriptions.
- [ ] Add MiGraphX fit/speed/quality metadata.
- [ ] Update model manager dialog.

## Phase 1 / M1.5 — Fusion

- [ ] Add fusion planner tests.
- [ ] Add family group planner.
- [ ] Support selective/non-selective fusion correctly.
- [ ] Display fused execution in planner preview.
- [ ] Preserve raw advanced stage behavior.

## Phase 1 / M1.6 — Release polish

- [ ] Update README/screenshots.
- [ ] Add migration notes.
- [ ] Add user guide.
- [ ] Add troubleshooting guide.
- [ ] Run package smoke tests.

## Phase 2 / M2.1 — Temporal Forge UX shell

- [ ] Add experimental Temporal Forge mode.
- [ ] Add mode presets.
- [ ] Add disabled overlay labels.
- [ ] Add debug overlay UI placeholders.

## Phase 2 / M2.2 — Temporal packet model

- [ ] Add `TemporalFrameWindow`.
- [ ] Add `TemporalFramePacket`.
- [ ] Add buffer metadata structs.
- [ ] Add cache ownership model.
- [ ] Add packet serialization/debug dump.

## Phase 2 / M2.3 — Motion provider

- [ ] Add provider interface.
- [ ] Add CPU reference block matcher.
- [ ] Research/implement FidelityFX Optical Flow integration feasibility.
- [ ] Add visualization overlay.
- [ ] Add scene-cut reset propagation.

## Phase 2 / M2.4 — Depth provider

- [ ] Wrap existing 2D→3D/depth output as provider.
- [ ] Normalize depth.
- [ ] Extract edge/discontinuity mask.
- [ ] Add fallback pseudo-depth.

## Phase 2 / M2.5 — Masks/confidence

- [ ] Add reactive mask provider.
- [ ] Add occlusion/disocclusion provider.
- [ ] Add text/subtitle mask.
- [ ] Add grain policy.
- [ ] Add confidence map.
- [ ] Add AI allowed mask.

## Phase 2 / M2.6 — Non-ML reconstruction

- [ ] Add temporal history cache.
- [ ] Add reprojection prototype.
- [ ] Add history clamp.
- [ ] Add disocclusion handling.
- [ ] Add CAS/RCAS-style final sharpen.

## Phase 2 / M2.7 — Masked AI refiner

- [ ] Wrap existing model backend as refiner.
- [ ] Add mask-aware blending.
- [ ] Add temporal-vs-AI slider.
- [ ] Compare temporal-only / AI-only / hybrid.

## Phase 2 / M2.8 — ML research

- [ ] Define dataset generation pipeline.
- [ ] Define model architecture candidates.
- [ ] Define export targets.
- [ ] Define metrics.
- [ ] Train/evaluate first prototype.

## Phase 3 — AMD/community

- [ ] Draft AMD outreach one-pager.
- [ ] Create demo video.
- [ ] Prepare clean IP statement.
- [ ] Publish benchmark clips.
- [ ] Open tester program.
