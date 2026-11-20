# Definition of Ready and Done

## Definition of Ready

A task may start only when:

- It has a task ID from `docs/19_IMPLEMENTATION_TASKS.md` or a maintainer-created equivalent.
- The target repo has `BASELINE_AUDIT.md` and `FILE_MAP.md`.
- The exact files likely to change are listed.
- The existing behavior is summarized.
- The intended new behavior is summarized.
- Required tests/smokes are named.
- Out-of-scope items are named.

## Definition of Done — code task

A code task is done only when:

- It builds or the blocker is exact and documented.
- Tests pass or failures are categorized.
- New behavior has unit/integration/smoke coverage appropriate to the task.
- CLI and GUI parity is preserved when user-facing behavior changes.
- Existing behavior is not removed without documented replacement.
- Docs/implementation notes are updated.
- Risk register is reviewed.

## Definition of Done — UI task

A UI task is done only when:

- The UI state is backed by shared data/model contracts, not hardcoded strings only.
- Missing/unavailable data has explicit empty/error states.
- Long jobs show progress and can cancel when cancellation is supported.
- The same underlying operation can be represented in CLI/profile form when relevant.
- Screenshots or manual interaction notes are captured.

## Definition of Done — Temporal Forge task

A Temporal Forge task is done only when:

- It is hidden/disabled/experimental by default.
- Debug output exists for new buffers/masks/providers.
- Missing provider behavior is safe and diagnostic.
- No production mode depends on it accidentally.
- Claims are limited to tested behavior.

## Definition of Done — release

A release is done only when:

- All phase release gates pass or waivers are recorded.
- User guide is updated.
- Migration notes are updated.
- Troubleshooting notes are updated.
- Known limitations are honest and visible.
- Benchmarks/demos do not overclaim.

## Definition of Ready — schema/contract task

A schema or contract task may start only when:

- the invariant is written in prose,
- JSON Schema capability vs semantic-validator requirement is identified,
- positive and negative fixtures are named,
- target repo test command is known or marked Phase 0 discover.

## Definition of Done — schema/contract task

A schema or contract task is done only when:

- positive fixtures pass,
- negative fixtures fail,
- semantic checks cover invariants JSON Schema cannot express,
- docs explain any remaining validator limitation,
- downstream tasks cite the updated contract.
