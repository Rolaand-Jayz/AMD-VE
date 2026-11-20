# Technical Contracts

## Purpose

This file converts roadmap concepts into data and behavior contracts. Agents should implement these contracts in the target repository using the repository's actual language, naming, and architecture.

Do not force these exact class names if the repo already has better conventions. Preserve the behavior and schema.

---

# 1. Mode definition contract

## Required fields

```cpp
struct EnhancementModeDefinition {
    std::string id;                  // stable machine id
    std::string label;               // user-facing name
    std::string purpose;             // one sentence
    std::string defaultFamilyId;     // empty only for advanced/manual modes
    std::vector<std::string> defaultCapabilities;
    std::vector<std::string> visibleControls;
    std::string previewBehavior;     // split_wipe, sbs_preview, motion_preview, etc.
    std::string outputBehavior;      // normal_video, sbs_video, interpolated_video, etc.
    bool experimental = false;
    bool advancedOnly = false;
};
```

## Required mode ids

| ID | Label | Experimental | Default preview |
|---|---:|---:|---|
| `restore_video` | Restore Video | false | split_wipe |
| `upscale_video` | Upscale Video | false | split_wipe |
| `anime_line_art` | Anime / Line Art | false | split_wipe |
| `stereo_3d_sbs` | 2D→3D SBS | false | sbs_preview |
| `interpolate_motion` | Interpolate / Smooth Motion | false | motion_preview |
| `advanced_legacy` | Advanced / Legacy | false | split_wipe |
| `temporal_forge_experimental` | Temporal Forge | true | debug_overlay |

## Validation rules

- `id` must be stable and lowercase snake case.
- Non-advanced, non-experimental modes must resolve to a default family or a clear unavailable state.
- Experimental mode must be hidden, disabled, or clearly labeled until enabled by setting/flag/build option.
- A mode must serialize into a profile or CLI-equivalent config.

---

# 2. Family metadata contract

## Required fields

```cpp
struct ModelFamilyMetadata {
    std::string familyId;
    std::string familyName;
    std::string displayPurpose;
    std::string contentType;     // general, live_action, anime, archival, stereo, motion, experimental
    std::string speedTier;       // S, A, B, C, D
    std::string qualityTier;     // S, A, B, C, D
    std::string migraphxFit;     // S, A, B, C, D, unknown
    bool visibleDefault = false;
    bool advancedOnly = false;
    std::vector<std::string> capabilities;
    std::vector<std::string> safeDefaults;
    bool fused = false;
    bool selective = false;
    std::vector<StrengthControl> strengthControls;
    std::vector<std::string> modelIds;
};
```

```cpp
struct StrengthControl {
    std::string id;              // deblur_strength, artifact_strength, etc.
    std::string label;
    float min = 0.0f;
    float max = 1.0f;
    float defaultValue = 0.5f;
    float step = 0.01f;
    std::string capability;      // optional capability controlled by this value
};
```

## Capability enum

Use stable ids. If the repository already has equivalent `StageKind` values, map these to them.

```text
restore_compression
remove_artifacts
denoise
deblur
dehalo
color_fix
upscale
sharpen
stereo_3d
interpolate
```

## Validation rules

- `familyId` must be stable.
- `capabilities` must be non-empty unless the family is `advanced_legacy`.
- `safeDefaults` must be a subset of `capabilities`.
- `selective=true` is valid only if `fused=true` or if individual stages support capability toggles.
- `advancedOnly=true` must prevent the family/model from appearing in the simple primary workflow.
- Old manifests must still load through compatibility defaults.

---

# 3. Fusion planner contract

## Planner input

```cpp
struct PipelineIntent {
    std::string modeId;
    std::string familyId;                  // optional if raw model path
    std::optional<std::string> rawModelId;  // advanced/manual path
    std::vector<std::string> requestedCapabilities;
    std::map<std::string, double> strengthValues;
    BackendPreference backendPreference;
    PrecisionPreference precisionPreference;
};
```

## Planner output

```cpp
struct PlannedPipeline {
    std::vector<PlanNode> nodes;
    std::vector<PlannerDiagnostic> diagnostics;
    std::string stableHash;
};

struct PlanNode {
    enum class Type { OrdinaryStage, FusedAIGroup, TemporalForgeExperimental } type;
    std::string familyId;
    std::vector<std::string> capabilities;
    std::vector<std::string> modelIds;
    BackendPreference backend;
    PrecisionPreference precision;
};

struct PlannerDiagnostic {
    enum class Severity { Info, Warning, Error } severity;
    std::string code;
    std::string message;
};
```

## Fusion algorithm

```text
1. Resolve mode → default family and safe default capabilities.
2. Merge explicit user capability changes.
3. Convert capabilities to deterministic stage order.
4. Group consecutive stages with the same family.
5. For each group:
   a. If raw model path is active, emit ordinary/raw node.
   b. If family.fused=false, emit ordinary stage nodes.
   c. If requested capabilities contain unsupported capability, emit error or fallback diagnostic.
   d. If family.fused=true and family.selective=true, emit FusedAIGroup for any valid non-empty subset.
   e. If family.fused=true and family.selective=false:
      - emit FusedAIGroup only when requested set equals full required fused set or declared safe fused set.
      - otherwise emit fallback ordinary stages only if ordinary mappings exist; else emit actionable error.
6. Emit diagnostics explaining each fusion/fallback decision.
7. Generate stable hash from normalized plan fields.
```

## Required diagnostic codes

```text
FUSION_APPLIED
FUSION_NOT_SUPPORTED
FUSION_PARTIAL_NOT_SELECTIVE
FUSION_UNSUPPORTED_CAPABILITY
FUSION_FALLBACK_TO_ORDINARY_STAGES
RAW_MODEL_ADVANCED_PATH
MODE_EXPERIMENTAL_DISABLED
```

## Required tests

| Case | Input | Expected |
|---|---|---|
| Non-fused family | denoise+upscale | ordinary stage nodes |
| Fused non-selective exact | deblur+upscale matching full set | one fused group |
| Fused non-selective partial | deblur only when full set is deblur+upscale | fallback or error diagnostic |
| Fused selective subset | deblur only | one fused group |
| Unsupported capability | stereo_3d on live-action upscaler | error/fallback diagnostic |
| Raw model | raw id selected | no family fusion |

---

# 4. Preview cache contract

## Cache key inputs

The cache key must include:

```text
cache_schema_version
app_version_or_commit
source_identity
source_size
source_mtime_or_content_hash
start_time
end_time
timebase
mode_id
family_id_or_raw_model_id
requested_capabilities
strength_values
model_ids
model_checksums_if_available
backend
precision
output_resolution_or_scale
tile/settings_affecting_pixels
```

## Cache state enum

```text
empty
analyzing
rendering
cached
warning
invalid
```

## Preview cache manifest

```cpp
struct PreviewCacheManifest {
    int schemaVersion;
    std::string cacheKey;
    std::string sourcePath;
    TimeRange range;
    PipelineIntent intent;
    PlannedPipeline plan;
    std::vector<CachedFrame> frames;
    CacheState state;
    std::vector<std::string> warnings;
};

struct CachedFrame {
    int64_t frameIndex;
    double ptsSeconds;
    std::string originalFramePath;
    std::string enhancedFramePath;
    std::string debugOverlayPath; // optional
    std::string frameHash;        // optional but preferred
};
```

## Invalidation rules

Invalidate or mark warning when:

- source identity/mtime/size/hash changes
- mode/family/capabilities change
- strength values change
- model id/checksum changes
- backend or precision changes when output pixels may change
- app/cache schema version changes
- missing frame files
- incomplete render/cancel state exists

## Required tests

- Stable identical config produces identical key.
- Each key input mutation changes key.
- Missing frame file marks cache invalid.
- Cancelled preview never reports cached success.

---

# 5. Preview viewer contract

## Required viewer modes

```text
split_wipe
side_by_side
original_only
enhanced_only
freeze_compare
debug_overlay
```

## Split-wipe behavior

- Divider default is 0.5.
- Drag updates divider position.
- Double-click resets to 0.5.
- Auto-sweep is optional but must be slow, adjustable, and pausable.
- Zoom and pan must be locked across compared images.
- Missing enhanced frame shows explicit uncached/missing state.

## Timeline behavior

- A timeline segment can be empty, rendering, cached, warning, or invalid.
- The user can select mark-in/mark-out range.
- Preview segment action renders only the selected segment.
- Full render action remains separate.

---

# 6. Temporal Forge contracts

Temporal Forge contracts are experimental until explicitly promoted.

## Temporal frame window

```cpp
struct TemporalFrameWindow {
    std::vector<FrameRef> history;   // oldest → newest before current
    FrameRef current;
    std::vector<FrameRef> future;    // optional, nearest → farther
    bool sceneResetBeforeCurrent = false;
    Rational timebase;
};
```

## Temporal packet

```cpp
struct TemporalFramePacket {
    TemporalFrameWindow window;
    std::optional<MotionField> motionPrevToCurrent;
    std::optional<MotionField> motionCurrentToNext;
    std::optional<ImagePlane> depth;
    std::optional<ImagePlane> reactiveMask;
    std::optional<ImagePlane> transparencyCompositionMask;
    std::optional<ImagePlane> occlusionMask;
    std::optional<ImagePlane> confidenceMap;
    std::optional<ImagePlane> aiAllowedMask;
    std::vector<DebugArtifact> debugArtifacts;
};
```

## Provider interfaces

```cpp
class MotionProvider {
public:
    virtual ProviderResult<MotionField> estimate(const TemporalFrameWindow& window) = 0;
};

class DepthProvider {
public:
    virtual ProviderResult<ImagePlane> estimateDepth(const FrameRef& frame) = 0;
};

class MaskProvider {
public:
    virtual ProviderResult<ImagePlane> compute(const TemporalFramePacket& packet) = 0;
};

class ConfidenceProvider {
public:
    virtual ProviderResult<ImagePlane> computeConfidence(const TemporalFramePacket& packet) = 0;
};

class TemporalReconstructor {
public:
    virtual ProviderResult<FrameRef> reconstruct(const TemporalFramePacket& packet) = 0;
};

class MaskedAIRefiner {
public:
    virtual ProviderResult<FrameRef> refine(
        const FrameRef& source,
        const FrameRef& temporalReconstruction,
        const ImagePlane& confidence,
        const ImagePlane& aiAllowedMask) = 0;
};
```

## Provider result

```cpp
struct ProviderDiagnostic {
    enum class Severity { Info, Warning, Error } severity;
    std::string code;
    std::string message;
};

template <typename T>
struct ProviderResult {
    std::optional<T> value;
    std::vector<ProviderDiagnostic> diagnostics;
    bool ok() const;
};
```

## Temporal debug artifacts

Minimum debug outputs:

```text
motion_field_visualization
depth_visualization
reactive_mask
occlusion_mask
confidence_map
ai_allowed_mask
scene_reset_markers
history_weight_map
```

## Temporal guardrails

- Temporal Forge must not be implemented as only `StageKind::Upscale`.
- Non-ML reconstruction baseline comes before custom ML training.
- AI refiner may only modify where masks/confidence permit by default.
- Protected text/UI/subtitle regions must be conservative.
- Every prototype claim must have benchmark output.

## Schema and semantic validation contract

JSON Schema catches structure. Semantic validators catch cross-field behavior that standard JSON Schema cannot express cleanly.

Required semantic checks:

| Contract | JSON Schema check | Semantic validator check |
|---|---|---|
| mode definition | ID shape/known built-ins, required fields, ordinary modes require `default_family_id` and `cli_profile_key` | complete built-in mode set exists when validating a mode collection |
| family metadata | enum/type/required fields | `safe_defaults` must be a subset of `capabilities`; strength control default must sit between min/max |
| preview cache manifest | cached manifests must contain frames with `enhanced_frame_path` | `range.end_seconds` must be greater than `range.start_seconds`; cached frame paths must exist when validating against filesystem |
| temporal debug manifest | artifact kinds/paths shape | artifact files exist and match frame/timebase when validating a real run |

The reference validation harness lives in `tests/validate_schemas.py` and must be kept passing whenever schema behavior changes.

## Legacy migration contract pointer

Model-family redesign must follow `docs/28_LEGACY_MIGRATION_CONTRACT.md`.

The key rule: family-first UX is additive. Existing raw/custom model access remains valid unless the target repository already cannot load the model, in which case the failure must be explicit and recoverable.
