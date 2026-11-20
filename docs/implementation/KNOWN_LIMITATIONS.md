# Known Limitations

Generated: 2026-05-07
Source: R2 conditional handoff pack — Phase 0

| ID | Limitation | Area | Severity | Workaround | Target fix |
|---|---|---|---:|---|---|
| KL-001 | ROCm/HIP ONNX Runtime execution provider not available on reference system | Backend | 3 | Use MiGraphX or Vulkan/NCNN paths | Install ONNX Runtime build with ROCM EP; non-blocking for Phase 1 |
| KL-002 | hipInfo not on PATH | Diagnostics | 1 | Install HIP runtime utilities or add to PATH | Fix environment setup; does not affect functionality |
| KL-003 | No mode/family/preset metadata system | Metadata | 5 | Use raw `--stage` CLI flags and GUI stage controls | Phase 1A |
| KL-004 | No fused stage planner (multi-capability single-pass) | Planner | 4 | Stages run sequentially through separate models | Phase 1B |
| KL-005 | No preview cache or timeline UI | Preview | 5 | Use `--preview --preview-duration` for short clip preview only | Phase 1C |
| KL-006 | No workstation-class UI layout | GUI | 4 | Current GUI is functional but basic | Phase 1D |
| KL-007 | No portable profile serialization (CLI/GUI parity) | CLI/GUI | 4 | GUI uses QSettings; CLI has no profile concept | Phase 1E |
| KL-008 | First-run MiGraphX compilation takes minutes | Runtime UX | 3 | Pre-compile models; emit progress during `migraphx-driver` | Future: async compilation with progress bar |
| KL-009 | Source-build model preparation requires `python3` and `unzip` in PATH | Dependencies | 2 | Ensure tools are installed | Future: bundle preparation tools or use self-contained extraction |
| KL-010 | Single verified environment (CachyOS/Arch + 7800X3D + 7900 GRE) | Validation | 3 | Beta testing program for broader verification | Beta testing and community validation |
| KL-011 | No temporal engine abstractions | Phase 2 | N/A | Not yet in scope | Phase 2A (experimental only) |
| KL-012 | Vulkan↔HIP interop bridge exists but default MiGraphX path uses host staging | Performance | 2 | HIP-mapped host staging optimization available when conditions allow | Future: extend zero-copy paths |
| KL-013 | Public visual-quality evidence narrower than throughput/packaging evidence | Validation | 3 | Share specific quality comparisons, not broad claims | Beta testing and screenshot collection |
| KL-014 | GUI is Linux-only (Qt6 + X11/Wayland) | Platform | 2 | Use CLI for headless; GUI requires display server | Out of scope: Windows/macOS not targeted |
| KL-015 | Subprocess exposure exists: SUBPROCESS_AUDIT.log records 9 popen() calls and 6 std::system() calls | Security | 2 | Shell quoting and token validation present; risk is bounded but not zero; accepted for handoff readiness but remains a product hardening risk | Future: replace with fork/exec where practical |
| KL-016 | No PR template in `.github/` — PRs lack structured review checklist | Process | 1 | Manual review | Maintainer action: create PR template |
| KL-017 | FidelityFX CAS references in filter/model catalog are descriptive only — no AMD SDK integration or endorsement implied | Legal | 1 | References describe FFmpeg filter behavior using public CAS algorithm name | No action needed |

## Rules

- Do not hide known failures.
- Do not mark a milestone complete by pretending a known limitation is solved.
- Move fixed items to a resolved section with commit/PR reference.

## Resolved limitations

| ID | Fixed in | Notes |
|---|---|---|
| | | |

## Resource-Bound Evidence Limits

The following evidence is not available in this handoff and is not claimed:

- multi-machine validation
- broad distro compatibility
- external tester results
- comprehensive visual-quality benchmark corpus
- production Temporal Forge validation
- full release artifact reproducibility from independent machines
- full website (specification pack exists but site is not deployed)

These are not hidden. They are accepted current limits.

Any future public claim about these areas requires new evidence.

## Archive Media Note

benchmarks/ave_benchmark_960x540_20s.mp4 may appear modified, missing, or altered in transfer archives because benchmark media was removed or changed to save space. This does not change source-code handoff status. Agents must not treat the transferred benchmark media state as proof of benchmark reproducibility. Benchmark reproducibility requires restoring or regenerating the expected fixture and recording its hash.

## Benchmark Media Policy

Benchmark claims require:
- fixture path,
- fixture source or generation method,
- fixture hash,
- command used,
- environment,
- output log,
- date,
- whether the result is maintainer-verified or community-submitted.

Without restored media and hashes, benchmark docs may describe protocol but must not claim reproducible benchmark results from the transfer archive alone.

A substitute clip may be used only for smoke testing or protocol demonstration unless its source or generation method, hash, command, environment, output log, date, and verification label (maintainer-verified or community-submitted) are recorded.
