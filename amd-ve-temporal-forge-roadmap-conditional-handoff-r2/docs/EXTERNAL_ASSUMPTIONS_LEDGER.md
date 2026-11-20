# External Assumptions Ledger

## Purpose

This ledger records version-sensitive external facts used by the roadmap. Update this file before changing AMD/FSR/FidelityFX implementation strategy, public claims, outreach language, or legal/IP posture.

- **Access date:** 2026-05-05
- **Source preference:** official AMD, GPUOpen, AMD GitHub, official SDK docs
- **Forbidden source class:** leaked, redacted, accidentally posted, private, or rights-uncertain materials

## Ledger entries

| ID | Claim | Source title | Official URL | Access date | Claim class | Roadmap dependency | Confidence | Required action before implementation |
|---|---|---|---|---|---|---|---:|---|
| EXT-AMD-001 | AMD FidelityFX SDK v1.1.4 is described by GPUOpen as a collection of optimized, open-source shader/runtime technologies for DirectX 12 or Vulkan applications. | Welcome to the AMD FidelityFX SDK 1.1.4 | https://gpuopen.com/manuals/fidelityfx_sdk/ | 2026-05-05 | verified official fact | Supports public-FidelityFX research and clean-room comparison, not automatic integration. | high | Verify exact license and platform fit before importing any SDK code. |
| EXT-AMD-002 | AMD FSR SDK 2.2 / Redstone includes ML-powered FSR Upscaling 4.1 and Ray Regeneration 1.1, with ML-powered features optimized for RDNA 4 / Radeon RX 9000-series class hardware; analytical fallback modes exist for some features. | Welcome to the AMD FSR SDK 2.2, now available on GPUOpen | https://gpuopen.com/learn/amd-fsr-sdk-2-2-now-available/ | 2026-05-05 | verified official fact | Frames Phase 2 as research/clean-room inspiration and prevents false Linux/RDNA3 production claims. | high | Do not depend on Redstone as AMD-VE Linux production path unless AMD/license/platform review clears it. |
| EXT-AMD-003 | AMD FSR SDK 2.2 requirements list DirectX 12 and Windows 10/11 OS requirements; some features require Windows 11 and DirectX 12 Agility SDK support. | Welcome to the AMD FSR SDK 2.2, now available on GPUOpen | https://gpuopen.com/learn/amd-fsr-sdk-2-2-now-available/ | 2026-05-05 | verified official fact | Blocks claims that Redstone is currently a drop-in Linux/Vulkan solution for AMD-VE. | high | Keep Temporal Forge AMD-first but SDK-optional until Linux/API feasibility is verified. |
| EXT-AMD-004 | AMD FidelityFX Optical Flow estimates motion between current and previous scene inputs, works on 8x8 blocks, outputs estimated motion vectors and a scene-change detection flag, and is used by FSR3 with upscaled game motion vectors. | FidelityFX Optical Flow 1.1.2 | https://gpuopen.com/manuals/fidelityfx_sdk/techniques/optical-flow/ | 2026-05-05 | verified official fact | Supports the conceptual choice to include motion fields and scene-reset handling in a video-native temporal cache. | high | Treat as inspiration or optional public SDK integration after license/platform review; do not copy unsupported internals. |
| EXT-AMD-005 | AMD FSR 3 supports DirectX 12, Vulkan, and Unreal Engine paths, and FSR 3.1.x source/documentation is part of the public FidelityFX/FSR documentation line. | AMD FidelityFX Super Resolution 3 | https://gpuopen.com/fidelityfx-super-resolution-3/ | 2026-05-05 | verified official fact | Supports comparison against public temporal-upscaling concepts and public SDK behavior. | high | If SDK code is used, preserve license notices and isolate adapters from AMD-VE-original temporal code. |
| EXT-PROJECT-001 | Temporal Forge is not FSR4, not Redstone, and not an AMD SDK redistribution unless an explicit integration decision is made. | Project inference from clean-room requirement | local project policy | 2026-05-05 | project policy / inference | Protects naming, outreach, and release claims. | high | Use “FSR-style,” “temporal reconstruction,” or “clean-room temporal path,” not “FSR4 for video.” |

## Update policy

Update this ledger when:

1. AMD/GPUOpen releases a new SDK version.
2. A task proposes importing SDK source, headers, binaries, or models.
3. Outreach language mentions AMD, FSR, FidelityFX, Redstone, RDNA, ROCm, MiGraphX, or GPUOpen.
4. Benchmarks or product claims rely on external SDK behavior.
5. Legal/IP review changes what is allowed.

## Source classification rules

| Classification | Allowed for implementation? | Notes |
|---|---:|---|
| Official AMD/GPUOpen public docs | yes, after license/platform review | Preferred source class. |
| Official SDK release source/binaries | maybe | Requires license, platform, redistribution, and packaging review. |
| Public academic/paper concepts | maybe | Must implement original code and cite concepts. |
| Community reverse engineering | no by default | Requires explicit legal review before use. |
| Leaked/redacted/accidentally posted internals | no | Blocked unless AMD gives explicit written permission and legal review approves. |

## Claim hygiene

Any public README, release note, benchmark, or demo must distinguish:

- **verified fact** — backed by this ledger
- **project inference** — derived from public facts but not guaranteed by AMD
- **prototype result** — measured in AMD-VE with recorded commit, hardware, media, command, and output
- **aspirational goal** — not yet proven
