# License, IP, and AMD Outreach

## Intent

AMD-VE should remain open source while avoiding unnecessary IP risk.

## Mainline rule

The public mainline must use:

- original code
- official public SDKs
- public documentation
- permissively licensed/open-source dependencies
- clean-room implementation choices

Do not make redacted or accidentally published FSR4 internals a dependency without AMD permission or legal review.

## Recommended posture

AMD-VE should approach AMD as a collaborator:

> This project is an open-source, AMD-first video enhancer that showcases AMD hardware for Linux creators and explores a clean-room video-native temporal reconstruction path inspired by public FidelityFX concepts.

## What to ask AMD

- Which FSR/FSR SDK components are safe to use in open-source Linux-native applications?
- Is AMD interested in a video-native temporal reconstruction use case?
- Can AMD provide technical guidance on FidelityFX Optical Flow, FSR SDK, or future Redstone paths?
- Is there a recommended way to integrate with GPUOpen/ROCm/MiGraphX efforts?
- Can AMD clarify branding boundaries and allowed terminology?

## What not to lead with

Do not lead with:

- “FSR4 was leaked, therefore...”
- “We can use your code whether you like it or not.”
- “This is FSR4 for video.”

Lead with:

- clean-room intent
- AMD user benefit
- open-source value
- Linux creator ecosystem
- demo results

## Suggested outreach package

1. One-page pitch
2. 2-minute demo video
3. Technical architecture summary
4. IP-cleanliness statement
5. Benchmark clips
6. Specific ask list
7. Repo link
8. Contact info

## License strategy options

### Option A: GPLv3 main app

Best for freedom-protecting open source.

Pros:

- keeps derivatives open when distributed
- aligns with anti-lockdown values
- still allows separate commercial licensing if copyright ownership is clean

Cons:

- less attractive to some corporate adopters

### Option B: Apache-2.0

Best for broad adoption and patent language.

Pros:

- permissive and business-friendly
- explicit patent grant language

Cons:

- corporations can close derivatives

### Option C: MPL-2.0

Middle path.

Pros:

- file-level copyleft
- easier corporate adoption than GPL

Cons:

- weaker freedom protection than GPL

## Recommended structure

If the project wants all code open while protecting freedom:

```text
AMD-VE core: GPLv3 or MPL-2.0
Plugin/API headers: Apache-2.0 if adoption matters
Docs/specs: CC BY-SA 4.0 or Apache-2.0
Benchmark data: separate dataset license
```

## If AMD wants to use it

If AMD wants proprietary integration, separate licensing can be discussed. Public open source and commercial licensing are not mutually exclusive if copyright ownership is managed.

## IP cleanliness checklist

- No redacted source copied.
- Dependency licenses recorded.
- Model licenses recorded.
- Dataset rights recorded.
- Contributor license expectations documented.
- Architecture notes distinguish public inspiration from copied implementation.
- AMD branding used only where allowed.

## Disclaimer

This is strategy guidance, not legal advice. For public use of disputed FSR4 materials, consult an open-source/IP attorney.

## Operational license controls

The IP checklist is now a merge gate, not a courtesy note.

Required registries:

- `templates/DEPENDENCY_LICENSE_REGISTRY_TEMPLATE.md`
- `templates/MODEL_LICENSE_REGISTRY_TEMPLATE.md`
- `templates/DATASET_PROVENANCE_TEMPLATE.md`
- `templates/SDK_USAGE_REVIEW_TEMPLATE.md`

Before release, copy these templates into the target repository under `docs/implementation/legal/` or the project’s equivalent release-compliance folder.

## Merge-blocking license classes

Block merge or require explicit maintainer/legal waiver for:

- unknown dependency license,
- model license missing redistribution/use rights,
- benchmark clip without provenance,
- SDK import without license and platform review,
- any leaked/redacted/rights-uncertain implementation material,
- copied code without attribution/license compatibility review.

## SPDX/SBOM expectation

If the target repo already has an SBOM workflow, use it. If not, Phase 1 release hardening must at minimum produce a dependency/license table. Phase 2 model/dataset work must not ship without model and media provenance tables.

## Review ownership

Each release checklist must name:

- reviewer,
- review date,
- registries reviewed,
- unresolved risks,
- waiver IDs if any.

Professional legal review is required for disputed, high-risk, proprietary, or SDK-redistribution questions.
