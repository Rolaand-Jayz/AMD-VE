# PRD — Phase 1: Refined AMD-VE Redesign

## Problem

AMD-VE has strong technical bones but the user-facing experience is too close to a stage/model control panel. The user must understand too much about individual models and backend behavior before getting a good result. Preview is not visually dominant enough for enhancement judgment.

## Product goal

Make AMD-VE feel refined, intentional, and useful immediately.

The app should feel like:

> A restoration bench with a powerful viewer, not a BIOS menu for video models.

## Target users

1. **Casual restorer**
   - wants old footage cleaned/upscaled
   - does not know model names
   - wants obvious before/after proof

2. **Power user**
   - wants control over strength, backend, model family, output
   - needs reproducibility
   - accepts advanced drawers

3. **Developer/tester**
   - needs diagnostics, logs, model compile status, backend stats
   - must not lose CLI parity

## Primary user stories

### US-1: Start a clean enhancement without model knowledge

As a user, I want to choose what I am fixing, not memorize model names.

Acceptance:

- User sees purpose-based modes.
- App selects a default family.
- Family capabilities are visible as chips.
- Advanced model selector is hidden but accessible.

### US-2: Compare quality before rendering

As a user, I want to see a large original/enhanced comparison before committing to a long render.

Acceptance:

- Preview viewer is the central UI element.
- Split-wipe is default.
- Manual divider and auto-sweep work.
- Freeze frame works.
- Side-by-side mode works.

### US-3: Preview by segment

As a user, I want to render a short section and replay it at any speed the cache supports.

Acceptance:

- Timeline supports mark-in/mark-out.
- Rendered segment shows cache status.
- User can scrub and replay cached frames.
- Playback does not pretend unavailable frames exist.

### US-4: Get all model-family benefits by default

As a user, when I choose a family, I want all of that family’s useful enhancements enabled by default.

Acceptance:

- Fused capabilities auto-enable.
- Strength controls exist.
- User can disable a capability if the family supports selective behavior.
- Pipeline uses one pass when valid.

## Functional requirements

### FR-1: Mode-first workflow

Primary modes:

- Restore Video
- Upscale Video
- Anime / Line Art
- 2D→3D SBS
- Interpolate / Smooth Motion
- Advanced / Legacy

Each mode must define:

- default family
- enabled capabilities
- visible controls
- preview behavior
- output behavior

### FR-2: Family cards

Each visible family card must show:

- family name
- short purpose sentence
- capability chips
- speed/quality indicator
- backend compatibility indicator
- installed/available state
- advanced details expander

### FR-3: Fused family execution

If a selected family declares `capabilities` and `fused=true`, matching requested enhancements should collapse into one execution group when allowed by `selective` and capability metadata.

### FR-4: Preview-first main layout

The main window must be organized around:

- top rail: input/output/backend/job controls
- center: large preview canvas
- right inspector: family/capability/strength controls
- bottom: timeline/cache/transport
- drawers: queue, logs, advanced model/backend settings

### FR-5: Keep CLI parity

Every new high-level preset/mode must resolve into explicit pipeline configuration that can be represented in CLI or saved profile form.

## Non-functional requirements

- Must not remove existing core pipeline functionality.
- Must not make the GUI the only path.
- Must not break existing model manifest loading.
- Must not expose experimental Temporal Forge as production-ready in Phase 1.
- Must preserve packaging/build sanity.

## Out of scope for Phase 1

- Full Temporal Forge ML implementation.
- FSR4/Redstone integration.
- New model training.
- Complete hardware-agnostic backend rewrite.

## Phase 1 definition of done

- New UI launches.
- Existing jobs still run.
- Main model list is reduced to visible curated families.
- Advanced/legacy model list remains accessible.
- Preview is large and split-wipe capable.
- Timeline can render and replay preview segments.
- Fused family behavior is implemented or behind a flag with tests.
- Docs and screenshots are updated.
