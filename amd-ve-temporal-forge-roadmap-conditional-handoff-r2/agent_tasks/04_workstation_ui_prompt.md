# Agent Prompt — Phase 1D Workstation UI Shell

You are implementing Phase 1D only: the preview-first workstation UI shell.

Prerequisites:

- Mode/family contracts exist.
- Preview cache/timeline model exists or has safe stubs.

Read:

- docs/03_UI_UX_WORKSTATION_SPEC.md
- docs/05_PREVIEW_TIMELINE_CACHE_SPEC.md
- docs/19_IMPLEMENTATION_TASKS.md: P1-E3-T3, P1-E4-T1, P1-E4-T2
- docs/20_TECHNICAL_CONTRACTS.md section 5
- docs/22_VALIDATION_MATRIX.md Phase 1D

Task:

1. Add top rail, central preview, right inspector, bottom timeline layout.
2. Add split-wipe viewer and side-by-side/original/enhanced modes.
3. Wire family cards and capability chips.
4. Keep advanced/raw model access.
5. Show cache states and uncached/missing states.
6. Capture screenshots or manual UI validation notes.

Do not:

- delete legacy controls until replacement parity is proven
- hardcode behavior that cannot serialize to profile/CLI
- implement Temporal Forge internals

Return PR notes using templates/PR_TEMPLATE.md.
