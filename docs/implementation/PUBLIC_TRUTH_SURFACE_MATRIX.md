# Public Truth Surface Matrix

## Purpose

This matrix checks whether public-facing docs make consistent claims about release state, support scope, MiGraphX bundling, limitations, and evidence.

## Matrix

| Surface | Release state | Support tier | MiGraphX wording | Limitations linked? | Benchmark/evidence wording | Status | Fix needed |
|---|---|---|---|---|---|---|---|
| README.md | `v0.1.0-beta.1` beta prerelease — correctly stated | Verified primary = Arch + 7800X3D + 7900 GRE; package targets broader than verified | Bundled custom MiGraphX runtime explicitly disclosed | Links to RELEASE_STATUS, SUPPORT_TIERS, LIMITATIONS, VALIDATION_AND_EVIDENCE | "Primary verified backend path: MiGraphX + FFmpeg" — bounded to reference system | aligned | none |
| docs/RELEASE_STATUS.md | Beta prerelease `v0.1.0-beta.1` — correctly stated | Primary verified = Arch + reference system; preview targets are validation targets, not broad proof | Bundled custom MiGraphX — stated as real but easy to miss | "What is not true yet" section explicitly present | Maintainer benchmark snapshot + test surface; not broader than evidence | aligned | none |
| docs/SUPPORT_TIERS.md | n/a (support tiers, not release state) | Explicit tier table: verified primary, preview target, experimental, out of scope | Bundled vs host-required boundary section present | Linked from README | "Not proof by itself" for packaging | aligned | none |
| docs/LIMITATIONS.md | Beta prerelease stated | Only verified primary = Arch + 7800X3D + 7900 GRE | Source-build depends on python3/unzip | Self-documenting | Benchmark evidence narrow; maintainer-verified | aligned | none |
| docs/VALIDATION_AND_EVIDENCE.md | n/a | Maintainer-verified vs community-submitted tiers defined | n/a | n/a | Honest about narrow evidence; "does not prove cross-distro compatibility" | aligned | none |
| docs/PACKAGING.md | Beta prerelease `v0.1.0-beta.1` | Primary verified stack stated; preview targets explicitly not validated | n/a | "Packaging reach is not the same thing as verified compatibility" present | Checksum sidecars; SHA256SUMS manifest | aligned | none |
| docs/RELEASING.md | Beta prerelease process doc | Primarily verified on Arch + reference hardware | n/a | Support messaging check in pre-publish list | Consistent with RELEASE_STATUS | aligned | none |
| CONTRIBUTING.md | Beta prerelease stated; "ONLY VERIFIED ON ARCH LINUX + RX 7900 GRE" | Explicit support boundary | n/a | n/a | n/a | aligned | none |
| .github/ISSUE_TEMPLATE/* | Bug report template: beta prerelease notice + verified environment statement | config.yml links to support tiers | n/a | n/a | n/a | aligned | none |

## Mismatches Found

| Surface | Mismatch | Risk | Action |
|---|---|---|---|
| (none) | — | — | — |

All inspected surfaces are consistent. No unhandled mismatches found.

## Metadata Access

Repo metadata (GitHub description, topics, homepage) requires GitHub UI or `gh` CLI. It cannot be edited from repository files directly.

Required maintainer action:
- **description**: Currently set. Verify it does not overstate release state.
- **topics**: Verify `amdgpu`, `rocm`, `migraphx`, `video-enhancement` are present. Do not add `production-ready`, `stable`, or `temporal-forge`.
- **homepage**: If set, verify it does not claim broader compatibility than the repo docs.

The `.github/skills/shared-skill-index/SKILL.md` file contains a local path (`/home/rolaandjayz/.agents/skills/`). This is an internal skill index, not a polished public doc. Accepted as internal tooling configuration.
