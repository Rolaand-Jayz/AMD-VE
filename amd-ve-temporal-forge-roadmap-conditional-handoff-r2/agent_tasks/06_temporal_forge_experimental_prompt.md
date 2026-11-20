# Agent Prompt — Phase 2A Temporal Forge Experimental Contracts

You are implementing Temporal Forge experimental abstractions only.

Prerequisites:

- Phase 1 contracts are implemented.
- Temporal Forge is hidden/disabled/experimental by default.

Read:

- docs/06_PHASE_2_TEMPORAL_FORGE_ML_PRD.md
- docs/07_TEMPORAL_ENGINE_ARCHITECTURE.md
- docs/08_BUFFER_PROVIDERS_AND_MASKS.md
- docs/09_BACKEND_DATAFLOW_AND_STAGE_SYSTEM.md
- docs/19_IMPLEMENTATION_TASKS.md Phase 2
- docs/20_TECHNICAL_CONTRACTS.md section 6
- schemas/temporal_debug_manifest.schema.json

Task:

1. Add TemporalFrameWindow and TemporalFramePacket equivalents.
2. Add provider interfaces and null/reference providers.
3. Add debug dump manifest support.
4. Add tests for optional future frames, scene reset, provider failure, dimension/timebase validation.
5. Keep all behavior experimental and disabled by default.

Do not:

- claim production readiness
- route normal upscale mode through Temporal Forge
- add leaked/redacted source
- train models

Return PR notes using templates/PR_TEMPLATE.md.
