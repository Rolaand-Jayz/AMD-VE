# Agent Prompt — Phase 1A Mode and Family Contracts

You are implementing Phase 1A only: mode and family metadata contracts.

Prerequisites:

- docs/implementation/BASELINE_AUDIT.md exists
- docs/implementation/FILE_MAP.md exists

Read:

- docs/19_IMPLEMENTATION_TASKS.md: P1-E1-T1, P1-E1-T2, P1-E1-T3
- docs/20_TECHNICAL_CONTRACTS.md sections 1 and 2
- schemas/mode_definition.schema.json
- schemas/family_metadata.schema.json

Task:

1. Add or extend the repo's mode/preset/profile data model.
2. Add or extend model family metadata.
3. Preserve existing manifests/custom models.
4. Add visible/advanced family classification.
5. Add unit tests and/or fixture tests.
6. Do not redesign the UI yet except minimal wiring needed to expose data.
7. Do not implement Temporal Forge internals.

Acceptance:

- Required mode ids exist.
- Family metadata validates.
- Old manifests still load.
- Visible families and advanced/raw models can be queried separately.
- CLI/profile representation exists or a minimal deterministic equivalent is implemented.

Return PR notes using templates/PR_TEMPLATE.md.
