# Claim Audit Grep Log

## Purpose

Search for stale, overbroad, or risky public claims in polished documentation.

## Command

    grep -RIn "production-ready\|production ready\|stable release\|fully supported\|all distros\|works on all\|guaranteed\|Topaz replacement\|Topaz alternative\|prerelease\|pre-release\|released\|verified\|supported\|FSR-class\|Temporal Forge\|/home/\|/mnt/" README.md docs .github CONTRIBUTING.md 2>/dev/null

## Raw Output

See:
docs/implementation/PHASE0_COMMAND_OUTPUTS.log

## Findings

| Finding | File | Line | Risk | Action |
|---|---|---|---|---|
| "beta prerelease" | README.md | 7, 13 | none — accurate beta state claim | accepted |
| "verified" (primary system) | README.md | 11 | none — scoped to specific hardware | accepted |
| "supported AMD GPU" | README.md | 148 | low — generic requirement, not a support promise | accepted |
| "unsupported ROCm distributions" | README.md | 182 | none — correctly frames as best-effort | accepted |
| "Temporal Forge" | docs/implementation/IMPLEMENTATION_DECISIONS.md | 26, 30, 38 | none — implementation planning doc, not public promise | accepted |
| "FSR-class ideas" | docs/implementation/IMPLEMENTATION_DECISIONS.md | 38 | none — planning context with IP-risk guard | accepted |
| "Temporal Forge" (R2 planning docs) | amd-ve-temporal-forge-roadmap-conditional-handoff-r2/ | multiple | none — planning archive, not in polished docs | accepted |
| "guaranteed fallback path" | docs/GOLD_STANDARD_FOR_IMPLEMENTATION.md | 383 | low — FFmpeg is genuinely always available | accepted |
| "verified 'first ever' claim" | docs/GOLD_STANDARD_FOR_IMPLEMENTATION.md | 542–544 | none — explicitly refuses the claim | accepted |
| "prerelease" / "pre-release" | docs/RELEASE_STATUS.md, docs/RELEASING.md, docs/LIMITATIONS.md, docs/PACKAGING.md | multiple | none — consistent beta state | accepted |
| "verified primary" | docs/SUPPORT_TIERS.md | 9 | none — correctly scoped to single system | accepted |
| "supported fallback" | docs/SUPPORT_TIERS.md | 19 | none — scoped to verified system | accepted |
| "supported" (control bindings) | docs/CUSTOM_MODEL_MANIFEST.md | 67 | none — refers to manifest syntax, not platform support | accepted |
| "/home/user/.agents/skills/" | .github/skills/shared-skill-index/SKILL.md | 17 | low — internal skill config, not polished public doc | accepted |
| "FSR-class" | R2 planning docs | n/a | none — planning archive, not public doc | accepted |
| "guaranteed" | docs/PARITY_PLAN.md | 10 | none — refers to FFmpeg fallback being always available, which is true | accepted |

## Accepted Remaining Claims

All findings above are accepted because:

1. **Beta prerelease state**: All surfaces consistently say `v0.1.0-beta.1` beta prerelease. No surface claims GA/stable.
2. **Verified primary scope**: "Verified" is always scoped to the specific reference system (Arch + 7800X3D + 7900 GRE).
3. **Support boundaries**: SUPPORT_TIERS.md and README.md clearly separate verified primary, preview target, experimental, and out-of-scope.
4. **Bundled MiGraphX**: Explicitly disclosed as custom runtime, not stock upstream.
5. **FFmpeg "guaranteed"**: Refers to the fallback always being available (true), not to quality guarantees.
6. **Temporal Forge / FSR-class**: Appears only in implementation planning docs and the R2 archive, never in polished public surfaces as a shipping feature.
7. **GOLD_STANDARD "first ever"**: Explicitly refuses the claim, documents why it should not be made.

## Removed or Reworded Claims

| Finding | Action |
|---|---|
| Local path `/home/user/workspace/amd-ve` in BASELINE_AUDIT.md | Fixed: replaced with `<repo-root>` placeholder |

No other claims required removal or rewording.

## Local Path Review

Latest scan: 2026-05-07 (post-contradiction-fix)

Command: `grep -RIn "/home/\|/mnt/" README.md docs .github CONTRIBUTING.md 2>/dev/null`

Results:

| File | Path | Context | Verdict |
|---|---|---|---|
| docs/implementation/AUDIT_CLOSURE_MATRIX.md | `/home/user/workspace/amd-ve` | Historical fix record | accepted — raw evidence log |
| docs/implementation/CLAIM_AUDIT_GREP_LOG.md | `/home/user/.agents/skills/`, grep patterns | Self-referential grep commands + fix records | accepted — raw evidence log |
| docs/implementation/PHASE0_COMMAND_OUTPUTS.log | `/home/user/workspace/amd-ve` | Raw `git rev-parse` output | accepted — raw command output |
| docs/implementation/SUBPROCESS_AUDIT.log | `/home/user/workspace/amd-ve` | Raw grep output in subprocess audit | accepted — raw command output |
| docs/implementation/PUBLIC_TRUTH_SURFACE_MATRIX.md | `/home/user/.agents/skills/` | Internal skill config reference | accepted — raw evidence record |
| .github/skills/shared-skill-index/SKILL.md | `/home/user/.agents/skills/` | Internal skill index path | accepted — internal tooling config, not polished public doc |

All remaining local paths exist only in raw command output logs, implementation evidence records, or internal tooling configuration. No local maintainer paths remain in polished public documentation.

Rules applied:
- Raw command output logs may contain local paths. **Allowed.**
- Polished docs must not present local paths as canonical repo paths. **No violations found.**
- If local paths remain only as raw evidence, marked accepted with explanation. **Confirmed.**
