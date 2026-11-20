# Preview, Timeline, and Cache Spec

## Goal

Preview must become a first-class product system, not a small before/after afterthought.

## Core concept

Rendering and viewing are separate but connected.

```text
Preview render creates real enhanced frames into a cache.
Viewer plays/scrubs whatever contiguous cached frames exist.
```

## Components

### PreviewCache

Responsible for:

- storing rendered preview frames
- mapping frames to timestamps
- tracking validity against settings/model/input hash
- exposing contiguous playable ranges
- invalidating stale ranges
- supporting comparison modes

### PreviewJob

Represents a render request for a time segment.

Fields:

```text
input_path
input_fingerprint
segment_start
segment_end
frame_rate
pipeline_hash
family_id
settings_hash
output_resolution
cache_path
state
progress
```

### TimelineSegment

User-created or system-created segment.

Fields:

```text
id
start_time
end_time
label
state
cache_coverage
warnings
```

## Cache validity

A cached frame is valid only if these match:

- input file fingerprint
- selected mode/family
- pipeline hash
- model artifact hash
- strength/settings hash
- output resolution
- color/output format
- relevant backend version

## Playback rules

- Play only cached frames.
- Seeking outside cache pauses at nearest available frame or asks to render.
- Playback speed may exceed realtime only if cache can deliver frames.
- Segment loop playback must not trigger hidden rendering unless explicitly enabled.

## Split-wipe implementation contract

For each preview frame index, viewer needs:

```text
original_frame
processed_frame
metadata
```

The split wipe must be UI-side composition, not a baked video export requirement.

## Preview cache states

| State | Meaning |
|---|---|
| Missing | No frame exists |
| Rendering | Frame requested/in progress |
| Ready | Frame available |
| Stale | Frame exists but settings changed |
| Error | Frame render failed |

## Segment-level states

| State | Meaning |
|---|---|
| Empty | no cached frames |
| Partial | some contiguous frames available |
| Complete | full segment cached |
| Stale | segment cache invalid |
| Error | render failed |

## UI controls

Required:

- play/pause
- step frame forward/back
- jump to segment start/end
- mark in/out
- preview segment
- render ahead
- clear cache for segment
- playback speed selector
- split/side-by-side/original/enhanced toggle

## Storage policy

Default cache root:

```text
~/.cache/amd-ve/previews/
```

User settings:

- max cache size
- clear on exit toggle
- cache root override
- keep successful previews toggle

## Phase 2 extension

Temporal Forge will use the same timeline/cache system, but with additional analysis products:

```text
motion vectors
proxy depth
reactive mask
confidence map
occlusion mask
scene cut markers
ai allowed mask
```

These should live beside frames in the cache with matching timestamps and pipeline hashes.

## Acceptance tests

- Cached preview survives app restart if settings match.
- Cached preview invalidates when model/settings change.
- Split wipe displays matched original/processed frames.
- Timeline correctly reports partial and complete ranges.
- Playback never silently crosses into unavailable frames.
