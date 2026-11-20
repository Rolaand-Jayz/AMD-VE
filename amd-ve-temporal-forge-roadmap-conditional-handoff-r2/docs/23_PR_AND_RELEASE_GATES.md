# PR and Release Gates

## PR size rule

Prefer PRs that complete one task ticket or one tight group of adjacent subtasks. Avoid mixing UI polish, planner behavior, and Temporal Forge internals in one PR.

## Required PR description

Every PR must use this structure:

```markdown
## Summary

## Task IDs

## Files changed

## Existing behavior preserved

## New behavior

## Tests run

## Screenshots / CLI output

## Known limitations

## Follow-up tasks

## Risk review
- Build risk:
- UX risk:
- Performance risk:
- Licensing/IP risk:
- Backward compatibility risk:
```

## PR acceptance gates

A PR may be accepted only when:

- The task id is named.
- The changed files match the task scope.
- Tests or smoke checks are listed.
- User-visible changes are documented.
- Known limitations are explicit.
- No unrelated rewrites are included.
- No leaked/redacted source appears.

## Phase 1 release gate

Phase 1 is release-ready only when:

- Baseline audit exists.
- File map exists.
- Mode/family metadata tests pass.
- Fused planner tests pass.
- Preview cache tests pass.
- GUI workstation shell launches.
- Segment preview works on a short clip.
- Split-wipe comparison works.
- CLI/profile parity works.
- Advanced/raw model access still works.
- README/user guide updated.
- Migration/troubleshooting docs updated.
- Packaging smoke is run or explicitly waived.

## Phase 2 prototype gate

Temporal Forge prototype may be shown as experimental only when:

- Temporal abstractions are tested.
- Provider interfaces are tested.
- At least one reference motion provider exists.
- At least one depth/fallback provider exists.
- Debug artifacts export.
- Temporal-only baseline renders a short clip.
- Claims are framed as prototype behavior.

## Phase 2 production gate

Temporal Forge may not be called production-ready until:

- Evaluation harness exists.
- Benchmark clips are documented.
- Temporal-only, AI-only, and hybrid comparisons exist.
- Runtime and memory are measured.
- Failure cases are documented.
- Protected text/UI/subtitle behavior is validated.
- The clean-room/IP review is complete.

## Rollback expectations

Every PR should be revertible without data loss. For schema changes:

- bump schema version
- keep compatibility reader where practical
- document migration behavior
- never silently corrupt old user profiles/cache

## Branch naming

Recommended:

```text
phase0/baseline-audit
phase1/mode-family-contracts
phase1/fusion-planner
phase1/preview-cache
phase1/workstation-ui-shell
phase1/cli-profile-parity
phase2/temporal-packets
phase2/provider-interfaces
phase2/reference-motion-provider
phase2/temporal-baseline
phase2/masked-ai-refiner
```

## Added merge gates from implementation-readiness review

A PR is blocked unless:

- Phase 0 has produced `BASELINE_AUDIT.md` and `FILE_MAP.md`, unless the PR is Phase 0 itself.
- Schema changes update positive/negative fixtures and keep `python tests/validate_schemas.py` passing.
- User-facing mode/family/profile changes include CLI/GUI parity notes.
- New dependency/model/dataset/SDK usage updates the appropriate license/provenance registry.
- Temporal Forge code remains disabled/experimental unless the current release gate explicitly permits it.

## License/provenance release gate

Before Phase 1 release, dependency licenses must be recorded or waived.

Before any Phase 2 public demo or release, dependency, model, dataset/clip, and SDK usage registries must be complete or explicitly waived with owner/date/reason.

A waiver is not valid without:

- reviewer,
- date,
- affected component,
- risk accepted,
- expiry or follow-up task.
