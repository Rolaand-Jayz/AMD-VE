# Agent Prompt — Phase 1C Preview Cache and Timeline Core

You are implementing Phase 1C only: preview cache, segment preview job, and timeline state model.

Prerequisites:

- Mode/family/planner contracts exist.
- File map identifies preview/render/cache/job areas or marks missing seams.

Read:

- docs/19_IMPLEMENTATION_TASKS.md: P1-E3-T1, P1-E3-T2
- docs/20_TECHNICAL_CONTRACTS.md section 4
- schemas/preview_cache_manifest.schema.json
- docs/22_VALIDATION_MATRIX.md Phase 1C

Task:

1. Add preview cache manifest and stable cache key.
2. Add cache states and invalidation.
3. Add selected segment model.
4. Add preview segment job using the normal render path.
5. Support progress/cancel.
6. Add tests for key stability, invalidation, missing frames, cancelled jobs.

Do not:

- replace full render path
- do UI polish before cache correctness
- report cancelled/incomplete cache as successful

Return PR notes using templates/PR_TEMPLATE.md.
