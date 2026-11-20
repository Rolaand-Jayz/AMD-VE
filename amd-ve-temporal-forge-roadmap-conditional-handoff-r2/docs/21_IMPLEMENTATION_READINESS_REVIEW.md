# Implementation Readiness Review

## Review summary

The original pack was strategically strong and internally coherent, but not fully implementation-ready for an autonomous coding agent because it left several execution details implicit.

This revision closes those gaps by adding:

- a required start-here execution guide
- explicit task tickets
- technical data contracts
- JSON schemas
- baseline/file-map templates
- PR/release gates
- agent prompts
- phase-specific acceptance checks

## Gaps closed

| Gap | Risk before | Added artifact |
|---|---|---|
| No hard execution order | Agent could start with UI polish or Temporal Forge too early | `START_HERE_IMPLEMENTATION_READY.md` |
| File paths unknown | Agent could hallucinate APIs/classes | `templates/FILE_MAP_TEMPLATE.md`, discovery commands |
| Concepts not converted into contracts | Agents could implement incompatible mode/family/cache structures | `docs/20_TECHNICAL_CONTRACTS.md`, `schemas/` |
| Backlog too checklist-like | Tasks lacked dependencies, rollback, test contracts | `docs/19_IMPLEMENTATION_TASKS.md` |
| Acceptance broad | Could pass with superficial UI changes | `docs/22_VALIDATION_MATRIX.md` |
| Agent prompts generic | Agents may overreach | `agent_tasks/` scoped prompts |
| No PR/release gates | Hard to know when a milestone is shippable | `docs/23_PR_AND_RELEASE_GATES.md` |
| No risk register | Legal/architecture/performance risks could be missed | `docs/24_RISK_REGISTER.md` |

## Remaining uncertainty

The pack still cannot name exact source files in the target AMD-VE repository because this archive does not contain the source repository. That is intentional. Instead, the conditional handoff requires agents to create a verified `FILE_MAP.md` before touching production code.

## What “conditional implementation-ready” now means

A coding agent can now:

1. Run baseline discovery.
2. Map concepts to real repository files.
3. Pick a scoped task ticket.
4. Implement against explicit contracts.
5. Run defined tests/smokes.
6. Produce a PR note using the included template.
7. Stop safely when a phase boundary, missing seam, or legal/IP risk appears.

## Non-goals

This pack does not:

- implement code directly
- claim AMD permission
- include leaked/redacted source material
- choose exact repository file paths without inspecting the target repo
- make Temporal Forge production-ready before metrics exist

## Machine-readable readiness interpretation

The authoritative readiness model is the `readiness` object in `manifest.json`.

| Readiness level | Value | Meaning |
|---|---:|---|
| planning_ready | true | The pack is coherent enough for roadmap, PRD, review, and task planning. |
| agent_execution_ready | true | An agent can begin Phase 0 discovery and documentation immediately. |
| repo_discovery_required | true | The target source repository must be inspected before feature implementation. |
| code_ready | false | Production-code edits are not ready until Phase 0 produces a verified file map. |

## Hard gate

A feature PR must not start until all of these exist in the target repository:

```text
docs/implementation/BASELINE_AUDIT.md
docs/implementation/FILE_MAP.md
docs/implementation/IMPLEMENTATION_DECISIONS.md
docs/implementation/KNOWN_LIMITATIONS.md
```

A task card that says `Blocked by discovery` remains blocked until `FILE_MAP.md` names the actual source files/classes/functions or explicitly records that the seam does not exist yet.
