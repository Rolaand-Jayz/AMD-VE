# Model Consolidation and Fused Enhancement Spec

## Goal

Reduce model clutter while preserving expert power.

Users should choose **intent and family**, not raw model chaos.

## Current foundation

The repo already supports:

- model ids and display names
- family ids/names
- stage kinds
- capability lists
- fused execution flags
- selective capability flags
- precision and format metadata
- control input bindings for custom fused models

This should be strengthened and made central to the UX.

## Main principle

A model family is the user-facing unit.

A model file is an implementation detail.

## Visible default families

Initial suggested visible family slate:

| Family | Purpose | Default status |
|---|---|---|
| ClearReality Fast | Fast preview / quick general cleanup | visible |
| OpenProteus Quality | Live-action quality restoration/upscale | visible |
| Anime / Line Art | Animation, anime, line preservation | visible |
| Motion / RIFE | Interpolation and future motion support | visible |
| Depth / SBS 3D | 2D→3D depth/stereo output | visible |
| Temporal Forge | future hero path placeholder | visible as experimental only |
| Advanced / Legacy | all extra models | collapsed |

## What moves to Advanced / Legacy

Move these classes out of the primary UI:

- duplicate Real-ESRGAN variants
- old NCNN-only variants when an ONNX/MiGraphX-friendly family exists
- near-duplicate anime upscalers
- refiner-only variants
- model exports that exist only for compatibility testing
- PyTorch raw checkpoints unless explicitly needed
- models with unstable MiGraphX compile behavior
- experimental models not proven on reference hardware

This is not deletion. It is demotion.

## Family card requirements

Each family must declare:

```ini
family_id=
family_name=
display_purpose=
content_type=general|live_action|anime|archival|stereo|motion|experimental
speed_tier=S|A|B|C|D
quality_tier=S|A|B|C|D
migraphx_fit=S|A|B|C|D
visible_default=true|false
advanced_only=true|false
```

## Capability behavior

Capabilities should map to `StageKind` values:

- restore_compression
- remove_artifacts
- denoise
- deblur
- dehalo
- color_fix
- upscale
- sharpen
- stereo_3d
- interpolate

## Fused default behavior

When a family supports multiple capabilities:

1. Enable all safe default capabilities.
2. Show chips for each capability.
3. Expose strength controls.
4. Collapse matching stages into one AI execution group if metadata permits.
5. Run separate passes only when the family cannot validly fuse the requested capabilities.

## Selective capability rule

If `selective=false`:

- The family can fuse only when the requested capabilities match its full declared capability set.
- UI should avoid offering arbitrary per-capability disable switches unless it will force fallback behavior.

If `selective=true`:

- User can disable individual capabilities.
- Planner can fuse any valid subset.

## Planner requirements

Add a planning pass after deterministic stage order:

```text
requested stages
  → deterministic order
  → group consecutive stages by family
  → validate capability/fusion metadata
  → emit fused execution group or ordinary stages
```

## Example

User selects:

```text
Restore Video → OpenProteus Quality
```

Family declares:

```ini
family_id=openproteus-compact-x2
capabilities=deblur,upscale
fused=true
selective=false
```

Planner emits:

```text
FusedAIGroup(openproteus-compact-x2, capabilities=[deblur, upscale])
```

Not:

```text
deblur pass → upscale pass
```

## Acceptance tests

- A fused family with exact requested full capabilities collapses into one group.
- A non-fused family never collapses into one group.
- A selective family collapses valid subsets.
- An advanced model still works by raw id.
- Existing custom `.avemodel` manifests remain loadable.
- GUI and CLI resolve to equivalent pipelines.
