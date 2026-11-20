# Risk Register

## Purpose

This register turns known danger zones into explicit checks. Update it as implementation reveals new risks.

| ID | Risk | Severity | Phase | Mitigation | Stop condition |
|---|---|---:|---|---|---|
| R-001 | Agent invents file paths/classes | High | All | Require `FILE_MAP.md` before code edits | No implementation until mapped |
| R-002 | UI redesign breaks CLI parity | High | Phase 1 | Profile/CLI serialization tests | GUI-only mode cannot merge |
| R-003 | Fusion planner drops a capability silently | High | Phase 1 | Fusion fixtures + diagnostics | Any silent drop blocks merge |
| R-004 | Cache reuses stale frames | High | Phase 1 | Stable key inputs + invalidation tests | Incorrect cached frame blocks release |
| R-005 | Preview cancel reports success | Medium | Phase 1 | Cancel state test | False cached state blocks merge |
| R-006 | Advanced model access lost | Medium | Phase 1 | Raw model regression tests | Expert access regression blocks release |
| R-007 | Temporal Forge leaks into production modes | High | Phase 2 | Experimental flag + routing tests | Accidental enable blocks merge |
| R-008 | Redacted/leaked FSR4 code enters repo | Critical | All | Clean-room policy + review | Immediate rejection/removal |
| R-009 | AMD SDK license/platform mismatch | High | Phase 2/3 | Separate evaluation branch and legal review | No dependency added without review |
| R-010 | ML refiner hallucinates detail over evidence | High | Phase 2 | AI-allowed masks, confidence gates, protected region tests | Uncontrolled repaint blocks production |
| R-011 | Text/UI/subtitle instability | High | Phase 2 | Protected masks and edge/text metric | Severe instability blocks production claim |
| R-012 | Performance unusable on target AMD hardware | Medium | Phase 2 | runtime/frame and VRAM metrics | Claims must be reduced or optimization required |
| R-013 | Model metadata migration breaks custom models | Medium | Phase 1 | compatibility reader tests | Custom manifest regression blocks release |
| R-014 | Huge rewrite derails project | High | All | one task ticket per PR, adapter-first rule | PR scope rejection |
| R-015 | Benchmark claims outpace evidence | High | Phase 3 | require reproducible demo outputs | Marketing claim cannot ship |

## Risk review cadence

Run this review:

- before each PR merge
- before each milestone label
- before public release/demo
- before AMD/community outreach

## Required risk note in PRs

```markdown
## Risk review
- Risks touched:
- Mitigations added:
- Remaining risk:
- Reason this is safe to merge now:
```
