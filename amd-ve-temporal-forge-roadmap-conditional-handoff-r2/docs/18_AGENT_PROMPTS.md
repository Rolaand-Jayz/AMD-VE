# Agent Prompts

## Crush prompt — Phase 0 baseline audit

```text
You are working in the AMD-VE repository. Your task is to create a baseline audit before any redesign work.

Read README.md, docs/GOLD_STANDARD_FOR_IMPLEMENTATION.md, docs/CUSTOM_MODEL_MANIFEST.md, src/gui/README.md, src/backends/README.md, include/ave/types.hpp, src/planner.cpp, and relevant model/backend files.

Create BASELINE_AUDIT.md with:
- current commit/ref
- build configuration
- current backend order
- stage taxonomy
- model catalog summary
- GUI architecture summary
- known risks
- exact tests/smoke checks run

Do not modify app behavior. Do not invent missing APIs. If something cannot be verified, mark it as unknown.
```

## Crush prompt — Phase 1 UI shell

```text
Implement the Phase 1 preview-first UI shell from docs/03_UI_UX_WORKSTATION_SPEC.md.

Goal:
- Make the preview canvas the center of the main window.
- Add top rail, right inspector, and bottom timeline shell.
- Preserve existing run/preview/model/settings functionality where possible.
- Do not implement Temporal Forge engine yet.

Rules:
- Keep CLI untouched.
- Keep existing backend pipeline intact.
- Add minimal tests or smoke notes.
- Avoid deleting existing controls until replacements are wired.
```

## Crush prompt — model family consolidation

```text
Implement model-family grouping and visibility tiers based on docs/04_MODEL_CONSOLIDATION_AND_FUSION_SPEC.md.

Goal:
- Group models by family in the GUI/model manager.
- Add visible_default / advanced_only style metadata where appropriate.
- Do not delete model entries unless explicitly obsolete and tested.
- Preserve custom .avemodel loading.
- Add tests for family helper behavior.
```

## Crush prompt — fused family planner

```text
Implement fused family planning.

Read docs/04_MODEL_CONSOLIDATION_AND_FUSION_SPEC.md and docs/09_BACKEND_DATAFLOW_AND_STAGE_SYSTEM.md.

Goal:
- After deterministic stage ordering, collapse consecutive compatible stages from the same family when model metadata allows fused execution.
- Respect selective=false and selective=true behavior.
- Add planner tests for fused, non-fused, selective, and invalid subset cases.

Do not change backend inference behavior until planner tests pass.
```

## Crush prompt — preview cache

```text
Implement the preview cache data model from docs/05_PREVIEW_TIMELINE_CACHE_SPEC.md.

Goal:
- Add preview job and cache metadata structs/classes.
- Add deterministic cache validity hashing.
- Add segment state tracking.
- Integrate minimally with GUI timeline shell.

Do not implement Temporal Forge analysis buffers yet; reserve extension points only.
```

## Crush prompt — Temporal Forge packet skeleton

```text
Begin Phase 2 by implementing only the Temporal Forge data-model skeleton.

Read docs/06_PHASE_2_TEMPORAL_FORGE_ML_PRD.md, docs/07_TEMPORAL_ENGINE_ARCHITECTURE.md, docs/08_BUFFER_PROVIDERS_AND_MASKS.md, and docs/09_BACKEND_DATAFLOW_AND_STAGE_SYSTEM.md.

Goal:
- Add TemporalFrameWindow and TemporalFramePacket concepts.
- Add provider interfaces for motion/depth/masks/exposure/reset.
- Add a CPU/reference stub provider where needed.
- Add tests for packet construction and reset behavior.

Do not implement FSR4. Do not vendor redacted source. Do not claim production Temporal Forge support.
```

## Copilot prompt — review mode

```text
Review the current diff against the AMD-VE roadmap docs in amd-ve-temporal-forge-roadmap/docs.

Focus on:
- whether the implementation preserves CLI/core pipeline behavior
- whether UI changes are preview-first without hiding required controls
- whether model family changes preserve custom manifests
- whether fused execution respects metadata
- whether any Phase 2 code is accidentally mixed into Phase 1
- whether any licensing-risk FSR4/redacted dependency was introduced

Return concrete file-specific issues and suggested fixes. Avoid generic style comments unless they affect maintainability.
```

## Codex prompt — implementation planning

```text
Create an implementation plan for the selected milestone from amd-ve-temporal-forge-roadmap/docs/15_MILESTONE_BACKLOG.md.

Before writing code:
- inspect the repository files that already implement this area
- list exact files likely to change
- identify tests to add/update
- identify migration risks
- break the work into small commits

Do not assume APIs. Do not implement until the plan maps to real files.
```

## ChatGPT prompt — architecture sanity check

```text
Evaluate the proposed AMD-VE change against the roadmap bundle.

Question:
Does this change belong to Phase 1 Refined AMD-VE, Phase 2 Temporal Forge ML, or Phase 3 outreach/ecosystem?

Then assess:
- alignment with roadmap
- risk
- missing requirements
- likely files impacted
- tests needed
- whether it could cause hallucinated implementation work

Be blunt and specific.
```
