# Remaining Audit Risks

## Purpose

Track what is fixed, open, deferred, accepted, or not applicable.

A handoff can be ready with open product risks.
A handoff cannot be ready with hidden product risks.

## Risk Table

| ID | Issue | Status | Evidence | Blocks handoff? | Blocks release? | Next action |
|---|---|---|---|---|---|---|
| R1 | Contradictory release-state claims | fixed | All surfaces consistently say `v0.1.0-beta.1` beta prerelease. No surface claims GA or stable. | no | no | Monitor on future edits |
| R2 | Support-tier ambiguity | fixed | SUPPORT_TIERS.md defines four explicit tiers. README links to it. | no | no | Monitor on future edits |
| R3 | Custom MiGraphX explanation unclear | fixed | README, RELEASE_STATUS, PACKAGING all disclose bundled custom MiGraphX runtime | no | no | Monitor on future edits |
| R4 | Benchmark coverage narrow | accepted | docs/BENCHMARKS.md states evidence is maintainer-verified on one reference system only | no | no | Broader benchmark corpus is resource-bound |
| R5 | Visual-quality proof limited | accepted | docs/LIMITATIONS.md and docs/VALIDATION_AND_EVIDENCE.md state narrow quality evidence | no | maybe | Beta program should grow quality evidence |
| R6 | Maintainer-local path leak | fixed | BASELINE_AUDIT.md local path replaced with `<repo-root>`. No local paths in polished docs. | no | no | Monitor on future edits |
| R7 | Shell subprocess hardening | accepted | SUBPROCESS_AUDIT.log records 9 popen() calls and 6 std::system() calls; risk is bounded but not absent | no | maybe | Future hardening may replace shell-dependent paths where practical or document why they remain. |
| R8 | PATH dependency disclosure | fixed | docs/LIMITATIONS.md documents python3 and unzip PATH requirements | no | no | Monitor if preparation path changes |
| R9 | First-run progress transparency | accepted | Progress emitted during migraphx-driver work; coarser than ideal | no | no | Future: async compilation with finer progress |
| R10 | Backend support-tier discipline | fixed | SUPPORT_TIERS.md explicitly separates verified primary, preview, experimental, out-of-scope backends | no | no | Monitor on future backend additions |
| R11 | Temporal Forge not validated | accepted | Temporal Forge does not exist in the codebase. R2 archive contains planning docs only. | no | yes | Keep disabled/gated behind experimental flags |
| R12 | No broad distro validation | accepted | Only Arch on reference system is verified. Preview package targets exist but are not validated. | no | maybe | Record as limitation; beta program expands |
| R13 | No external tester corpus | accepted | Beta testing program exists but is still growing. Most proof is maintainer-verified. | no | no | Record as limitation |
| R14 | No comprehensive visual-quality corpus | accepted | No visual-quality benchmark suite exists. Quality claims stay narrow and specific. | no | maybe | Record as limitation |
| R15 | No full website | accepted | Website specification pack exists in docs/specification-pack/. Site is not deployed. | no | no | Record as non-blocking; deploy when ready |

Definitions:
- **Blocks handoff?** means the next agent cannot proceed safely.
- **Blocks release?** means public product maturity is incomplete.
- **Accepted** limitations are allowed only when explicitly documented.
- No handoff-blocking issues remain.

## Additional Risk Entries

| ID | Issue | Status | Evidence | Blocks handoff? | Blocks release? | Next action |
|---|---|---|---|---|---|---|
| R16 | Subprocess inventory must not claim zero exposure | fixed | SUBPROCESS_AUDIT.log: 9 popen + 6 std::system documented. All prior "no popen" claims removed. | no | no | Monitor on future edits |
| R17 | Benchmark media may be altered in transfer archives | accepted | Archive media note added to BASELINE_AUDIT, KNOWN_LIMITATIONS, HANDOFF_REPORT | no | no | Agents must not claim reproducibility from archive media state |
| R18 | First-agent scope was ambiguous between docs-only and Phase 1A | fixed | HANDOFF_REPORT now gates Phase 1A behind maintainer approval of Phase 0 PR | no | no | Monitor on future edits |
| R19 | Temporal Forge lock must be explicit in handoff docs | fixed | Temporal Forge Lock section added to HANDOFF_REPORT, IMPLEMENTATION_DECISIONS, this file | no | no | Monitor on future edits |
