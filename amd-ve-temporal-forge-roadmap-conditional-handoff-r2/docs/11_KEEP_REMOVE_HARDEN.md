# Keep, Remove, Refactor, Harden

## Keep and harden

### C++20 core

Keep. This is a major strength.

Hardening tasks:

- enforce clearer interfaces around stage plans
- add more tests around planning and backend selection
- keep CLI/GUI shared pipeline parity

### FFmpeg media spine

Keep. It is essential for a real video app.

Hardening tasks:

- ensure preview cache and full export use consistent decode/probe assumptions
- preserve audio/caption handling rules
- add clearer errors for unsupported formats

### MiGraphX-first backend

Keep. This is core identity.

Hardening tasks:

- improve artifact cache diagnostics
- document shape buckets
- precompile common preview/output shapes
- add failure explanations when compile/runtime validation fails

### ROCm/HIP fallback

Keep.

Hardening tasks:

- clarify supported model shapes
- expose fallback reason in UI diagnostics
- avoid user-facing confusion when fallback path differs in quality/performance

### Vulkan Compute backend

Keep and promote strategically.

This backend becomes more important for Temporal Forge because non-ML reconstruction should likely live in Vulkan/compute rather than MiGraphX.

### NCNN Vulkan backend

Keep as Advanced/Legacy compatibility.

Do not make it central unless specific model families require it.

### GLSL/libplacebo backend

Keep.

Useful for shader filters, CAS/RCAS-style passes, and compatibility. Do not confuse it with the Temporal Forge core.

### Custom model manifests

Keep and expand.

Hardening tasks:

- add family-card metadata
- add visibility tier metadata
- add speed/quality/MiGraphX fit metadata
- add recommended presets

## Remove or demote from primary UX

### Raw model sprawl

Remove from primary UI. Keep in Advanced/Legacy.

### Duplicate model variants

Demote. The main UI should not show every export/format variant.

### Backend jargon on main screen

Demote to Advanced.

### Wizard feeling

Refactor. The app should feel like a viewer with controls, not a multi-step form.

### Small dual-label preview

Replace. The new preview should be a large comparison canvas.

## Refactor

### Planner

Refactor into layers:

1. deterministic stage order
2. family/fusion grouping
3. temporal plan builder

### GUI

Refactor around:

- top rail
- preview canvas
- right inspector
- bottom timeline
- drawers

### Model manager UI

Refactor from model list to family inventory.

Support views:

- Installed families
- Optional downloads
- Advanced models
- Broken/unsupported models

### Settings

Refactor settings into:

- User preferences
- Backend/runtime settings
- Cache settings
- Advanced experimental flags

## Do not remove

- CLI frontend
- profile save/load capability
- existing custom model manifest loading
- fallback backends
- FFmpeg fallback path
- packaging scripts
- release status / support-tier honesty docs

## Technical debt to track

- inconsistent preview/export behavior
- unsupported/broken models remaining visible
- lack of full preview cache system
- limited temporal packet abstraction
- no first-class concept of confidence maps
- no UI for temporal debug overlays
- direct FSR/Redstone feasibility unknown for Linux-native app
