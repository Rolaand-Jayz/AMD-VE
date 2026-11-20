# Revision Changelog — R2 Conditional Implementation Handoff

## Why this revision exists

This revision applies the evaluator findings from the previous pack review. The central correction: the pack was strong, but it called itself implementation-ready too broadly. It is now explicitly **conditional implementation-ready after Phase 0 repository discovery**.

## Major changes

| Finding | Change |
|---|---|
| F-001 readiness overstated | Replaced bare `implementation_ready=true` with a structured readiness object; updated README, Start Here, and readiness review language. |
| F-002 schemas under-enforced | Strengthened schemas; added positive/negative fixtures and `tests/validate_schemas.py` semantic validator. |
| F-003 external facts uncited | Added `docs/EXTERNAL_ASSUMPTIONS_LEDGER.md` with official AMD/GPUOpen source URLs, access date, confidence, and action rules. |
| F-004 Temporal Forge feasibility vague | Added hardware tiers, clip/resolution envelope, performance budgets, report fields, and stop criteria. |
| F-005 tasks too large | Added suffix-based split cards and patch scope refinement table. |
| F-006 validation not executable enough | Added executable validation rows with commands/manual steps, evidence paths, pass/fail rules, and CI/manual designation. |
| F-007 accessibility thin | Added keyboard map, focus order, accessibility requirements, and responsive layout contract. |
| F-008 license controls not operationalized | Added dependency/model/dataset/SDK registry templates and merge/release gates. |
| F-009 migration compatibility underspecified | Added schema/fixture validation groundwork and release gate expectations; target-repo legacy fixtures still need Phase 0 discovery. |
| F-010 traceability thin | Added stable requirement IDs linking PRD, contracts, tasks, schemas/fixtures, validation, and release gates. |

## Remaining honest limitations

- The target AMD-VE source repository is still not bundled, so exact production file paths remain Phase 0 work.
- Some compatibility fixtures, especially legacy AMD-VE manifests, must be created from real repository examples after discovery.
- Temporal Forge performance budgets are prototype thresholds, not proven benchmark results.
- This pack is not legal advice; disputed SDK/model/dataset rights still require qualified review.

## Next best action

Run Phase 0 in the target AMD-VE repository, then start with mode/family contracts and schema fixtures before UI or Temporal Forge implementation.

## R2 addendum — migration compatibility

Added `docs/28_LEGACY_MIGRATION_CONTRACT.md` plus migration example fixtures. This closes the gap where the pack said old manifests/custom models must keep working but did not define conversion behavior, warnings, or round-trip expectations.
