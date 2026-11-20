# Implementation Decisions

## Decision record format

```markdown
## D-YYYYMMDD-NN — Title

Status: proposed / accepted / superseded

Context:

Decision:

Consequences:

Related task IDs:
```

## D-00000000-01 — Phase boundaries

Status: accepted

Context: Phase 1 product refinement and Phase 2 Temporal Forge are different risk classes.

Decision: Phase 1 and Phase 2 must not be merged as one rewrite. Phase 2 remains experimental until Phase 1 contracts and validation gates exist.

Consequences: UI, metadata, fusion, preview cache, and CLI/profile parity land before Temporal Forge production work.

Related task IDs: all

## D-00000000-02 — Clean-room temporal path

Status: accepted

Context: The roadmap discusses FSR-class ideas but must avoid licensing/IP risk.

Decision: No leaked, redacted, or accidentally posted FSR4 source is a dependency. Use public SDKs only after license/platform review. Otherwise build clean-room.

Consequences: Any PR adding questionable source is rejected.

Related task IDs: Phase 2, Phase 3

## D-00000000-03 — CLI/GUI parity

Status: accepted

Context: The app has CLI and GUI surfaces.

Decision: User-facing GUI mode/family/preset behavior must serialize to deterministic config/profile or CLI-equivalent behavior.

Consequences: GUI-only features cannot be considered complete.

Related task IDs: P1-E1, P1-E5
