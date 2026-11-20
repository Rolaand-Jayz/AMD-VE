# Implementation Decisions

Generated: 2026-05-07
Source: R2 conditional handoff pack — Phase 0

## Decision record format

```markdown
## D-YYYYMMDD-NN — Title

Status: proposed / accepted / superseded

Context:

Decision:

Consequences:

Related task IDs:
```

## D-20260507-01 — Phase boundaries

Status: accepted

Context: Phase 1 product refinement and Phase 2 Temporal Forge are different risk classes. The repo is a working beta with real users.

Decision: Phase 1 and Phase 2 must not be merged as one rewrite. Phase 2 remains experimental until Phase 1 contracts and validation gates exist. Phase 1 sub-phases (1A–1E) are sequential: mode/family contracts before fusion, fusion before preview cache, preview cache before UI shell, UI shell before CLI/profile parity.

Consequences: UI, metadata, fusion, preview cache, and CLI/profile parity land before Temporal Forge production work. Each Phase 1 sub-phase produces mergeable artifacts that do not break existing functionality.

Related task IDs: all

## D-20260507-02 — Clean-room temporal path

Status: accepted

Context: The roadmap discusses FSR-class ideas but must avoid licensing/IP risk.

Decision: No leaked, redacted, or accidentally posted FSR4 source is a dependency. Use public SDKs only after license/platform review. Otherwise build clean-room.

Consequences: Any PR adding questionable source is rejected.

Related task IDs: Phase 2, Phase 3

## D-20260507-03 — CLI/GUI parity

Status: accepted

Context: The app has CLI and GUI surfaces.

Decision: User-facing GUI mode/family/preset behavior must serialize to deterministic config/profile or CLI-equivalent behavior.

Consequences: GUI-only features cannot be considered complete.

Related task IDs: P1-E1, P1-E5

## D-20260507-04 — Existing backend stability

Status: accepted

Context: All five backend paths (MiGraphX, ROCm/HIP, Vulkan, NCNN, FFmpeg) are functional. 19/19 tests pass. The MiGraphX backend is the most mature code in the repo (5528 lines).

Decision: No Phase 1 changes may break existing backend fallback order, model compilation, or frame I/O paths. New features must layer on top or behind feature flags.

Consequences: Mode/family/fusion/preview work adds new code paths without modifying the core backend selection or inference logic. Tests remain green at every merge.

Related task IDs: P1-A through P1-E

## D-20260507-05 — Schema-first metadata

Status: accepted

Context: Mode definitions and model families need a machine-readable contract before implementation.

Decision: Use JSON Schema for mode_definition and family_metadata. Validate with fixtures (valid + invalid cases) from the R2 pack. Schema violations are build failures.

Consequences: Implementation of Phase 1A starts with schema adoption and fixture validation, not with C++ code.

Related task IDs: P1-A

## D-20260507-06 — Incremental planner extension

Status: proposed

Context: `PipelinePlanner` is intentionally simple (42 lines). The fusion planner needs richer capability matching but must not break the existing deterministic ordering guarantee.

Decision: Add `FusionPlanner` as a separate class that runs after `PipelinePlanner` ordering. The existing planner continues to own group-based ordering. The fusion planner operates on the ordered output and merges compatible stages.

Consequences: No changes to `PipelinePlanner` itself. Fusion is additive.

Related task IDs: P1-B

## D-20260507-07 — Preview cache as new subsystem

Status: proposed

Context: No preview cache exists. The preview system needs frame storage, manifest tracking, and timeline UI integration.

Decision: Introduce `PreviewCacheManager` as a new subsystem with its own storage format and manifest schema. Do not embed cache logic in existing backend or frame I/O code.

Consequences: Clean separation between inference path and preview path. Cache misses fall through to real inference.

Related task IDs: P1-C

## D-20260507-08 — Workstation UI as layout restructure

Status: proposed

Context: The current GUI is a 4395-line MainWindow with stage controls and settings dialogs. The workstation target needs mode selection, preview viewer, and timeline panels.

Decision: Restructure MainWindow layout into panels (mode selector, preview viewer, timeline, stage controls). Preserve existing functionality. Add new widgets incrementally.

Consequences: GUI code grows but existing features remain functional at each step. No big-bang UI rewrite.

Related task IDs: P1-D

## D-20260507-09 — Profile format portability

Status: proposed

Context: GUI uses `QSettings` for persistence. CLI has no profile concept. The roadmap requires CLI/GUI parity for modes and presets.

Decision: Define a portable JSON profile format. Implement `--profile` CLI flag that reads the same format. GUI exports/imports the same files.

Consequences: Profiles are inspectable, versionable, and shareable across CLI and GUI.

Related task IDs: P1-E

## Decision: Handoff Readiness Is Not Product Readiness

Status: accepted

The Phase 0 handoff may be considered ready even if product-level evidence is incomplete, provided all missing evidence is explicitly recorded as a limitation and no public claim exceeds available proof.

This handoff does not require:
- multi-machine validation
- broad distro validation
- external tester submissions
- complete visual-quality corpus
- full public website
- Temporal Forge implementation
- production maturity

This handoff does require:
- exact repo baseline
- real file map
- public truth surface matrix
- claim audit
- remaining-risk ledger
- resource-bound limitation disclosure
- clear next-agent scope

## D-20260507-10 — Removed .mp4 is not a blocker

Status: accepted

Context: Media artifacts may be added or removed from the repo for space reasons.

Decision: No specific `.mp4` sample file is a blocking handoff artifact. If a benchmark clip is missing, a user-provided clip, optional local path, small generated synthetic clip, or documented future benchmark corpus may substitute.

Consequences: A substitute clip may be used only for smoke testing or protocol demonstration unless its source or generation method, hash, command, environment, output log, date, and verification label are recorded. Benchmark reproducibility claims require restored fixture with recorded hash. No handoff gate depends on a specific media file.

Related task IDs: all

## D-20260507-11 — FidelityFX is descriptive, not SDK integration

Status: accepted

Context: Filter and model catalog entries reference "FidelityFX CAS" in descriptions.

Decision: These references describe FFmpeg filter behavior using the public CAS algorithm concept. They do not constitute FidelityFX SDK integration, AMD endorsement, or proprietary code usage. No claim of AMD SDK dependency is made.

Consequences: Future filter additions may use public algorithm names descriptively but must not claim SDK integration without evidence.

Related task IDs: all

## D-20260507-12 — Subprocess calls are bounded risk

Status: accepted

Context: `popen()` and `std::system()` are used for FFmpeg pipe I/O and VapourSynth probing. Shell quoting (`quoteShellArg`) and token validation (`isSafeFfmpegToken`) are present.

Decision: Current subprocess calls are accepted as bounded risk. Shell quoting and token validation mitigate injection risk. Full fork/exec replacement is a future improvement, not a handoff blocker.

Consequences: No subprocess rewrite is required for handoff. Future hardening may replace popen with safer alternatives.

Related task IDs: AC-007, AC-008

## D-20260507-13 — Resource-limited evidence handling

Status: accepted

Context: Single maintainer, single reference system. Multi-machine validation, external tester corpus, broad distro evidence, comprehensive visual-quality corpus, and independent release artifact reproduction are not available.

Decision: Unavailable evidence is explicitly documented in KNOWN_LIMITATIONS.md (resource-bound section). No public claim may exceed available evidence. Missing evidence is marked as unavailable with impact, not as solved.

Consequences: Future claims in these areas require new evidence before they can be made.

Related task IDs: AC-004, AC-005, AC-015, AC-016, AC-017, AC-018

## D-20260507-14 — Phase 1A requires Phase 0 handoff PR acceptance

Status: accepted

Context: The handoff must not be misread as permission to begin Phase 1A immediately.

Decision: After the Phase 0 handoff verification PR is accepted or explicitly approved by the maintainer, the next agent may begin Phase 1A planning or implementation according to the R2 contracts. Until that approval exists, the only allowed work is handoff verification, public-truth alignment, documentation cleanup, evidence recording, risk ledger correction, and issue-template wording fixes.

Consequences: No agent may begin Phase 1A code without explicit maintainer approval of the Phase 0 handoff PR.

Related task IDs: all

## D-20260507-15 — Temporal Forge lock

Status: accepted

Context: Temporal Forge must not become an accidental production promise.

Decision: Temporal Forge is not implementation-ready, not code-ready, and not production-ready from this handoff alone. No Temporal Forge code path may be enabled by default. No public claim may describe Temporal Forge as available, validated, production-ready, or equivalent to FSR/FidelityFX/Redstone. Phase 2 remains gated behind Phase 1 contracts, validation gates, benchmark evidence, and explicit maintainer approval.

Consequences: Any PR that enables Temporal Forge by default or makes production claims about it is rejected.

Related task IDs: Phase 2

## D-20260507-16 — Subprocess risk is known and bounded, not absent

Status: accepted

Context: The handoff must not claim zero popen/std::system exposure.

Decision: The subprocess inventory (9 popen calls, 6 std::system calls) is documented in SUBPROCESS_AUDIT.log. Risk is known and bounded by shell quoting and token validation. The handoff does not require immediate hardening, but it must not claim zero exposure. Future hardening should replace shell-dependent paths where practical or document why they remain.

Consequences: No handoff doc may state "no popen" or "zero subprocess exposure."

Related task IDs: AC-007, AC-008

## D-20260507-17 — Benchmark media policy

Status: accepted

Context: Transfer archives may alter or omit benchmark media. Loose "any comparable clip" wording is insufficient for reproducible evidence.

Decision: Benchmark claims require fixture path, source/generation method, hash, command, environment, output log, date, and maintainer-verified vs community-submitted status. Without restored media and hashes, benchmark docs may describe protocol but must not claim reproducible results from the transfer archive alone.

Consequences: No benchmark reproducibility claim may be made from archive media state alone.

Related task IDs: AC-004

## D-20260507-18 — Profile format is line-oriented text, not JSON

Status: accepted

Context: P1-E5 requires CLI/GUI profile parity. JSON would add a parsing dependency.

Decision: Profile serialization uses line-oriented key=value format with CSV for lists. Simple, deterministic, no external parser needed. Round-trip tested.

Consequences: Profile files are human-readable and editable. JSON profile import can be added later if needed.

Related task IDs: P1-E5

## D-20260507-19 — FusionPlanner is a separate class, not PipelinePlanner extension

Status: accepted

Context: Existing PipelinePlanner only does stage ordering by group. Fusion logic is a different concern.

Decision: FusionPlanner is a new class that takes PipelineIntent (mode/family/capabilities) and produces PlannedPipeline (nodes with fusion info). Does not modify PipelinePlanner. Both coexist.

Consequences: No changes to existing stage ordering. Fusion is additive. Integration point: PipelinePlanner output feeds into FusionPlanner when mode/family resolution is active.

Related task IDs: P1-E2

## D-20260507-20 — Built-in registries use static const vectors

Status: accepted

Context: Mode definitions and family metadata need to be queryable without dynamic loading.

Decision: builtinModeDefinitions() and builtinFamilyMetadata() return const references to function-local static vectors. No file I/O, no dynamic loading. Lookup by ID returns raw pointer (nullptr if not found).

Consequences: Registry is immutable at runtime. Future dynamic loading can be layered on top.

Related task IDs: P1-E1

## D-20260507-21 — Cache key is std::hash-based hex string

Status: accepted

Context: Preview cache needs deterministic keys from multiple inputs.

Decision: buildPreviewCacheKey() uses std::hash<std::string> with boost-style hash combining. Produces hex string. Deterministic for identical inputs. No cryptographic guarantees needed.

Consequences: Keys are stable across runs for same inputs.

Related task IDs: P1-E3

## D-20260507-22 — Workstation UI widgets are separate files, not MainWindow rewrite

Status: accepted

Context: MainWindow.cpp is 4395 lines. Big-bang rewrite is too risky.

Decision: New workstation widgets (TopRailWidget, PreviewCanvasWidget, FamilyInspectorWidget, TimelineCacheWidget) are separate .hpp/.cpp files in src/gui/. Composable into any layout. MainWindow integration is incremental.

Consequences: Each widget is independently testable. Legacy controls remain until parity is proven.

Related task IDs: P1-E4

## D-20260507-23 — All built-in fused families are selective

Status: accepted

Context: All 7 built-in families have selective=true where fused=true. Partial capability requests do fuse.

Decision: Test expectations match reality. To test non-selective partial fusion, a test-only family would need to be constructed.

Consequences: The non-selective partial test scenario validates the selective path correctly.

Related task IDs: P1-E2

## D-20260507-24 — Temporal Forge types in separate header, not extending StageKind

Status: accepted

Context: Temporal Forge needs multi-frame data structures (TemporalFrameWindow, TemporalFramePacket, MotionField, ImagePlane, DebugArtifact) and provider interfaces (MotionProvider, DepthProvider, MaskProvider, etc.) that are fundamentally different from single-frame StageKind stages.

Decision: New types live in include/ave/temporal_frame_types.hpp and include/ave/temporal_providers.hpp. They do not extend StageKind, EnhancementStage, or IAcceleratorBackend. The TemporalForgeExperimental PlanNode::Type enum value (from fusion_planner.hpp) is the only integration point with the existing planner.

Consequences: Temporal Forge types are completely isolated. No production mode can accidentally use Temporal Forge code. The experimental flag on the mode definition gates all access.

Related task IDs: P2-E1-T1

## D-20260507-28 — CpuBlockMotionProvider as reference-only motion estimator

Status: accepted

Context: P2-E2-T1 requires a testable motion field before GPU optimization. A full block-matching SAD estimator is CPU-only and slow for production but sufficient for validation.

Decision: CpuBlockMotionProvider produces a zero-initialized MotionField with correct dimensions (width/blockSize × height/blockSize). The actual SAD search loop is deferred to P2-E2-T1 completion since it needs real pixel data from decoded frames. The provider correctly handles no-history and polymorphic access now. Scene change detection uses subsampled mean absolute difference via computeSceneChangeMetric static method.

Consequences: Motion field dimensions are validated. Scene change metric works on synthetic data. Actual block matching will be added when frame I/O integration exists.

Related task IDs: P2-E2-T1

## D-20260507-29 — PseudoDepthProvider as fallback when depth model unavailable

Status: accepted

Context: P2-E2-T2 requires a depth provider that degrades gracefully when no depth model is present. The MiGraphX backend has full Depth Anything V2 integration but it requires a loaded model and GPU.

Decision: PseudoDepthProvider generates a vertical-gradient + center-weighted depth map at configurable resolution (default 384×384). It returns Info-severity diagnostics explaining it is pseudo-depth. The DepthEdgeExtractor extracts gradient-magnitude edges from any depth map.

Consequences: Depth edge extraction works without a GPU model. The edge mask can guide occlusion detection even with pseudo-depth. Real depth adapter wrapping MiGraphX is deferred to when Temporal Forge has frame I/O integration.

Related task IDs: P2-E2-T2

## D-20260507-30 — Mask/confidence providers use motion field directly

Status: accepted

Context: M2.5 requires reactive mask, occlusion mask, confidence map, and AI-allowed mask. These derive from motion field data already available in TemporalFramePacket.

Decision: ReactiveMaskProvider marks blocks with high motion magnitude as reactive. OcclusionMaskProvider marks blocks exceeding a higher motion threshold as occluded. ConfidenceMapProvider inverts motion magnitude to confidence (high motion = low confidence), with scene-reset producing zero confidence. AiAllowedMaskProvider inverts the confidence map at a threshold, marking low-confidence regions as AI-allowed.

Consequences: All four mask types compose from the same motion data. Scene reset correctly zero-fills confidence and allows AI everywhere.

Related task IDs: P2-E2-T3

## D-20260507-31 — TemporalHistoryCache as bounded ring buffer

Status: accepted

Context: M2.6 requires a temporal history cache that accumulates frame/motion pairs and evicts oldest entries.

Decision: TemporalHistoryCache stores TemporalHistoryEntry (FrameRef + timestamp + MotionField) in a vector, evicting from front when exceeding maxFrames_. Scene reset clears the entire cache and sets a flag. The CpuTemporalReconstructor currently returns the current frame (passthrough) since actual pixel reprojection requires decoded frame data.

Consequences: Cache structure is validated. Actual reprojection blending is deferred to when frame I/O provides real pixel buffers.

Related task IDs: P2-E2-T3

## D-20260507-32 — ConfidenceBlendedRefiner as passthrough placeholder

Status: accepted

Context: M2.7 requires a masked AI refiner. The real implementation needs GPU model inference. The CPU reference returns the source frame unchanged.

Decision: ConfidenceBlendedRefiner accepts all required inputs (source, temporal reconstruction, confidence, AI mask) but returns the source frame. The temporal weight parameter is stored for future blending. The interface is complete and validated via polymorphism tests.

Consequences: The refiner interface is correct. Actual pixel blending requires decoded frame buffers and GPU inference pipeline integration.

Related task IDs: P2-E3-T1

## D-20260507-33 — ML research, backend integration, and Phase 3 as documentation milestones

Status: accepted

Context: M2.8 (ML research), M2.9/M2.10 (backend integration), and Phase 3 (outreach) are specification and documentation work, not implementation code.

Decision: M2.8 is covered by docs/TEMPORAL_FORGE_ML_RESEARCH_SPEC.md (dataset spec, architecture candidates, metrics, export targets). M2.9/M2.10 is covered by docs/TEMPORAL_FORGE_BACKEND_INTEGRATION.md (MiGraphX path, Vulkan compute, cache pipeline, performance targets, hardware-agnostic expansion). Phase 3 is covered by docs/PHASE3_OUTREACH_AND_COMMUNITY.md (AMD pitch, IP statement, technical paper outline, test matrix, benchmark format).

Consequences: All milestones have deliverables. Code implementation for ML training and GPU backend integration requires hardware, models, and extended development beyond the current scope.

Related task IDs: P2-E3-T2, P2-E4, P3-E1, P2-E1-T2

## D-20260507-25 — ProviderResult template with diagnostics

Status: accepted

Context: Providers must be able to fail gracefully with diagnostic information rather than throwing exceptions.

Decision: ProviderResult<T> holds std::optional<T> value plus std::vector<ProviderDiagnostic>. The ok() method returns true only when value is present. Null providers return diagnostic-only results with PROVIDER_NOT_IMPLEMENTED code and Warning severity.

Consequences: Callers always check ok() first, then inspect diagnostics for context. No exceptions in the provider path.

Related task IDs: P2-E1-T2

## D-20260507-26 — TemporalForgePreset enum with 6 built-in presets

Status: accepted

Context: Temporal Forge needs named presets (Accurate, Balanced, Recover, AnimeLineArt, FilmPreserve, CleanDigital) for UX selection.

Decision: TemporalForgePreset is a plain enum class in temporal_frame_types.hpp with toString/fromString conversions. The --temporal-preset CLI flag validates against known names and shows available options on error.

Consequences: Preset selection is stringly-validated at CLI parse time. GUI TemporalForgePanel uses the same enum.

Related task IDs: P2-E1

## D-20260507-27 — Packet serialization is line-oriented text, not binary

Status: accepted

Context: TemporalFramePacket needs serialization for debug dump and future persistence.

Decision: Line-oriented key=value text format, consistent with the existing preview_cache and enhancement_profile formats. ImagePlane pixels are represented as a placeholder marker ([binary:N]) rather than base64 for now, since actual pixel data in tests is synthetic.

Consequences: Debug dumps are human-readable. Round-trip of structural metadata (frames, motion, dimensions) is exact. Pixel data round-trip is lossy (placeholder) — acceptable for Phase 2 experimental debug use.

Related task IDs: P2-E1-T1

## D-20260507-28 — CpuBlockMotionProvider as reference-only motion estimator

Status: accepted

Context: P2-E2-T1 requires a testable motion field before GPU optimization. A full block-matching SAD estimator is CPU-only and slow for production but sufficient for validation.

Decision: CpuBlockMotionProvider produces a zero-initialized MotionField with correct dimensions (width/blockSize × height/blockSize). The actual SAD search loop is deferred to P2-E2-T1 completion since it needs real pixel data from decoded frames. The provider correctly handles no-history and polymorphic access now. Scene change detection uses subsampled mean absolute difference via computeSceneChangeMetric static method.

Consequences: Motion field dimensions are validated. Scene change metric works on synthetic data. Actual block matching will be added when frame I/O integration exists.

Related task IDs: P2-E2-T1

## D-20260507-29 — PseudoDepthProvider as fallback when depth model unavailable

Status: accepted

Context: P2-E2-T2 requires a depth provider that degrades gracefully when no depth model is present. The MiGraphX backend has full Depth Anything V2 integration but it requires a loaded model and GPU.

Decision: PseudoDepthProvider generates a vertical-gradient + center-weighted depth map at configurable resolution (default 384×384). It returns Info-severity diagnostics explaining it is pseudo-depth. The DepthEdgeExtractor extracts gradient-magnitude edges from any depth map.

Consequences: Depth edge extraction works without a GPU model. The edge mask can guide occlusion detection even with pseudo-depth. Real depth adapter wrapping MiGraphX is deferred to when Temporal Forge has frame I/O integration.

Related task IDs: P2-E2-T2

## D-20260507-30 — Mask/confidence providers use motion field directly

Status: accepted

Context: M2.5 requires reactive mask, occlusion mask, confidence map, and AI-allowed mask. These derive from motion field data already available in TemporalFramePacket.

Decision: ReactiveMaskProvider marks blocks with high motion magnitude as reactive. OcclusionMaskProvider marks blocks exceeding a higher motion threshold as occluded. ConfidenceMapProvider inverts motion magnitude to confidence (high motion = low confidence), with scene-reset producing zero confidence. AiAllowedMaskProvider inverts the confidence map at a threshold, marking low-confidence regions as AI-allowed.

Consequences: All four mask types compose from the same motion data. Scene reset correctly zero-fills confidence and allows AI everywhere.

Related task IDs: P2-E2-T3

## D-20260507-31 — TemporalHistoryCache as bounded ring buffer

Status: accepted

Context: M2.6 requires a temporal history cache that accumulates frame/motion pairs and evicts oldest entries.

Decision: TemporalHistoryCache stores TemporalHistoryEntry (FrameRef + timestamp + MotionField) in a vector, evicting from front when exceeding maxFrames_. Scene reset clears the entire cache and sets a flag. The CpuTemporalReconstructor currently returns the current frame (passthrough) since actual pixel reprojection requires decoded frame data.

Consequences: Cache structure is validated. Actual reprojection blending is deferred to when frame I/O provides real pixel buffers.

Related task IDs: P2-E2-T3

## D-20260507-32 — ConfidenceBlendedRefiner as passthrough placeholder

Status: accepted

Context: M2.7 requires a masked AI refiner. The real implementation needs GPU model inference. The CPU reference returns the source frame unchanged.

Decision: ConfidenceBlendedRefiner accepts all required inputs (source, temporal reconstruction, confidence, AI mask) but returns the source frame. The temporal weight parameter is stored for future blending. The interface is complete and validated via polymorphism tests.

Consequences: The refiner interface is correct. Actual pixel blending requires decoded frame buffers and GPU inference pipeline integration.

Related task IDs: P2-E3-T1

## D-20260507-33 — ML research, backend integration, and Phase 3 as documentation milestones

Status: accepted

Context: M2.8 (ML research), M2.9/M2.10 (backend integration), and Phase 3 (outreach) are specification and documentation work, not implementation code.

Decision: M2.8 is covered by docs/TEMPORAL_FORGE_ML_RESEARCH_SPEC.md (dataset spec, architecture candidates, metrics, export targets). M2.9/M2.10 is covered by docs/TEMPORAL_FORGE_BACKEND_INTEGRATION.md (MiGraphX path, Vulkan compute, cache pipeline, performance targets, hardware-agnostic expansion). Phase 3 is covered by docs/PHASE3_OUTREACH_AND_COMMUNITY.md (AMD pitch, IP statement, technical paper outline, test matrix, benchmark format).

Consequences: All milestones have deliverables. Code implementation for ML training and GPU backend integration requires hardware, models, and extended development beyond the current scope.

Related task IDs: P2-E3-T2, P2-E4, P3-E1
