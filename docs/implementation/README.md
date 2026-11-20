# Implementation Docs

This directory contains the Phase 0 verified handoff documentation for the AMD-VE → Temporal Forge implementation path.

## What this folder is for

These docs are the **implementation-ready handoff layer** between:
- the existing AMD-VE codebase (what exists today)
- the Temporal Forge roadmap (what is planned for the future)

They are designed so that a capable agent can safely continue implementation without inventing missing APIs, overstating release readiness, or mixing Phase 1 and Phase 2 work.

## Canonical docs

| Doc | Purpose | Status |
|---|---|---|
| `BASELINE_AUDIT.md` | Exact repo state, build/test evidence, seams, unknowns | verified |
| `FILE_MAP.md` | Real files/classes/functions — no fictional seams | verified |
| `IMPLEMENTATION_DECISIONS.md` | Durable decisions for all phases | verified |
| `KNOWN_LIMITATIONS.md` | What is verified, planned, experimental, unsupported, missing | verified |
| `AUDIT_CLOSURE_MATRIX.md` | Open audit findings with fix status and evidence | verified |
| `HANDOFF_VERIFICATION_REPORT.md` | Final handoff verdict and next-agent scope | verified |
| `PUBLIC_TRUTH_SURFACE_MATRIX.md` | Public-facing claim consistency check | verified |
| `CLAIM_AUDIT_GREP_LOG.md` | Risky-claim search results and disposition | verified |
| `REMAINING_AUDIT_RISKS.md` | Risk ledger with status tracking | verified |
| `SCHEMA_VALIDATION_EVIDENCE.md` | R2 schema validation results | verified |
| `PHASE0_COMMAND_OUTPUTS.log` | Raw command output evidence | verified |
| `PHASE0_SYMBOL_GREP_FULL.log` | Full symbol search capture | verified |
| `SUBPROCESS_AUDIT.log` | Full popen/std::system inventory | verified |
| `BENCHMARK_MEDIA_POLICY.md` | Benchmark evidence requirements and archive media handling | verified |

## How agents should use these docs

1. **Read `HANDOFF_VERIFICATION_REPORT.md` first** — it contains the verdict, allowed scope, and forbidden scope.
2. **Read `BASELINE_AUDIT.md`** for the captured local audit state at handoff time.
3. **Read `FILE_MAP.md`** for real source seams before writing code.
4. **Read `IMPLEMENTATION_DECISIONS.md`** for durable decisions that constrain implementation.
5. **Read `KNOWN_LIMITATIONS.md`** before making any public-facing change.
6. **Check `AUDIT_CLOSURE_MATRIX.md`** before closing an audit item.

## What not to do

- **Do not** treat these docs as permission to implement Temporal Forge.
- **Do not** begin Phase 2 feature work until Phase 1 contracts are merged.
- **Do not** invent APIs, classes, files, or seams that are not in `FILE_MAP.md`.
- **Do not** make public claims that exceed the evidence documented in `KNOWN_LIMITATIONS.md`.
- **Do not** modify existing backend, model management, or frame I/O behavior without explicit scoped permission.
- **Do not** mix feature work into documentation-only handoff PRs.
- **Do not** depend on the removed `.mp4` sample as a blocking artifact. The benchmark clip `benchmarks/ave_benchmark_990x540_20s.mp4` exists in the repo and can be used, but no specific media file is a blocking requirement.
- **Do not** demand evidence that cannot realistically exist (multi-machine validation, external tester corpus, AMD legal review, etc.).

## Phase boundaries

```
Phase 0 (current) ─── repo discovery, audit, handoff docs
    │
Phase 1A ─── mode/family metadata contracts
Phase 1B ─── fused planner behavior
Phase 1C ─── preview cache and timeline model
Phase 1D ─── workstation UI shell
Phase 1E ─── CLI/profile parity and release hardening
    │
Phase 2A ─── temporal abstractions only (experimental)
Phase 2B ─── providers/masks/reconstruction prototypes (experimental)
Phase 2C ─── masked AI refiner and evaluation harness (experimental)
    │
Phase 3 ─── AMD/community outreach
```

**Phase 2 must not start until Phase 1 contracts are merged or intentionally gated behind disabled experimental flags.**

## Temporal Forge status

Temporal Forge is **not implemented**. It is a Phase 2 experimental feature:
- No temporal engine abstractions exist in the codebase.
- No buffer providers, mask generators, or temporal reconstruction pipeline exists.
- The R2 roadmap archive (`amd-ve-temporal-forge-roadmap-conditional-handoff-r2/`) contains planning documents, schemas, and fixtures only.
- Temporal Forge must remain disabled by default, gated behind experimental flags, and clean-room.
- No FSR4, Redstone, or leaked/redacted AMD source may be used as a dependency.
