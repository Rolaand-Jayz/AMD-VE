# Validation Matrix

## Purpose

This matrix defines what must be tested before each milestone is considered real. Passing means behavior is implemented, not merely mocked.

---

# Phase 0 validation

| Check | Required evidence |
|---|---|
| Repo state captured | commit hash, branch, dirty state |
| Build command known | exact command and result |
| Test command known | exact command and result |
| GUI launch known | screenshot or log |
| CLI help known | captured output or command result |
| Backend inventory known | MiGraphX/ROCm/Vulkan/NCNN/FFmpeg availability |
| Model inventory known | count, families if available, broken models |
| File map complete | real source paths or explicit `not found` |

---

# Phase 1A — Mode/family metadata validation

## Unit tests

- Mode ids unique.
- Required modes exist.
- Experimental mode cannot be enabled accidentally.
- Mode serializes/deserializes.
- Family ids unique.
- Visible families query excludes advanced-only models.
- Advanced/raw model query includes demoted models.
- Old manifests load.
- Invalid new metadata produces actionable errors.

## Smoke tests

- GUI opens mode list.
- CLI can dry-run at least one mode/family profile.
- Advanced/raw model selection remains possible.

---

# Phase 1B — Fused planner validation

## Required tests

| Test | Expected result |
|---|---|
| non_fused_two_capabilities | ordinary stage nodes |
| fused_nonselective_exact | one fused group |
| fused_nonselective_partial | fallback/error diagnostic |
| fused_selective_subset | one fused group |
| unsupported_capability | actionable error/fallback diagnostic |
| raw_model_advanced_path | raw/ordinary node, no family fusion |
| planner_hash_stable | same normalized intent gives same hash |
| planner_hash_changes | capability/model/backend/strength changes alter hash |

## Smoke tests

- CLI planner preview shows fused group.
- GUI advanced foldout shows same planner result.

---

# Phase 1C — Preview cache/timeline validation

## Unit tests

- Cache key stable for identical config.
- Cache key changes for source/model/strength/backend/precision/schema changes.
- Missing frame marks invalid.
- Cancelled render marks warning/invalid, not cached.
- Segment ranges validate timebase.

## Integration tests

- Render a short preview segment.
- Scrub cached frames.
- Change strength and verify cache invalidation.
- Cancel preview and verify state.
- Full render path still works.

---

# Phase 1D — Workstation UI validation

## Interaction tests

- Main window has top rail, central preview, right inspector, bottom timeline.
- Split-wipe divider drags.
- Double-click resets divider.
- Side-by-side mode works.
- Zoom/pan are synchronized.
- Missing enhanced frame shows uncached state.
- Family cards show purpose/capabilities/backend/install state.
- Non-selective fused family does not expose misleading capability toggles.

## Accessibility/UX checks

- Important render/preview controls are reachable by keyboard or menus.
- Long operations expose progress/cancel.
- Errors are user-facing and log-facing.

---

# Phase 1E — CLI/profile/release validation

## Required checks

- GUI-created profile runs in CLI.
- CLI-created profile can be opened or previewed in GUI/planner.
- Existing CLI workflows remain compatible or have documented migration.
- Package/build scripts still work or failures are documented.
- README/user guide updated.
- Troubleshooting notes added.

---

# Phase 2A — Temporal abstraction validation

## Unit tests

- Temporal frame window accepts history/current/future.
- Future frames optional.
- Scene reset flag propagates.
- Packet debug dump writes expected metadata.
- Null providers fail gracefully.
- Provider output dimensions/timebase validate.

## Guard checks

- Temporal Forge disabled unless experimental flag/setting is enabled.
- Production modes do not route through temporal code accidentally.

---

# Phase 2B — Providers/reconstruction validation

## Motion provider

- Synthetic horizontal shift produces correct direction.
- Synthetic vertical shift produces correct direction.
- Scene cut triggers reset.
- Debug visualization exports.

## Depth provider

- Existing depth/SBS output can be adapted.
- Depth normalized to 0..1.
- Missing depth model degrades gracefully.
- Edge/discontinuity mask exports.

## Reconstruction baseline

- Temporal-only render completes on short clip.
- Confidence map exports.
- Disocclusion handling visible in debug output.
- Protected regions are preserved conservatively.

---

# Phase 2C — AI refiner/evaluation validation

## AI refiner

- High-confidence temporal regions are mostly preserved.
- AI-allowed mask controls influence.
- Protected text/UI/subtitle regions are conservative by default.
- Temporal-vs-AI slider changes output predictably.

## Evaluation harness

- Benchmark manifest loads.
- At least one clip per category can be registered.
- Runtime/frame is recorded.
- VRAM/system memory is recorded if available.
- Temporal consistency metric or proxy is recorded.
- A/B exports are generated.

---

# Global no-go checks

A milestone must not be marked complete if:

- build is broken without documented blocker
- test failures are unexplained
- GUI and CLI disagree on pipeline planning
- cache reports success with missing/incomplete frames
- Phase 2 code is enabled in production accidentally
- redacted/leaked source is introduced
- user-facing claims exceed benchmark evidence

---

# Executable validation rows

Rows marked `Phase 0 discover` must be replaced with real project commands after `FILE_MAP.md` exists. Until then, they are not passable; they are pending discovery.

| Check ID | Phase | Command or manual step | Fixture/input | Expected output | Evidence path | Pass/fail rule | CI/manual |
|---|---|---|---|---|---|---|---|
| VAL-SCHEMA-001 | 1A/1C | `python tests/validate_schemas.py` | `fixtures/schema/{valid,invalid}` | valid fixtures accepted; invalid fixtures rejected | terminal log or CI artifact | fail on any unexpected accept/reject | CI |
| VAL-P0-001 | 0 | `git rev-parse --show-toplevel && git status --short && git rev-parse HEAD` | target repo | repo root, dirty state, commit hash captured | `docs/implementation/BASELINE_AUDIT.md` | fail if commit/dirty state missing | manual/CI |
| VAL-P0-002 | 0 | Phase 0 file-map grep/ripgrep commands from `START_HERE_IMPLEMENTATION_READY.md` | target repo | real files/classes/functions or explicit `not found` | `docs/implementation/FILE_MAP.md` | fail if roadmap area is blank or invented | manual |
| VAL-P1A-001 | 1A | Phase 0 discover: project unit-test command for mode/family contracts | mode/family fixtures | required built-in modes resolve; old manifests load | test log | fail if old manifests break or Temporal Forge enabled by default | CI |
| VAL-P1B-001 | 1B | Phase 0 discover: planner fixture test command | non-fused/fused/selective/raw fixtures | expected planner nodes and diagnostics | test log | fail if capability disappears silently | CI |
| VAL-P1C-001 | 1C | Phase 0 discover: preview cache unit/integration test command | short redistributable clip or synthetic frames | cache key stable/invalidates correctly | test log + cache manifest | fail if cached state has missing frames | CI |
| VAL-P1C-002 | 1C | Manual: render 3-5s preview segment, scrub timeline, change strength, cancel preview | short redistributable clip | cached segment scrubs; strength invalidates; cancel does not fake success | screenshot/log/cache manifest | fail if user cannot identify cache state | manual |
| VAL-P1D-001 | 1D | Manual keyboard-only pass through preview/render/cancel/timeline/advanced foldout | workstation UI | all primary actions reachable and visible focus present | screen recording or notes | fail if primary workflow requires mouse-only control | manual |
| VAL-P1E-001 | 1E | Phase 0 discover: CLI dry-run/profile round-trip command | GUI-created profile and CLI-created profile | equivalent planner output | profile files + dry-run logs | fail if GUI/CLI disagree | CI/manual |
| VAL-LICENSE-001 | 1E/2 | Fill dependency/model/dataset/SDK registries | release candidate | no unknown/high-risk unwaived entries | `docs/implementation/legal/` | fail if registry incomplete or waived without owner/date/reason | manual gate |
| VAL-P2A-001 | 2A | Phase 0 discover: temporal packet unit tests | synthetic frame windows | history/current/future, scene reset, null providers behave safely | test log + debug manifest | fail if production path routes through temporal code | CI |
| VAL-P2B-001 | 2B | Prototype benchmark command | synthetic shifts + short clips | motion direction, scene reset, confidence map, debug overlays | `benchmarks/out/<run_id>/` | fail if debug artifacts missing | CI/manual |
| VAL-P2C-001 | 2C | Prototype evaluation harness command | licensed benchmark manifest | spatial-only, temporal-only, AI-only, hybrid metrics reported | `benchmarks/out/<run_id>/report.md` | fail if runtime/frame, memory, hardware, clip ID, and baseline comparisons missing | manual/CI |

## Evidence naming convention

Use this pattern for benchmark and validation artifacts:

```text
validation/<phase>/<check_id>/<YYYYMMDD-HHMM>-<commit>/
```

Every evidence folder should include the command, environment, commit, result, and known limitations.

| VAL-MIGRATION-001 | 1A/1E | Phase 0 discover: legacy manifest/profile migration tests | real legacy/custom manifest fixtures plus `fixtures/migration/*.example.json` | old/custom/raw models load or warn explicitly; no silent remap to wrong family | test log + converted metadata snapshots | fail if existing model/profile behavior silently changes | CI/manual |
