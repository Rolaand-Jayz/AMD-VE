# AMD-VE Temporal Forge Roadmap + PRD Pack

This archive is a **conditional implementation handoff bundle** for the next major evolution of AMD-VE.

It is implementation-ready for **agent execution after Phase 0 repository discovery**, not ready for blind production-code edits by itself. The target source repository is intentionally not bundled here, so exact file paths, build commands, and current APIs must be verified before any feature work.

It covers two major product tracks:

1. **Phase 1 — Refined AMD-VE redesign**
   - full UI facelift
   - larger preview and split-wipe comparison
   - seekable render-cache timeline
   - model catalog simplification
   - fused family enhancements
   - streamlined default workflow
   - hardening of existing MiGraphX/backend/model infrastructure

2. **Phase 2 — Temporal Forge ML**
   - AMD-first, clean-room, video-native temporal reconstruction
   - FSR-style buffer synthesis from ordinary video
   - motion/depth/reactive/confidence masks
   - temporal-first reconstruction
   - confidence-guided AI refinement
   - hardware-agnostic architecture after the AMD golden path

## Intended reader

This bundle is written for coding agents and human maintainers. It is deliberately explicit so an agent can execute the work without inventing missing requirements.

## Canonical product thesis

> AMD-VE should stop being a model picker and become a preview-first restoration workstation.
>
> Phase 1 makes the app refined, simpler, and credible.
>
> Phase 2 makes it genuinely new: temporal video reconstruction through time, with AI acting only where evidence runs out.

## Grounding assumptions from the current repo

AMD-VE currently presents itself as a Linux-first C++20 video enhancer with CLI and Qt GUI frontends, deterministic planning, model management, MiGraphX-first inference, ROCm/HIP fallback, Vulkan Compute fallback, NCNN Vulkan fallback, FFmpeg media spine, and distro packaging. The current stage taxonomy includes restoration, artifact removal, denoise, deblur, dehalo, color fix, upscale, sharpen, stereo 3D, and interpolation. The model catalog/manifest system already has fields for model families, capabilities, fused execution, selective capabilities, control bindings, precision, scale, and MiGraphX compile properties.

This plan assumes those foundations are preserved and hardened, not thrown away.

## Current external technology facts used by this pack

External AMD/FSR/FidelityFX facts are now maintained in `docs/EXTERNAL_ASSUMPTIONS_LEDGER.md` with official source URLs, access dates, confidence, and roadmap dependency notes.

Do not treat README-level summaries as the source of truth. Before implementation or public claims, re-check the ledger and update it if AMD/GPUOpen documentation has changed.

The clean-room rule is unchanged: accidentally posted, redacted, leaked, or otherwise uncertain FSR internals are **not** a required dependency for this roadmap. The mainline path must remain original code, public documentation, official SDKs with verified license/platform fit, or explicit AMD permission.

## File map

| File | Purpose |
|---|---|
| `docs/00_EXECUTIVE_SUMMARY.md` | Board-level / maintainer-level summary |
| `docs/01_MASTER_ROADMAP.md` | Full phased roadmap with milestones |
| `docs/02_PHASE_1_REDESIGN_PRD.md` | PRD for the refined app redesign |
| `docs/03_UI_UX_WORKSTATION_SPEC.md` | Detailed UI/UX layout and flows |
| `docs/04_MODEL_CONSOLIDATION_AND_FUSION_SPEC.md` | Model reduction, families, fused enhancements |
| `docs/05_PREVIEW_TIMELINE_CACHE_SPEC.md` | Large preview, split wipe, render cache, timeline |
| `docs/06_PHASE_2_TEMPORAL_FORGE_ML_PRD.md` | PRD for the hero temporal reconstruction path |
| `docs/07_TEMPORAL_ENGINE_ARCHITECTURE.md` | Technical architecture of Temporal Forge ML |
| `docs/08_BUFFER_PROVIDERS_AND_MASKS.md` | Motion, depth, reactive, confidence, exposure, reset |
| `docs/09_BACKEND_DATAFLOW_AND_STAGE_SYSTEM.md` | Backend/stage/planner changes |
| `docs/10_ML_REFINER_TRAINING_AND_EVAL.md` | ML refiner, dataset, metrics, validation |
| `docs/11_KEEP_REMOVE_HARDEN.md` | What to keep, remove, refactor, harden |
| `docs/12_AGENT_IMPLEMENTATION_PROTOCOL.md` | How agents should execute without hallucinating |
| `docs/13_ACCEPTANCE_TESTS_AND_VALIDATION.md` | Test plans and definition of done |
| `docs/14_LICENSE_IP_AND_AMD_OUTREACH.md` | Licensing/IP posture and AMD collaboration plan |
| `docs/15_MILESTONE_BACKLOG.md` | Implementation backlog grouped by milestone |
| `docs/16_GLOSSARY.md` | Required terminology |
| `docs/17_TRACEABILITY_MATRIX.md` | Requirement-to-doc traceability |
| `docs/18_AGENT_PROMPTS.md` | Copy/paste prompts for Crush/Copilot/Codex/ChatGPT |

## Non-negotiables

- Do not build the public mainline on redacted/leaked FSR4 internals without explicit legal clearance or AMD permission.
- Do not claim “FSR4 for video” unless AMD grants the right to use that framing.
- Do not remove CLI parity. GUI improvements must drive the same core pipeline.
- Do not let the UI expose every model as an equal first-class choice.
- Do not let AI enhancement dominate the new temporal path by default.
- Do not make Phase 2 block Phase 1. The app must become refined before the hero research path becomes the main product.

---

# Implementation-ready revision

This revised archive adds an execution layer on top of the original roadmap/PRD pack.

Start with:

- `START_HERE_IMPLEMENTATION_READY.md`
- `docs/19_IMPLEMENTATION_TASKS.md`
- `docs/20_TECHNICAL_CONTRACTS.md`
- `docs/22_VALIDATION_MATRIX.md`
- `agent_tasks/00_baseline_and_file_map_prompt.md`

New implementation assets:

| File/Folder | Purpose |
|---|---|
| `START_HERE_IMPLEMENTATION_READY.md` | Required first read and execution order |
| `docs/19_IMPLEMENTATION_TASKS.md` | Task tickets with dependencies, acceptance criteria, tests, rollback, and scope guards |
| `docs/20_TECHNICAL_CONTRACTS.md` | Mode, family, fusion, preview cache, viewer, and Temporal Forge contracts |
| `docs/21_IMPLEMENTATION_READINESS_REVIEW.md` | What was missing and what this revision closes |
| `docs/22_VALIDATION_MATRIX.md` | Phase-by-phase validation requirements |
| `docs/23_PR_AND_RELEASE_GATES.md` | PR template expectations and release gates |
| `docs/24_RISK_REGISTER.md` | Risk register and stop conditions |
| `docs/25_REPO_PATCH_PLAN.md` | Practical patch order for the target repository |
| `docs/26_DEFINITION_OF_READY_AND_DONE.md` | Ready/done rules for tasks, UI, Temporal Forge, and releases |
| `schemas/` | JSON schemas for mode, family, preview cache, and temporal debug manifests |
| `templates/` | Baseline audit, file map, decision log, PR, known limitations, release gate templates |
| `agent_tasks/` | Copy-paste task prompts for coding agents |

Implementation rule:

> Do not let an agent touch production code until it has created a real baseline audit and file map from the target repository.

That is the armor plating. Without it, the agent may hallucinate file paths, invent APIs, and turn a strong plan into smoke.

