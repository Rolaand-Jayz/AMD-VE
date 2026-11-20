# Implementation Tasks

## Status language

Each task uses this language:

- **Ready** — can be implemented after repo discovery.
- **Blocked by discovery** — agent must map current files/classes first.
- **Blocked by dependency** — another task must land first.
- **Experimental** — must be disabled or hidden unless explicitly enabled.

## Task ID convention

```text
P<phase>-E<epic>-T<task>
```

Example: `P1-E2-T3` = Phase 1, Epic 2, Task 3.

---

# Phase 0 — Baseline and repo map

## P0-E0-T1 — Create baseline audit

**Status:** Ready

**Objective:** Record the exact repo/build/runtime starting point before product changes.

**Inputs:** Repository checkout, target hardware, OS, available models.

**Implementation steps:**

1. Copy `templates/BASELINE_AUDIT_TEMPLATE.md` to `docs/implementation/BASELINE_AUDIT.md`.
2. Fill every section with real observed data.
3. Run the current build and tests.
4. Capture current CLI help output.
5. Capture current GUI launch result.
6. Record one short preview/render attempt.
7. Record one MiGraphX model compile/cache reuse attempt if the backend exists.

**Required commands:** Use project-native commands when known. If unknown, discover them from README, CMake files, package scripts, CI configs, or build docs.

**Acceptance criteria:**

- Baseline document exists.
- Current commit hash is recorded.
- Build command is recorded.
- Test command is recorded.
- Known failures are categorized as blocker, existing debt, or environment-specific.
- No feature files are changed in this task.

**Rollback:** Delete the new baseline file only.

---

## P0-E0-T2 — Create implementation file map

**Status:** Ready

**Objective:** Prevent hallucinated file paths and fake APIs by creating a verified map of the current codebase.

**Implementation steps:**

1. Copy `templates/FILE_MAP_TEMPLATE.md` to `docs/implementation/FILE_MAP.md`.
2. Fill the map using repository searches.
3. For each roadmap area, list real files/classes/functions or mark `not found`.
4. Add the exact grep/ripgrep commands used.

**Areas to map:**

- CLI entry point
- GUI entry point
- Qt main window/viewer widgets
- stage planner
- `StageKind` or equivalent stage taxonomy
- model manifest parser
- model manager/catalog
- backend selection
- MiGraphX integration
- ROCm/HIP fallback
- Vulkan/NCNN fallback
- FFmpeg decode/encode path
- preview/render job system
- tests and fixtures
- packaging scripts

**Acceptance criteria:**

- `docs/implementation/FILE_MAP.md` exists.
- Every major roadmap area has a real location or an explicit `not found` result.
- No production code changed.

---

## P0-E0-T3 — Add implementation decision log

**Status:** Ready

**Objective:** Keep decisions durable as agents iterate.

**Implementation steps:**

1. Copy `templates/IMPLEMENTATION_DECISIONS_TEMPLATE.md` to `docs/implementation/IMPLEMENTATION_DECISIONS.md`.
2. Add initial decisions for phase boundaries, clean-room policy, and CLI/GUI parity.
3. Add a rule that every future change adding a new public behavior must log a decision or cite an existing one.

**Acceptance criteria:** Decision log exists and has at least the initial required entries.

---

# Phase 1 — Refined AMD-VE

## Epic P1-E1 — Mode and family metadata foundation

### P1-E1-T1 — Add mode definition contract

**Status:** Blocked by discovery

**Objective:** Introduce a durable product-level mode contract for Restore, Upscale, Anime/Line Art, 2D→3D SBS, Interpolate, Advanced/Legacy, and Temporal Forge experimental shell.

**Likely implementation areas:** mode resolver, GUI state model, CLI profile serialization, settings/profile code.

**Discovery query:**

```bash
grep -RIn "mode\|preset\|profile\|StageKind\|pipeline\|planner" src include app gui ui core models tests 2>/dev/null | head -300
```

**Data contract:** Use `schemas/mode_definition.schema.json`.

**Implementation steps:**

1. Create an internal `EnhancementModeDefinition` or equivalent struct.
2. Add constants/default definitions for:
   - `restore_video`
   - `upscale_video`
   - `anime_line_art`
   - `stereo_3d_sbs`
   - `interpolate_motion`
   - `advanced_legacy`
   - `temporal_forge_experimental`
3. Each mode must define:
   - id
   - label
   - purpose
   - default family id
   - visible controls
   - default capabilities
   - preview behavior
   - output behavior
   - CLI/profile representation
4. Add parser/serializer if profiles exist.
5. Wire only enough UI to select modes or expose definitions to the UI.

**Test contract:**

- Unit test mode ids are unique.
- Unit test each mode resolves to a valid default family or explicit advanced/manual state.
- Unit test mode serialization round-trips.
- CLI dry-run can print/resolve mode configuration, or equivalent profile inspection exists.

**Acceptance criteria:**

- Modes exist as data, not scattered UI strings.
- Each mode can resolve to pipeline intent without opening the GUI.
- Temporal Forge mode is marked experimental/disabled unless explicitly enabled.

**Out of scope:** UI polish, Temporal Forge implementation, model training.

---

### P1-E1-T2 — Extend model family metadata

**Status:** Blocked by discovery

**Objective:** Make family metadata the primary UX/planner unit while preserving raw model access.

**Likely implementation areas:** model manifest parser, model manager/catalog, UI model list, planner.

**Discovery query:**

```bash
grep -RIn "model.*manifest\|manifest.*model\|family\|capabilities\|fused\|selective\|precision\|scale" src include app gui ui core models tests 2>/dev/null | head -300
```

**Data contract:** Use `schemas/family_metadata.schema.json`.

**Implementation steps:**

1. Add or formalize family-level fields:
   - `family_id`
   - `family_name`
   - `display_purpose`
   - `content_type`
   - `speed_tier`
   - `quality_tier`
   - `migraphx_fit`
   - `visible_default`
   - `advanced_only`
   - `capabilities`
   - `fused`
   - `selective`
   - `safe_defaults`
   - `strength_controls`
2. Validate manifests at load time.
3. Preserve existing custom `.avemodel` or equivalent manifests.
4. Add compatibility path for older manifests with default family grouping.

**Test contract:**

- Existing manifests still load.
- New metadata loads and validates.
- Missing optional fields get safe defaults.
- Invalid enum values produce actionable errors.
- Advanced/raw model id still works.

**Acceptance criteria:**

- GUI/catalog can request visible families separately from advanced/legacy models.
- Planner can consume family capabilities without parsing UI strings.

---

### P1-E1-T3 — Add visible family slate

**Status:** Blocked by P1-E1-T2

**Objective:** Define the first visible family slate without deleting legacy models.

**Implementation steps:**

1. Add visible family definitions for:
   - ClearReality Fast
   - OpenProteus Quality
   - Anime / Line Art
   - Motion / RIFE
   - Depth / SBS 3D
   - Temporal Forge Experimental
   - Advanced / Legacy
2. Mark duplicates, unstable variants, compatibility exports, and raw checkpoints as advanced/legacy.
3. Add a migration note explaining demotion is not deletion.

**Test contract:**

- Visible family query returns only default families.
- Advanced query returns demoted/raw models.
- Raw id selection still resolves.

**Acceptance criteria:** Main workflow is family-first; expert access is preserved.

---

## Epic P1-E2 — Fused planner

### P1-E2-T1 — Add fusion planner test fixtures

**Status:** Blocked by discovery

**Objective:** Lock the desired fusion behavior before changing planner code.

**Likely implementation areas:** tests for planner/model catalog/stage execution.

**Discovery query:**

```bash
find . -maxdepth 4 -type f | grep -Ei 'test|spec|fixture|ctest|gtest|catch|doctest|pytest'
grep -RIn "planner\|StageKind\|fused\|capabilit" tests src include 2>/dev/null | head -300
```

**Required fixtures:**

1. Non-fused family with two requested capabilities.
2. Fused non-selective family with exact full capability set.
3. Fused non-selective family with partial requested capability set.
4. Fused selective family with valid subset.
5. Fused family with unsupported requested capability.
6. Advanced raw model selection.

**Expected behavior:** See `docs/20_TECHNICAL_CONTRACTS.md`.

**Acceptance criteria:** Tests fail before implementation if current behavior is missing.

---

### P1-E2-T2 — Implement fusion planning group

**Status:** Blocked by P1-E2-T1

**Objective:** Collapse compatible family capabilities into one execution group where valid.

**Implementation steps:**

1. Add `FusedAIGroup` or equivalent planner node.
2. Implement grouping after deterministic stage ordering.
3. Validate requested capabilities against family metadata.
4. Respect `fused` and `selective` rules.
5. Preserve ordinary stage execution when fusion is invalid.
6. Add planner diagnostics explaining why fusion did or did not happen.

**Test contract:** Existing P1-E2-T1 fixtures pass.

**Acceptance criteria:**

- No capability silently disappears.
- No unsupported fusion occurs.
- Diagnostics are visible to CLI/profile/debug output.

---

### P1-E2-T3 — Expose planner preview to CLI and GUI

**Status:** Blocked by P1-E2-T2

**Objective:** Users and agents can see what the planner will execute before running a job.

**Implementation steps:**

1. Add a dry-run/planner-preview output path if missing.
2. Include mode, family, capabilities, fused groups, raw stages, backend, precision, cache keys.
3. Expose same data to GUI advanced foldout.

**Acceptance criteria:**

- CLI and GUI planner previews use the same backend representation.
- A fused group is clearly identified.

---

## Epic P1-E3 — Preview cache and timeline

### P1-E3-T1 — Add preview cache manifest model

**Status:** Blocked by discovery

**Objective:** Cache preview segment outputs deterministically so users can scrub rendered previews.

**Likely implementation areas:** preview job system, cache path/settings, frame storage, timeline model.

**Discovery query:**

```bash
grep -RIn "preview\|cache\|timeline\|segment\|render job\|job" src include app gui ui core tests 2>/dev/null | head -400
```

**Data contract:** Use `schemas/preview_cache_manifest.schema.json`.

**Implementation steps:**

1. Add cache root setting.
2. Add cache manifest type.
3. Add stable cache key from:
   - source file identity
   - source file mtime/size or content hash if affordable
   - time range
   - frame rate/timebase
   - mode/family/capabilities
   - model ids/checksums
   - backend/precision
   - relevant strength controls
   - app/cache schema version
4. Add cache states: empty, analyzing, rendering, cached, warning, invalid.
5. Add cache invalidation rules.

**Test contract:**

- Same config yields same key.
- Different strength/model/backend/source timestamp yields different key.
- Missing frame invalidates segment.
- Schema version bump invalidates old cache or marks migration needed.

**Acceptance criteria:** Preview cache can be reasoned about without GUI.

---

### P1-E3-T2 — Implement segment preview job

**Status:** Blocked by P1-E3-T1

**Objective:** Render a selected time range into cache without committing full output.

**Implementation steps:**

1. Add segment range model.
2. Add preview job type using the normal backend/pipeline path.
3. Write original/enhanced frame references and timing metadata.
4. Support cancellation.
5. Report progress to timeline/cache lane.

**Acceptance criteria:**

- A short segment renders into cache.
- User can scrub cached frames.
- Cancel leaves cache in warning/invalid/incomplete state, not fake success.
- Full render path remains unchanged.

---

### P1-E3-T3 — Add split-wipe preview compositor

**Status:** Blocked by discovery; can run parallel after P1-E3-T1 contract

**Objective:** Compare original and enhanced preview frames with a draggable divider.

**Implementation steps:**

1. Create a preview compositor/viewer component.
2. Support modes:
   - split wipe
   - side-by-side
   - original only
   - enhanced only
   - freeze compare
   - debug overlay placeholder
3. Implement synchronized zoom/pan.
4. Implement divider drag, reset, and auto-sweep.
5. Use cached frames when available.

**Acceptance criteria:**

- Split-wipe works on a cached segment.
- Zoom/pan applies to both sides.
- Missing enhanced frame displays an explicit uncached state.

---

## Epic P1-E4 — Workstation UI shell

### P1-E4-T1 — Replace primary flow with workstation layout shell

**Status:** Blocked by P1-E1 and P1-E3 contracts

**Objective:** Make the viewer the main product surface.

**Implementation steps:**

1. Create layout with:
   - top rail
   - central preview canvas
   - right inspector
   - bottom timeline/cache lane
   - drawers for queue/logs/advanced settings
2. Wire mode/family data into right inspector.
3. Keep existing advanced/manual controls accessible.
4. Do not remove old behavior until parity is proven.

**Acceptance criteria:**

- GUI launches into the new layout.
- Existing simple render still possible.
- Advanced/raw model access still possible.
- Timeline can show empty/cache states even before full preview segment implementation.

---

### P1-E4-T2 — Add family cards and capability controls

**Status:** Blocked by P1-E1-T2

**Objective:** Expose purpose-first controls while preserving expert inspectability.

**Implementation steps:**

1. Render visible family cards.
2. Display capability chips.
3. Display backend compatibility and installed/available state.
4. Render safe default strength controls.
5. Add advanced foldout with raw model/backend/planner details.

**Acceptance criteria:**

- User can select a family without selecting a raw model.
- Advanced users can still view raw model id and backend plan.
- Non-selective fused families do not present misleading per-capability toggles.

---

## Epic P1-E5 — CLI/profile parity

### P1-E5-T1 — Add profile serialization for high-level mode/family selections

**Status:** Blocked by P1-E1

**Objective:** Every GUI mode/family choice must become an explicit reproducible config.

**Implementation steps:**

1. Define profile format or extend existing one.
2. Serialize:
   - mode id
   - family id or raw model id
   - capabilities
   - strength controls
   - backend preference
   - precision
   - output target
3. Add CLI load/profile/dry-run parity.

**Acceptance criteria:**

- GUI-created profile can be run by CLI.
- CLI-created equivalent config appears correctly in GUI or planner preview.
- Round-trip tests exist.

---

### P1-E5-T2 — Add release hardening checklist

**Status:** Ready after Phase 1 implementation

**Objective:** Ensure Phase 1 does not ship as a half-redesign.

**Implementation steps:**

1. Copy `templates/RELEASE_GATE_TEMPLATE.md` to release notes or implementation docs.
2. Run all test/smoke commands.
3. Add screenshots.
4. Add migration notes.
5. Verify packages if packaging exists.

**Acceptance criteria:** All release gate boxes are checked or explicitly waived with reason.

---

# Phase 2 — Temporal Forge ML

## Epic P2-E1 — Temporal abstractions

### P2-E1-T1 — Add TemporalFrameWindow and TemporalFramePacket contracts

**Status:** Blocked by Phase 1 contracts; experimental

**Objective:** Add multi-frame temporal data structures without changing production upscaling behavior.

**Implementation steps:**

1. Add temporal namespace/module.
2. Implement contracts from `docs/20_TECHNICAL_CONTRACTS.md`.
3. Add serialization/debug dump for packets.
4. Add tests with previous/current/next frame cases.
5. Keep feature hidden behind experimental flag.

**Acceptance criteria:**

- Packet can represent current frame plus optional history/future.
- Missing optional frames are valid.
- Scene reset metadata can be attached.
- No production mode uses Temporal Forge accidentally.

---

### P2-E1-T2 — Add provider interfaces

**Status:** Blocked by P2-E1-T1; experimental

**Objective:** Define swappable providers for motion, depth, reactive mask, confidence, and AI-allowed mask.

**Implementation steps:**

1. Add provider interfaces from `docs/20_TECHNICAL_CONTRACTS.md`.
2. Add null/reference providers.
3. Add debug dump outputs.
4. Add tests for dimensions, timebase, invalid/missing provider behavior.

**Acceptance criteria:**

- Providers can fail gracefully with diagnostics.
- Temporal engine can request buffers without knowing provider implementation.

---

## Epic P2-E2 — Reference providers and non-ML baseline

### P2-E2-T1 — Add CPU reference block-motion provider

**Status:** Blocked by P2-E1; experimental

**Objective:** Provide a simple, testable motion field before GPU/provider optimization.

**Implementation steps:**

1. Implement block-based motion estimation.
2. Add scene-change detection threshold.
3. Output visualization/debug map.
4. Add synthetic tests for known translations.

**Acceptance criteria:**

- Known synthetic shifts produce expected motion direction.
- Scene cut emits reset signal.
- Provider performance is documented as reference-only if slow.

---

### P2-E2-T2 — Wrap existing depth/SBS output as depth provider

**Status:** Blocked by discovery and P2-E1; experimental

**Objective:** Reuse the existing 2D→3D/depth path as a proxy-depth provider.

**Implementation steps:**

1. Discover existing depth/stereo code.
2. Add adapter provider.
3. Normalize depth to stable 0..1 range.
4. Add edge/discontinuity mask.
5. Add fallback pseudo-depth if depth is unavailable.

**Acceptance criteria:**

- Provider returns depth dimensions matching source/working resolution.
- Missing depth model degrades gracefully.
- Debug overlay can display normalized depth.

---

### P2-E2-T3 — Add temporal-only reconstruction baseline

**Status:** Blocked by P2-E2-T1/T2; experimental

**Objective:** Build a non-ML temporal reconstruction baseline before training or using an AI refiner.

**Implementation steps:**

1. Implement history reprojection.
2. Add confidence/history clamp.
3. Add disocclusion handling.
4. Add protected-region handling for text/UI/subtitles.
5. Add final sharpen optional stage.
6. Compare against spatial-only baseline.

**Acceptance criteria:**

- Temporal-only mode can render a short clip behind experimental flag.
- Debug outputs include motion, confidence, reset, and AI-allowed masks.
- Metrics compare temporal-only, spatial-only, and source.

---

## Epic P2-E3 — Masked AI refiner

### P2-E3-T1 — Add AI refiner adapter

**Status:** Blocked by P2-E2-T3; experimental

**Objective:** Use AI only where confidence/masks permit it.

**Implementation steps:**

1. Wrap existing backend model execution as refiner.
2. Accept input stack:
   - source/current frame
   - temporal reconstruction
   - confidence map
   - AI-allowed mask
   - optional motion/depth/debug buffers
3. Blend output according to confidence and protected regions.
4. Add slider/control for temporal-vs-AI influence.

**Acceptance criteria:**

- Protected text/UI/subtitle areas are not aggressively repainted by default.
- High-confidence temporal areas preserve temporal reconstruction.
- Low-confidence areas may receive AI residual correction.

---

### P2-E3-T2 — Add evaluation harness

**Status:** Blocked by P2-E2-T3; experimental

**Objective:** Measure Temporal Forge against baselines without relying on vibes.

**Implementation steps:**

1. Add benchmark manifest format.
2. Add clip categories:
   - live action
   - anime/line art
   - archival/compressed
   - game capture with UI/text
   - fast motion
   - grain/noise
3. Add metrics:
   - PSNR/SSIM if paired ground truth exists
   - LPIPS or equivalent perceptual metric if available
   - temporal consistency/flicker metric
   - edge/text stability metric
   - runtime/frame and VRAM use
4. Add visual A/B export.

**Acceptance criteria:**

- Every Temporal Forge claim references a benchmark output.
- Metrics and visual outputs are versioned.

---

# Phase 3 — Outreach and release proof

## P3-E1-T1 — Create AMD/community demo package

**Status:** Blocked by Phase 1 release or Phase 2 prototype, depending on target

**Objective:** Produce a credible outreach package without overclaiming.

**Implementation steps:**

1. Create clean IP statement.
2. Create benchmark/demo clips or use redistributable sample clips.
3. Record before/after preview workflow.
4. Document AMD hardware/backend path.
5. Document what is clean-room and what uses public AMD SDKs.

**Acceptance criteria:**

- No leaked/redacted source dependency.
- Claims are matched to reproducible demos.
- AMD-facing language is collaborative, not adversarial.

---

# Oversized-task split cards

These split cards preserve the existing task IDs by adding suffixes. Use them when the original card is too broad for a safe PR.

## P1-E3-T1a — Preview cache schema/type only

- **Primary subsystem:** cache manifest data model and schema validation
- **Allowed file areas after discovery:** cache/model types, schema fixtures, tests
- **Forbidden areas:** Qt layout rewrite, render job changes, Temporal Forge providers
- **Required tests:** schema fixture validation; cache manifest parse/serialize
- **Rollback:** remove manifest type and fixtures only
- **Maximum PR scope:** no frame rendering, no timeline UI

## P1-E3-T1b — Cache key and invalidation

- **Primary subsystem:** cache key builder and invalidation rules
- **Allowed file areas after discovery:** cache manager, source-file identity helpers, planner hash helpers, tests
- **Forbidden areas:** split-wipe viewer, Temporal Forge, full render changes unrelated to preview
- **Required tests:** same config same key; source/model/backend/strength/schema changes alter key
- **Rollback:** disable preview cache key use while preserving type definitions
- **Maximum PR scope:** no UI except debug output if needed

## P1-E3-T2a — Segment range and preview job skeleton

- **Primary subsystem:** preview job orchestration
- **Allowed file areas after discovery:** job system, segment range model, progress/cancel signal path, tests
- **Forbidden areas:** cache compositor, UI layout rewrite, model-family redesign
- **Required tests:** valid/invalid range; cancel produces non-cached state
- **Rollback:** disable preview job registration
- **Maximum PR scope:** no final split-wipe rendering

## P1-E3-T2b — Frame extraction/write path

- **Primary subsystem:** FFmpeg/frame extraction and cache writer
- **Allowed file areas after discovery:** media spine, cache writer, preview job tests
- **Forbidden areas:** main workstation shell, Temporal Forge providers
- **Required tests:** short segment writes original/enhanced frame refs and manifest
- **Rollback:** remove frame writer integration and leave schema intact
- **Maximum PR scope:** one short-clip integration path only

## P1-E4-T1a — Workstation shell layout only

- **Primary subsystem:** main window layout skeleton
- **Allowed file areas after discovery:** Qt main window, layout components, placeholder state model
- **Forbidden areas:** backend planner changes, cache writer changes, Temporal Forge internals
- **Required tests:** GUI launch smoke; empty states visible
- **Rollback:** revert shell entry point to legacy layout
- **Maximum PR scope:** layout and empty-state wiring only

## P1-E4-T1b — Inspector/family wiring

- **Primary subsystem:** right inspector and family card binding
- **Allowed file areas after discovery:** mode/family view models, inspector UI, tests/smokes
- **Forbidden areas:** raw planner algorithm changes, cache job implementation
- **Required tests:** visible families separate from advanced/raw models
- **Rollback:** hide new inspector and expose legacy model list
- **Maximum PR scope:** family display and selection only

## P2-E1-T1a — Temporal packet data model only

- **Primary subsystem:** temporal namespace/types
- **Allowed file areas after discovery:** new experimental temporal module, tests
- **Forbidden areas:** production planner routing, ML refiner, UI overlays
- **Required tests:** packet represents history/current/future and scene reset
- **Rollback:** remove experimental module
- **Maximum PR scope:** no provider implementation

## P2-E1-T2a — Provider interfaces/null providers only

- **Primary subsystem:** temporal provider interfaces
- **Allowed file areas after discovery:** experimental temporal module, debug manifest, tests
- **Forbidden areas:** optimized GPU providers, ML training, production routing
- **Required tests:** null providers fail gracefully with diagnostics
- **Rollback:** remove provider interface registration
- **Maximum PR scope:** no real motion/depth algorithm
