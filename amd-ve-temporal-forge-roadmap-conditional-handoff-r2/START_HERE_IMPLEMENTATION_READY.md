# Start Here — Conditional Implementation Execution Guide

## Purpose

This pack is structured so a coding agent can move from roadmap to implementation **after** it completes Phase 0 repository discovery.

The pack is not permission to edit production code blindly. It is planning-ready and agent-execution-ready; code implementation becomes ready only when `BASELINE_AUDIT.md` and `FILE_MAP.md` exist in the target repository.

Use this file first. Do not start by editing code.

## Execution rule

Implement in this order:

1. **Phase 0 — Baseline and repo map**
2. **Phase 1A — Mode/family metadata contracts**
3. **Phase 1B — Fused planner behavior**
4. **Phase 1C — Preview cache and timeline model**
5. **Phase 1D — Workstation UI shell**
6. **Phase 1E — CLI/profile parity and release hardening**
7. **Phase 2A — Temporal abstractions only**
8. **Phase 2B — Providers/masks/reconstruction prototypes**
9. **Phase 2C — Masked AI refiner and evaluation harness**

Phase 2 work must not start until Phase 1 contracts are merged or intentionally stubbed behind disabled experimental flags.

## Required first action in the repo

Create these files in the target repository before feature work:

```text
docs/implementation/BASELINE_AUDIT.md
docs/implementation/FILE_MAP.md
docs/implementation/IMPLEMENTATION_DECISIONS.md
docs/implementation/KNOWN_LIMITATIONS.md
```

Use the templates in `templates/`.

## Required discovery commands

Run equivalent commands from the repository root and paste results into `docs/implementation/FILE_MAP.md`.

```bash
git rev-parse --show-toplevel
git status --short
git branch --show-current
git rev-parse HEAD
find . -maxdepth 3 -type f | sort | sed 's#^./##' | grep -Ei '(^src/|^include/|^app/|^gui/|^ui/|^core/|^models/|^tests/|^cmake/|CMakeLists|README|docs/)'
grep -RIn "StageKind\|ModelManager\|MiGraphX\|migraphx\|Vulkan\|NCNN\|Preview\|Timeline\|Qt\|QMainWindow\|FFmpeg\|avformat" src include app gui ui core models tests 2>/dev/null | head -300
```

If these paths do not exist, search the whole repository and record the real locations.

## Conditional implementation readiness rule

A feature/code task is implementation-ready only when Phase 0 is complete and the task has:

- target behavior
- likely implementation area or required discovery query
- data contract
- test contract
- CLI/GUI parity requirement if user-facing
- rollback plan
- explicit out-of-scope guardrails

The task tickets in `docs/19_IMPLEMENTATION_TASKS.md` follow this format.

## Golden path for agents

Use the prompts in `agent_tasks/` exactly in order unless the maintainer assigns a narrower task.

Recommended agent split:

| Agent | Best role |
|---|---|
| Crush | execute scoped repo changes end-to-end per task card |
| Copilot | IDE-local implementation and review while seeing real files |
| Codex/ChatGPT | design review, test planning, gap analysis, PR review |

## Non-negotiables

- No redacted or leaked FSR4 source.
- No Phase 2 production claims without metrics.
- No UI-only feature that cannot serialize to a deterministic profile or CLI configuration.
- No breaking fallback backends unless a tested replacement exists.
- No large rewrite when a seam, adapter, or incremental model is enough.
