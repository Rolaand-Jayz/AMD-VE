# UI/UX Workstation Spec

## UX principle

The viewer is the product.

All controls should orbit the preview experience. The user should judge visual changes first and only then inspect model/backend details.

## Main screen layout

```text
┌────────────────────────────────────────────────────────────────────────────┐
│ Top rail: input | output | backend status | active family | render controls │
├───────────────────────────────────────┬────────────────────────────────────┤
│                                       │ Right inspector                    │
│                                       │ - selected mode                    │
│ Large preview canvas                  │ - family card                      │
│ - split wipe default                  │ - capability chips                 │
│ - side-by-side optional               │ - strength sliders                 │
│ - overlays optional                   │ - advanced foldout                 │
│                                       │                                    │
├───────────────────────────────────────┴────────────────────────────────────┤
│ Timeline: seek | cache lane | segments | transport | speed | mark in/out    │
└────────────────────────────────────────────────────────────────────────────┘
```

## Top rail

### Required elements

- Current input file name
- Output preset/target
- Active backend status
- Active enhancement family chip
- Preview button
- Render button
- Pause/cancel controls
- Queue/log drawer buttons

### Design rule

The top rail should not become a settings strip. Keep it operational.

## Preview canvas

### Default mode: split wipe

- Original on one side.
- Enhanced on the other.
- Divider can be dragged.
- Divider can auto-sweep.
- Labels appear subtly near divider edge.
- User can swap sides.
- Zoom/pan are locked across original/enhanced.

### Required preview modes

| Mode | Purpose |
|---|---|
| Split wipe | Best default for subtle comparison |
| Side-by-side | Good for broad composition/texture comparison |
| Original only | Debug/inspect source |
| Enhanced only | Inspect final output |
| Freeze compare | Lock one frame and inspect |
| Overlay/debug | For Temporal Forge masks later |

### Divider behavior

- Drag with pointer.
- Double-click resets to 50%.
- Button toggles auto-sweep.
- Auto-sweep speed should be slow and adjustable.
- Holding modifier pauses sweep.
- Divider must not hide important center pixels with a thick handle.

## Right inspector

### Simple mode content

- Mode selector
- Family card
- Capability chips
- Strength sliders
- Output target
- One-sentence explanation

Example family card language:

```text
OpenProteus Quality
Best for clean live-action HD/FHD → higher resolution with restrained sharpening.
Capabilities: deblur, upscale, light artifact cleanup
```

### Advanced foldout

- raw model id
- backend preference
- precision
- compile shape
- tile size
- cache path
- individual stage params
- planner preview

Advanced controls should be collapsed by default.

## Timeline

### Required lanes

1. **Source time ruler**
2. **Segment lane**
3. **Preview cache lane**
4. **Temporal/debug lane** for Phase 2

### Cache lane states

| State | Meaning |
|---|---|
| Empty | Not rendered |
| Analyzing | Phase 2 analysis / future use |
| Rendering | Preview frames being generated |
| Cached | Playable preview exists |
| Warning | Cached but quality risk exists |
| Invalid | Cache must be regenerated |

## Segment flow

1. User selects a region.
2. User clicks `Preview Segment`.
3. App renders frames into preview cache.
4. Timeline marks contiguous playable regions.
5. User scrubs/replays/speed-adjusts cached section.
6. User adjusts controls and re-renders the segment if needed.
7. User commits full render.

## 2D→3D SBS mode

2D→3D does not need fancy split comparison at first.

Required:

- Show selected SBS format.
- Show output preview as SBS.
- Allow depth/divergence strength controls.
- Use one visible best depth/stereo family.
- Hide alternate depth models unless Advanced/Legacy is enabled.

## Temporal Forge future overlays

Reserve UI surface for:

- motion confidence
- proxy depth
- reactive mask
- occlusion/disocclusion mask
- scene-cut reset markers
- ghost-risk overlay
- AI-allowed mask

These overlays should not exist as fake controls before the engine produces data.

## UX language rules

Prefer:

- Restore
- Clean
- Sharpen
- Recover detail
- Preserve grain
- Compare
- Preview segment
- Render cache

Avoid primary-surface jargon:

- tensor
- opset
- MiGraphX compile shape
- model id
- fused capability set
- backend artifact manifest

Those belong in Advanced.

## Accessibility and responsive layout contract

The workstation must remain usable without perfect pointer control or a large monitor.

### Keyboard map

| Action | Required keyboard path |
|---|---|
| Open/select input | menu action or shortcut |
| Preview selected segment | focusable button and shortcut |
| Render full job | focusable button and shortcut |
| Cancel/pause current job | focusable button and shortcut |
| Move split-wipe divider | focused divider + arrow keys |
| Reset split-wipe divider | focused divider + Enter/Space or context action |
| Seek timeline | focused timeline + arrows/PageUp/PageDown/Home/End |
| Toggle original/enhanced/split/SBS | focusable segmented control |
| Open advanced foldout | focusable disclosure control |
| Open logs/queue | focusable drawer control |

### Focus order

Default focus order:

1. top rail operational controls,
2. preview mode controls,
3. preview canvas/split divider,
4. timeline/transport,
5. right inspector simple controls,
6. advanced foldout,
7. queue/log drawers.

Focus must remain visible at all times. Modal dialogs must trap focus until closed.

### Minimum accessibility requirements

- Critical controls must have accessible names/tooltips.
- Preview, render, cancel, timeline, split divider, advanced foldout, and error indicators must be discoverable by keyboard.
- Text must remain readable at supported minimum window size.
- Touch/click targets for primary actions should be at least 40x40 CSS/UI pixels where toolkit conventions allow.
- Progress/cancel state must be visible in the UI and recorded in logs.
- Errors must not rely on color alone.
- Color contrast should target WCAG AA-equivalent readability for text and key controls.

### Responsive layout breakpoints

| Window class | Behavior |
|---|---|
| wide desktop | normal top rail + central preview + inspector + timeline |
| medium desktop | inspector can collapse to drawer; preview remains primary |
| narrow / low-resolution | inspector and logs become drawers/tabs; timeline remains accessible below preview |

Do not let backend/model debug controls displace the preview canvas on small windows. The viewer stays king.
