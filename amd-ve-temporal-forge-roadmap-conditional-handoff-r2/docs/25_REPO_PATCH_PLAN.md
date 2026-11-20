# Repository Patch Plan

## Purpose

This file gives a practical patch order for the target AMD-VE repository. It does not assume exact source paths. The agent must map real paths first using `FILE_MAP.md`.

---

# Patch 0 — Documentation scaffolding only

## Files to add

```text
docs/implementation/BASELINE_AUDIT.md
docs/implementation/FILE_MAP.md
docs/implementation/IMPLEMENTATION_DECISIONS.md
docs/implementation/KNOWN_LIMITATIONS.md
```

## No production code changes

This patch proves the agent understands the current repo.

---

# Patch 1 — Mode/family data model

## Expected code areas

- model manifest parser/catalog
- mode/preset/profile resolver
- tests

## Changes

- Add `EnhancementModeDefinition` equivalent.
- Add `ModelFamilyMetadata` equivalent or extend existing metadata.
- Add validation.
- Add compatibility defaults for old manifests.
- Add tests.

## Do not do

- Do not redesign the UI yet.
- Do not change render execution yet.

---

# Patch 2 — Planner fusion tests

## Expected code areas

- planner tests
- test fixtures/manifests

## Changes

- Add fixtures for non-fused, fused non-selective, fused selective, unsupported capability, raw model.
- Add expected outputs and diagnostics.

## Do not do

- Do not implement planner changes in the same patch unless the project convention requires test+implementation together.

---

# Patch 3 — Planner fusion implementation

## Expected code areas

- stage planner
- pipeline intent/profile resolver
- diagnostics/logging

## Changes

- Add fused group node or equivalent.
- Implement grouping rules.
- Add diagnostics.
- Pass tests.

---

# Patch 4 — Planner preview parity

## Expected code areas

- CLI dry-run/help/profile output
- GUI advanced foldout or debug panel

## Changes

- Expose planned pipeline in one shared representation.
- Show fused/ordinary stage plan.

---

# Patch 5 — Preview cache core

## Expected code areas

- preview/job/cache systems
- settings/cache path
- tests

## Changes

- Add cache manifest.
- Add cache key.
- Add invalidation.
- Add cache states.

## Do not do

- Do not build the whole UI timeline yet.

---

# Patch 6 — Segment preview job

## Expected code areas

- render job system
- FFmpeg frame extraction/preview path
- cache writer
- cancellation/progress

## Changes

- Render marked segment into cache.
- Track progress/cancel.
- Write manifest/frame metadata.

---

# Patch 7 — Preview viewer and split-wipe

## Expected code areas

- Qt viewer widget
- preview frame loader
- GUI state

## Changes

- Add split-wipe, side-by-side, original/enhanced-only.
- Lock zoom/pan.
- Show uncached state.

---

# Patch 8 — Workstation shell

## Expected code areas

- main window/layout
- right inspector
- timeline widget
- queue/log drawers

## Changes

- Move UI to top rail + central preview + inspector + bottom timeline.
- Wire mode/family selection.
- Keep legacy advanced access.

---

# Patch 9 — CLI/profile parity

## Expected code areas

- profile save/load
- CLI args/config
- GUI profile import/export

## Changes

- Serialize mode/family/capabilities/strength/backend/precision.
- Round-trip tests.

---

# Patch 10 — Phase 1 release hardening

## Changes

- Update README/user guide.
- Update screenshots.
- Add troubleshooting.
- Run package smoke.
- Close release gate checklist.

---

# Patch 11+ — Temporal Forge experimental branch

Only after Phase 1 contracts land.

Recommended order:

1. Temporal packet structs + tests.
2. Provider interfaces + null providers.
3. CPU reference motion provider.
4. Depth adapter provider.
5. Reactive/confidence/AI-allowed masks.
6. Temporal-only reconstruction baseline.
7. Masked AI refiner adapter.
8. Evaluation harness.

All Phase 2 patches must be disabled or experimental by default.

---

# Patch scope refinement

Use these narrower patch boundaries when an agent would otherwise touch too many subsystems at once.

| Original patch | Split patch | Primary behavior | Review boundary |
|---|---|---|---|
| Patch 5 | 5a cache manifest/type | data model + schema only | no rendering/UI |
| Patch 5 | 5b cache key/invalidation | deterministic key and invalidation | no timeline widget |
| Patch 6 | 6a segment job skeleton | range/progress/cancel state | no frame extraction if not needed |
| Patch 6 | 6b frame extraction/cache write | short segment writes cache frames | no workstation redesign |
| Patch 7 | 7a preview compositor core | split/SBS/original/enhanced component | no whole app layout rewrite |
| Patch 8 | 8a workstation layout shell | top rail/preview/inspector/timeline placeholders | no backend planner changes |
| Patch 8 | 8b inspector/family binding | family cards and advanced foldout | no cache/job changes |
| Patch 11+ | 11a temporal packet model | types and serialization only | no production routing |
| Patch 11+ | 11b provider interfaces | null/reference interfaces only | no ML/refiner |
| Patch 11+ | 11c reference motion provider | synthetic tests and debug output | reference-only performance label |
| Patch 11+ | 11d temporal-only baseline | experimental render path and metrics | no production enablement |

A PR that crosses two rows must explain why the split is impossible in that repository.
