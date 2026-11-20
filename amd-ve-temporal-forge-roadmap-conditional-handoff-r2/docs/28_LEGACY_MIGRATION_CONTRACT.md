# Legacy Manifest and Profile Migration Contract

## Purpose

The Phase 1 model-family redesign must not punish existing AMD-VE users for having older manifests, custom models, saved profiles, or raw model IDs.

This document defines the migration rules agents must implement after Phase 0 discovers the real manifest/profile formats.

## Compatibility principle

New family-first UX is a layer above existing model access. It must simplify the default workflow without deleting or silently changing expert workflows.

## Required migration behavior

| Legacy input | Required behavior | User-visible result | Test fixture required |
|---|---|---|---|
| Old manifest with raw model ID only | Load model as advanced/raw family member with generated compatibility family | Model appears under Advanced/Legacy, not lost | yes |
| Old manifest with known capability/stage | Map capability to new enum when unambiguous | Model can be selected through matching family or raw path | yes |
| Old manifest with unknown capability/stage | Load as advanced/raw with warning | User sees actionable compatibility warning | yes |
| Custom user model | Preserve raw ID and user path | User can still select custom model | yes |
| GUI profile referencing raw model | Resolve raw model path or show missing-model error | No silent fallback to wrong family | yes |
| CLI profile referencing old flags | Preserve old behavior or emit explicit migration warning | CLI remains deterministic | yes |

## Versioning rules

- New manifests must include `schema_version` or target-repo equivalent once the real manifest structure is known.
- Compatibility readers must accept at least the currently shipped manifest/profile version discovered in Phase 0.
- Migration must not rewrite user files in place without backup or explicit consent.
- Any automatic conversion must record source version, converted version, timestamp, and tool/app version.

## Warning language

Warnings should be specific and recoverable:

```text
Loaded legacy model manifest as Advanced/Legacy.
Reason: missing family_id and safe_defaults.
Model remains usable through raw model selection.
Recommended fix: add family metadata or run the manifest migration tool.
```

Do not use vague warnings like `invalid model` when the model is still usable.

## Required target-repo fixtures after Phase 0

Create or adapt fixtures from real examples:

```text
tests/fixtures/manifest/legacy_raw_model_minimal.json
tests/fixtures/manifest/legacy_known_stage.json
tests/fixtures/manifest/legacy_unknown_stage.json
tests/fixtures/manifest/custom_user_model.json
tests/fixtures/profile/legacy_cli_raw_model_profile.*
tests/fixtures/profile/gui_family_profile.*
```

## Expected converted family metadata example

A minimal legacy raw model should convert to a compatibility wrapper similar to:

```json
{
  "family_id": "legacy_raw_<stable_hash>",
  "family_name": "Legacy Raw Model",
  "display_purpose": "Imported legacy/custom model. Behavior preserved through Advanced/Legacy.",
  "content_type": "experimental",
  "speed_tier": "D",
  "quality_tier": "D",
  "migraphx_fit": "unknown",
  "visible_default": false,
  "advanced_only": true,
  "capabilities": ["upscale"],
  "safe_defaults": ["upscale"],
  "fused": false,
  "selective": false,
  "model_ids": ["legacy_raw_model_id"]
}
```

If capability cannot be inferred, use the safest target-repo fallback category and require a warning. Do not invent quality claims.

## Acceptance tests

A migration implementation is complete only when:

1. old manifests still load,
2. custom models still load,
3. raw model IDs still resolve,
4. new family metadata validates,
5. unknown fields do not crash the loader,
6. ambiguous mappings warn instead of silently changing output behavior,
7. GUI and CLI show the same migrated plan,
8. round-trip tests prove user profiles are not silently rewritten or corrupted.
