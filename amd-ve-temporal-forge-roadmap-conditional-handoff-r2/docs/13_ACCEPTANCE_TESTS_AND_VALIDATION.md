# Acceptance Tests and Validation

## Phase 1 validation

### Build tests

Run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DAVE_HAVE_CURL=ON \
  -DAVE_HAVE_MIGRAPHX=ON \
  -DAVE_HAVE_HIP=ON \
  -DAVE_HAVE_VULKAN=ON \
  -DAVE_HAVE_NCNN=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

### GUI smoke tests

- Launch GUI.
- Load video.
- Select Restore Video.
- Confirm default family chosen.
- Preview segment.
- Use split wipe.
- Scrub cached segment.
- Change strength slider.
- Confirm cache invalidates.
- Render short output.

### CLI parity tests

- Save GUI profile.
- Export/inspect equivalent pipeline.
- Run CLI dry-run with same stages.
- Confirm planner output matches expected family/fused behavior.

### Model family tests

- Visible family count is limited.
- Advanced/Legacy exposes full list.
- Broken/unsupported models do not appear as recommended defaults.
- Custom `.avemodel` still loads.

### Fusion tests

- Non-fused family stays separate.
- Fused non-selective family collapses exact capability set.
- Fused selective family collapses valid subsets.
- Wrong subset does not silently fuse.

## Phase 2 validation

### Temporal packet tests

- Packet can represent current/previous frames.
- Packet can omit optional future frames.
- Packet carries motion/depth/masks/exposure/reset.
- Packet invalidates on seek/scene cut.

### Motion tests

Test clips:

- slow pan
- fast pan
- object moving across static background
- hard scene cut
- subtitles over motion

Acceptance:

- Motion vectors are visualizable.
- Scene cut resets history.
- Motion confidence drops on failure.

### Depth tests

Test clips:

- foreground person/background wall
- anime line art
- flat footage
- camera pan

Acceptance:

- Proxy depth provides useful discontinuities.
- Bad depth does not corrupt output globally.

### Mask tests

- subtitles protected
- UI/text protected
- scene cut reset
- fast motion reactive zones marked
- grain policy respected

### Reconstruction tests

Compare:

1. spatial-only model
2. temporal-only baseline
3. AI-only enhancement
4. hybrid temporal+AI

Acceptance:

- Hybrid does not create worse ghosting than temporal-only.
- Hybrid improves weak regions over temporal-only.
- Hybrid flickers less than AI-only.

## Benchmark clips

Create a small benchmark suite:

| Clip type | Reason |
|---|---|
| low-bitrate live action | compression stress |
| anime pan | line shimmer/edge stability |
| subtitles | text protection |
| camera pan over texture | temporal detail recovery |
| fast action | motion failure handling |
| old SD footage | restoration realism |
| clean 1080p | avoid damaging good input |

## Metrics report template

```markdown
# Benchmark Result

## Build
- commit:
- hardware:
- driver:
- backend:

## Clip
- name:
- source resolution:
- target resolution:
- duration:

## Pipeline
- mode:
- family:
- temporal settings:
- refiner:

## Results
- FPS:
- VRAM:
- SSIM/LPIPS if available:
- temporal stability notes:
- ghosting notes:
- text/subtitle notes:
- visual verdict:

## Artifacts
- source sample:
- output sample:
- overlay captures:
```
