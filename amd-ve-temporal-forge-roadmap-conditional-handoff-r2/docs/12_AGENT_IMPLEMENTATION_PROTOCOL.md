# Agent Implementation Protocol

## Purpose

This file tells coding agents how to execute this roadmap without hallucinating or overreaching.

## Ground rules

1. Inspect the repo before modifying.
2. Cite the files changed in your own implementation notes.
3. Do not assume a class/function exists because this PRD names a concept.
4. If the current code lacks a required seam, create the smallest adapter/seam first.
5. Preserve buildability after each milestone.
6. Do not mix Phase 1 and Phase 2 work unless explicitly assigned.
7. Do not vendor redacted FSR4 source.
8. Do not remove working fallbacks without replacement.
9. Keep CLI/GUI parity.
10. Add tests for planner/model/cache behavior before large UI polish.

## Required pre-work for every task

Before making changes, the agent must answer:

```text
What exact files implement this area today?
What behavior already exists?
What behavior is missing?
What is the smallest safe change?
What tests or smoke checks will prove it?
```

## Implementation style

Prefer incremental seams:

```text
interface → tests → minimal implementation → UI wiring → docs
```

Avoid giant rewrites.

## Phase assignment rules

### If task is Phase 1

Allowed:

- UI layout
- preview cache
- model family UX
- fused family planner
- docs/tests

Disallowed:

- ML training
- FSR4/Redstone integration
- deep temporal reconstruction engine

### If task is Phase 2

Allowed:

- temporal packet abstractions
- motion/depth/mask providers
- temporal cache
- reconstruction baseline
- ML refiner

Disallowed:

- breaking Phase 1 UI
- forcing Temporal Forge into the standard model path
- claiming production readiness without benchmarks

## Commit discipline

Recommended commits:

- `docs: add temporal forge roadmap references`
- `ui: introduce preview workstation layout shell`
- `core: add preview cache model`
- `core: add family fusion planning tests`
- `models: add family visibility metadata`
- `temporal: add frame packet interfaces`
- `temporal: add motion provider interface`
- `temporal: add confidence mask debug overlay`

## Required implementation notes

Every PR/task should include:

```markdown
## Summary

## Files changed

## Existing behavior preserved

## New behavior

## Tests run

## Known limitations

## Follow-up tasks
```

## Hallucination blockers

If the agent cannot find a referenced file/class/function:

- search the repo
- inspect adjacent README/docs
- create a mapping note
- do not invent fake APIs
- ask the maintainer only if the missing choice changes implementation direction

## Definition of done for agent tasks

- Code builds or failure is documented with exact blocker.
- Tests exist or smoke test is documented.
- User-facing behavior is described.
- No unrelated files changed.
- No licensing-risk code introduced.
