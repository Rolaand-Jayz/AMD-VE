# Traceability Matrix

| Requirement | Source doc | Implementation area | Test area |
|---|---|---|---|
| Large preview canvas | 02, 03 | GUI main window / preview widget | GUI smoke |
| Split-wipe compare | 03, 05 | preview compositor | preview tests |
| Auto-sweep divider | 03 | preview widget | GUI interaction |
| Seekable timeline | 03, 05 | timeline widget / preview cache | timeline tests |
| Rendered preview cache | 05 | cache manager | cache validity tests |
| Segment preview | 05 | preview job system | segment playback tests |
| Mode-first workflow | 02, 03 | GUI mode resolver | UX smoke |
| Family cards | 04 | model manager UI/schema | model family tests |
| Fused execution | 04, 09 | planner/model metadata | fusion tests |
| Advanced/Legacy model access | 04 | model manager dialog | UI tests |
| CLI parity | 02, 09 | profile/CLI serialization | CLI parity tests |
| Temporal Forge mode shell | 06 | GUI mode resolver | UI smoke |
| TemporalFramePacket | 07, 09 | temporal core | packet tests |
| Motion provider | 08 | temporal providers | motion tests |
| Depth provider | 08 | depth/stereo integration | depth tests |
| Reactive mask | 08 | mask providers | mask tests |
| Confidence map | 08 | confidence provider | confidence tests |
| Non-ML reconstruction baseline | 06, 07 | reconstruction backend | temporal baseline tests |
| Masked AI refiner | 06, 10 | ML refiner adapter | hybrid comparisons |
| Clean-room FSR-style path | 06, 14 | architecture policy | IP checklist |
| AMD outreach package | 14 | docs/release | outreach review |

---

# Stable requirement traceability index

| Req ID | Requirement | PRD/source | Technical contract | Task ID(s) | Schema/fixture | Validation check | Release gate |
|---|---|---|---|---|---|---|---|
| REQ-P0-001 | Baseline audit before code edits | START, 12, 21 | Phase 0 gate | P0-E0-T1 | templates/BASELINE_AUDIT | VAL-P0-001 | Phase 1 release gate |
| REQ-P0-002 | Verified file map before feature work | START, 12, 21 | no invented paths rule | P0-E0-T2 | templates/FILE_MAP | VAL-P0-002 | all PR gates |
| REQ-P1A-001 | Built-in mode definitions exist and serialize | 02, 20 | mode definition contract | P1-E1-T1 | `mode_definition.schema.json`, mode fixtures | VAL-P1A-001, VAL-SCHEMA-001 | Phase 1 release gate |
| REQ-P1A-002 | Family metadata drives UX/planner | 04, 20 | family metadata contract | P1-E1-T2/T3 | `family_metadata.schema.json`, family fixtures | VAL-P1A-001, VAL-SCHEMA-001 | Phase 1 release gate |
| REQ-P1B-001 | Fusion planner preserves all requested capabilities | 04, 09, 20 | fused family planner rules | P1-E2-T1/T2/T3 | planner fixtures after discovery | VAL-P1B-001 | Phase 1 release gate |
| REQ-P1C-001 | Preview cache manifests are deterministic and valid | 05, 20 | cache key + state contract | P1-E3-T1a/T1b | `preview_cache_manifest.schema.json`, preview fixtures | VAL-P1C-001, VAL-SCHEMA-001 | Phase 1 release gate |
| REQ-P1C-002 | Segment preview renders without fake success | 05, 20 | preview job/cancel rules | P1-E3-T2a/T2b | short licensed clip/synthetic frames | VAL-P1C-002 | Phase 1 release gate |
| REQ-P1D-001 | Viewer-first workstation shell | 02, 03 | UI state contract | P1-E4-T1a/T1b | UI smoke evidence | VAL-P1D-001 | Phase 1 release gate |
| REQ-P1D-002 | Accessibility baseline for primary workflow | 03, 22 | keyboard/focus/accessibility contract | P1-E4-T1a/T1b | manual keyboard pass | VAL-P1D-001 | Phase 1 release gate |
| REQ-P1E-001 | CLI/GUI profile parity | 02, 09, 20 | shared planner/profile representation | P1-E5-T1 | profile round-trip fixtures | VAL-P1E-001 | Phase 1 release gate |
| REQ-IP-001 | Clean-room implementation and license registries | 14, external ledger | IP/license merge gate | P1-E5-T2, P3-E1-T1 | legal templates | VAL-LICENSE-001 | all release gates |
| REQ-P2A-001 | Temporal abstractions are experimental by default | 06, 07, 20 | temporal packet/provider contracts | P2-E1-T1a/T2a | temporal debug manifest | VAL-P2A-001 | Phase 2 prototype gate |
| REQ-P2B-001 | Temporal providers produce inspectable debug artifacts | 08, 20 | provider/debug manifest contracts | P2-E2-T1/T2/T3 | debug artifacts | VAL-P2B-001 | Phase 2 prototype gate |
| REQ-P2C-001 | Masked AI refiner beats baselines without protected-region regressions | 06, 10 | refiner/eval contract | P2-E3-T1/T2 | benchmark manifest after discovery | VAL-P2C-001 | Phase 2 production gate |
| REQ-P2C-002 | Temporal Forge feasibility envelope is reported | 06, 10, 22 | prototype report contract | P2-E2/P2-E3 | benchmark report | VAL-P2C-001 | Phase 2 production gate |
