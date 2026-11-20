# Audit Closure Matrix

## Purpose

Track every audit finding from the Phase 0 verified handoff with fix status, evidence, and remaining gaps.

## Findings

| ID | Finding | Severity | Affected files | Fix status | Evidence | Tests/commands run | Remaining gap | Owner/future task |
|---|---|---|---|---|---|---|---|---|
| AC-001 | Contradictory release-state claims across public surfaces | high | README.md, docs/RELEASE_STATUS.md, docs/LIMITATIONS.md, CONTRIBUTING.md | fixed | All surfaces consistently say `v0.1.0-beta.1` beta prerelease. No surface claims GA or stable. | `grep -RIn "prerelease\|pre-release\|released\|production-ready" README.md docs .github CONTRIBUTING.md` — all results are correct beta framing | none | Monitor on future edits |
| AC-002 | Support-tier ambiguity between packaging targets and verified compatibility | high | docs/SUPPORT_TIERS.md, README.md | fixed | Four explicit tiers: verified primary, preview target, experimental, out of scope. README links to SUPPORT_TIERS.md. | Manual review of SUPPORT_TIERS.md — tier tables present and scoped | none | Monitor on future edits |
| AC-003 | Bundled custom MiGraphX runtime not clearly explained | medium | README.md, docs/RELEASE_STATUS.md, docs/PACKAGING.md | fixed | README line 13: "Bundled MiGraphX reality" explicitly disclosed. RELEASE_STATUS and PACKAGING repeat it. | `grep -RIn "bundled.*MiGraphX\|custom MiGraphX" README.md docs/` — 4+ surfaces disclose it | none | Monitor on future edits |
| AC-004 | Benchmark coverage limited to single reference system | medium | docs/BENCHMARKS.md | accepted (resource-bound) | BENCHMARKS.md explicitly states maintainer-verified on one reference system. Does not claim broader coverage. | Manual review | Broader benchmark corpus requires external testers | Beta program growth |
| AC-005 | Visual-quality proof limited — no comparison corpus | medium | docs/VALIDATION_AND_EVIDENCE.md, docs/LIMITATIONS.md | accepted (resource-bound) | Both docs explicitly state narrow quality evidence. No visual-quality superiority claims made. | Manual review | No visual-quality benchmark suite exists | Beta testing + screenshot collection |
| AC-006 | Local maintainer path leak in polished docs | high | docs/implementation/BASELINE_AUDIT.md | fixed | Replaced `/home/user/workspace/amd-ve` with `<repo-root>`. Only remaining local paths are in raw logs and internal skill configs. | `grep -RIn "/home/\|/mnt/" README.md docs .github CONTRIBUTING.md` — no polished-doc leaks remain | none | Monitor on future edits |
| AC-007 | `std::system()` shell invocations (6 locations) | medium | `src/backends/glsl_shader_backend.cpp:39,411`, `src/backends/vapoursynth_backend.cpp:82`, `tests/frame_io_pipe_tests.cpp:22`, `tests/process_observer_tests.cpp:20`, `tests/rgb_video_loop_tests.cpp:21` | accepted (bounded risk) | 3 in production code (GLSL backend + VapourSynth probe), 3 in test helpers. All use controlled command construction. `process_observer::normalizeShellExitCode()` wraps exit codes. Full inventory in SUBPROCESS_AUDIT.log. | `grep -RIn "std::system" src include tests tools` — 6 locations confirmed | Future hardening: consider exec-family calls | P2 or post-release |
| AC-008 | `popen()` calls (9 locations) | medium | `src/frame_io.cpp:824,1241,1442,1492,1532`, `src/ffmpeg_runner.cpp:213,307`, `src/process_observer.cpp:173,203` | accepted (bounded risk) | All `popen()` calls construct commands using `quoteShellArg()` for user-supplied values. `ffmpeg_runner.cpp:442` has `isSafeFfmpegToken()` for codec/preset/profile tokens. Frame I/O pipe handles are RAII-wrapped. Full inventory in SUBPROCESS_AUDIT.log. Subprocess risk is known and bounded, not absent. | `grep -RIn "popen(" src include tests tools` — 9 locations confirmed | Shell injection risk is bounded but not zero. Handoff must not claim zero exposure. | Future: replace popen with fork/exec |
| AC-009 | python3 PATH dependency for ONNX model preparation | low | `src/model_manager.cpp:911-912,955,1964-1965,2005` | accepted (documented) | Explicit error messages when python3 not found. `docs/LIMITATIONS.md` documents the dependency. | `grep -n "python3.*not found" src/model_manager.cpp` — 2 locations with clear errors | Packaging could bundle Python helper or use self-contained alternative | Future improvement |
| AC-010 | unzip PATH dependency for archive extraction | low | `src/model_manager.cpp:1563,1568-1569,1584,1592,1605` | accepted (documented) | Explicit error when unzip not found. `docs/LIMITATIONS.md` documents the dependency. | `grep -n "unzip.*not found" src/model_manager.cpp` — 1 location with clear error | Future: use libarchive or zlib-based extraction | Future improvement |
| AC-011 | libcurl HTTPS-only download with TLS verification | info | `src/model_manager.cpp:1436-1555` | accepted (already hardened) | CURLOPT_SSL_VERIFYPEER=1, SSL_VERIFYHOST=2, HTTPS-only protocols, 30s connect timeout, 1800s total timeout, redirect limits. | `grep -n "CURLOPT_SSL\|CURLOPT_PROTOCOLS\|CURLOPT_TIMEOUT" src/model_manager.cpp` — TLS verification confirmed | none | Already secure |
| AC-012 | FFmpeg token sanitization prevents shell injection | info | `src/ffmpeg_runner.cpp:442` | accepted (already hardened) | `isSafeFfmpegToken()` validates codec, preset, and profile tokens before passing to FFmpeg. | `grep -n "isSafeFfmpegToken" src/ffmpeg_runner.cpp` — present at line 442 | none | Already secure |
| AC-013 | Temporal Forge not validated (does not exist) | n/a | — | accepted | No temporal engine code exists in the repo. R2 archive is planning docs only. | `grep -RIn "Temporal Forge" src include` — zero results in source | Phase 2 experimental only | Phase 2A+ |
| AC-014 | FidelityFX CAS reference in filter descriptions | low | `src/filter_catalog.cpp:1460`, `src/model_catalog.cpp:694` | accepted (descriptive only) | "FidelityFX CAS-style" is a descriptive reference to the public CAS algorithm concept, not a claim of FidelityFX SDK integration or AMD endorsement. Both entries are FFmpeg filter pipelines, not AMD SDK code. | Manual review of context | none | No action needed |
| AC-015 | No broad distro validation beyond Arch on reference system | medium | docs/SUPPORT_TIERS.md, docs/LIMITATIONS.md | accepted (resource-bound) | SUPPORT_TIERS.md separates verified primary from preview target. LIMITATIONS.md states single verified environment. | Manual review | External tester corpus needed | Beta program growth |
| AC-016 | No external tester corpus | low | docs/BETA_TESTING_PROGRAM.md | accepted (resource-bound) | Beta testing program exists with structured reporting templates. Still growing. | Manual review | Community participation needed | Beta program growth |
| AC-017 | No comprehensive visual-quality corpus | medium | docs/VALIDATION_AND_EVIDENCE.md | accepted (resource-bound) | Validation doc explicitly states narrow evidence. No quality superiority claims. | Manual review | Quality corpus requires resources | Future work |
| AC-018 | No full public website | low | docs/specification-pack/ | accepted (non-blocking) | Website specification pack exists with requirements, constraints, and content-proof matrix. Site is not deployed. | `ls docs/specification-pack/` — 9 specification docs present | Deployment requires resources | Future deployment |
| AC-019 | Benchmark clip exists in repo | info | `benchmarks/ave_benchmark_960x540_20s.mp4` | n/a | The benchmark clip referenced by docs and scripts exists at the expected path. | `ls benchmarks/*.mp4` — confirmed present | none | No action needed |
| AC-020 | R2 schema validation passes | info | `amd-ve-temporal-forge-roadmap-conditional-handoff-r2/` | verified | 3 valid fixtures accepted, 6 invalid fixtures rejected. | `python3 tests/validate_schemas.py` — exit code 0 | Schema-only validation, not runtime integration | Phase 1A integration |
| AC-021 | No PR template in .github | low | `.github/` | open | No `.github/PULL_REQUEST_TEMPLATE.md` exists. Issue templates exist but PR template does not. | `ls .github/PULL_REQUEST_TEMPLATE* 2>/dev/null` — not found | PRs lack structured review checklist | Maintainer action: create PR template |

## Summary

| Status | Count |
|---|---:|
| fixed | 4 |
| accepted (bounded/documentable) | 11 |
| accepted (resource-bound) | 5 |
| accepted (already hardened) | 2 |
| open (non-blocking) | 1 |
| n/a / info | 2 |
| AC-022 | Subprocess inventory previously understated popen/system exposure | high | docs/implementation/BASELINE_AUDIT.md, HANDOFF_VERIFICATION_REPORT.md | fixed | SUBPROCESS_AUDIT.log created with full inventory: 9 popen + 6 std::system. All prior "no popen" or understated claims removed. | `cat docs/implementation/SUBPROCESS_AUDIT.log` — verified | none | Monitor on future edits |
| AC-023 | Benchmark media may be altered in transfer archives | medium | docs/implementation/BASELINE_AUDIT.md, KNOWN_LIMITATIONS.md, HANDOFF_VERIFICATION_REPORT.md, PHASE0_COMMAND_OUTPUTS.log | fixed | Archive media note added to all relevant files. Benchmark media policy documented. | Manual review | none | Monitor on future edits |
| AC-024 | First-agent scope was ambiguous between docs-only and Phase 1A | high | docs/implementation/HANDOFF_VERIFICATION_REPORT.md, IMPLEMENTATION_DECISIONS.md | fixed | Phase 1A now gated behind maintainer approval of Phase 0 handoff PR. Explicit wording added. | Manual review | none | Monitor on future edits |
| AC-025 | Temporal Forge lock needed explicit documentation | high | HANDOFF_VERIFICATION_REPORT.md, IMPLEMENTATION_DECISIONS.md, REMAINING_AUDIT_RISKS.md | fixed | Temporal Forge Lock section added. IMPLEMENTATION_DECISIONS D-20260507-15 added. | Manual review | none | Monitor on future edits |

## Summary

| Status | Count |
|---|---:|
| fixed | 8 |
| accepted (bounded/documentable) | 11 |
| accepted (resource-bound) | 5 |
| accepted (already hardened) | 2 |
| open (non-blocking) | 1 |
| n/a / info | 2 |
| **total** | **29** |

No handoff-blocking findings remain.
One open non-blocking item: AC-021 (PR template).
