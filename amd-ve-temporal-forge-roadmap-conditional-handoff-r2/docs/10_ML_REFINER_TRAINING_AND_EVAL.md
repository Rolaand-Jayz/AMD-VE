# ML Refiner Training and Evaluation Spec

## Principle

The first ML component in Temporal Forge should be a **masked refiner**, not a full all-powerful upscaler.

Temporal reconstruction produces the truthful base. ML improves weak regions.

## Refiner inputs

Minimum:

- temporal reconstruction result
- current source frame
- AI allowed mask
- temporal confidence map

Preferred:

- motion vectors
- proxy depth
- occlusion mask
- reactive mask
- precleaned frame
- local luma/exposure features

## Refiner outputs

Minimum:

- refined frame

Optional:

- predicted residual
- uncertainty map
- hallucination risk map

## Recommended model behavior

The refiner should learn residual correction:

```text
refined = temporal_result + learned_residual * ai_allowed_mask
```

This is safer than making the model output the whole frame.

## Dataset requirements

### Source types

- clean high-quality footage
- compressed online video
- old SD footage
- anime/line art
- camera pans
- high-detail textures
- faces/hair
- text/subtitles
- fast motion
- low light/noisy footage

### Degradation pipeline

Generate training pairs by degrading high-quality video:

- downscale
- bitrate compression
- H.264/H.265 artifacts
- noise/grain
- blur
- ringing
- chroma subsampling damage
- repeated encode damage

### Temporal windows

Train/evaluate on windows:

- 3-frame
- 5-frame
- 7-frame

Do not only train on independent frames.

## Metrics

Use a mix of objective and perceptual metrics.

| Metric | Purpose |
|---|---|
| PSNR/SSIM | basic fidelity, not enough alone |
| LPIPS/DISTS | perceptual similarity |
| tOF/tLPIPS or temporal consistency proxy | flicker/stability |
| edge preservation | line/text stability |
| ghosting score | temporal artifact detection |
| hallucination review | human/manual or model-assisted review |
| speed/FPS | practical usability |
| VRAM | deployment feasibility |

## Benchmark comparisons

Always compare:

1. source → standard spatial model
2. source → temporal-only baseline
3. source → AI-only enhancement
4. source → Temporal Forge hybrid
5. source → no enhancement / conventional scaler

## Export targets

Preferred:

- ONNX with MiGraphX compatibility
- FP16 first
- INT8 only after visual validation

Do not optimize only for theoretical speed. Visual stability matters more.

## Acceptance criteria for first refiner

- Improves low-confidence regions without altering high-confidence regions too much.
- Does not smear subtitles/text.
- Does not create obvious temporal flicker.
- Runs on the reference AMD GPU within preview-acceptable speed.
- Can be disabled cleanly.

## Stop criteria

Pause training/model integration if:

- ML refiner worsens temporal stability
- output becomes obviously hallucinated
- model cannot export to a supported runtime
- results cannot beat the non-ML temporal baseline on key clips

## Evaluation budget and benchmark reporting contract

The masked refiner is not allowed to graduate from experiment to product behavior until evaluation reports prove it improves the right thing.

### Required baselines

Each benchmark run must compare:

1. source-only / no enhancement,
2. current spatial-only AMD-VE path,
3. temporal-only reconstruction,
4. AI-only or existing model-only refinement where applicable,
5. hybrid Temporal Forge result.

### Required metrics

| Metric | Required? | Notes |
|---|---:|---|
| runtime/frame | yes | Report mean, p95 if available, and resolution. |
| peak VRAM/system RAM | yes when measurable | Record tool used. |
| temporal consistency/flicker proxy | yes | May start with frame-difference or optical-flow-compensated error. |
| text/subtitle stability | yes for UI/text clips | Manual score acceptable at first; automate later. |
| PSNR/SSIM | when paired truth exists | Do not force on unpaired footage. |
| perceptual metric | optional | LPIPS or equivalent only if dependency/license is acceptable. |
| human visual notes | yes | Must be tied to clip/timecode, not vibe-only. |

### Pass/fail rule

A refiner pass is valid only if it improves at least one target weakness without regressing protected regions beyond the documented tolerance.

A refiner fails if it creates plausible but unsupported detail in high-confidence temporal regions, smears text, destabilizes scene cuts, or hides failure by omitting debug masks.
