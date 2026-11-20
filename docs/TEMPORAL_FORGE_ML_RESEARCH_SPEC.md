# ML Research Specification — Temporal Forge Model Training

Status: experimental (M2.8)

## Objective

Define the dataset generation, model architecture, training, and export pipeline for a Temporal Forge ML model that improves upon the CPU reference temporal reconstruction baseline.

## Clean-room policy

All model code must be clean-room. No FSR source, leaked or redacted, may be used. The architecture draws from publicly published techniques only.

## Dataset specification

### Degradation pipeline

Input: high-quality source frames (1080p+, clean, minimal compression).

Applied degradations (composable):
- H.264/H.265 compression at CRF 18–38
- Downscale to 540p/720p then upscale back (bicubic)
- Gaussian noise sigma 1–10
- Motion blur kernel 3–15px
- Film grain overlay
- Sharpening halos from prior processing
- Color space conversion artifacts

### Ground truth

The original high-quality frame before degradation is the ground truth target.

### Clip categories

| Category | Content | Motion | Purpose |
|----------|---------|--------|---------|
| live_action | People, nature, sports | Moderate-fast | General quality |
| anime | Cel animation, line art | Slow-medium | Edge preservation |
| archival | Compressed SD content | Slow | Artifact removal |
| game_ui | Game capture with HUD | Fast | Text/UI protection |
| fast_motion | Sports, action | Very fast | Motion handling |
| grainy | Film with visible grain | Slow | Grain policy |

### Dataset size targets

- Training: 50,000+ frame triplets (prev, current, next)
- Validation: 5,000+ frame triplets
- Test: 2,000+ frame triplets, disjoint from train/val

## Architecture candidates

### Candidate A: Motion-conditioned U-Net

Input: current frame + motion-compensated history (3 frames)
Output: enhanced current frame
Parameters: ~2M (small), ~8M (medium)
Strengths: well-understood, good speed/quality tradeoff
Weaknesses: fixed window size

### Candidate B: Temporal attention transformer

Input: current frame + N history frames + motion fields
Output: enhanced current frame
Parameters: ~5M (small), ~20M (medium)
Strengths: variable window, attention over temporal context
Weaknesses: slower inference, higher VRAM

### Candidate C: Multi-scale temporal refiner

Input: current frame + temporal reconstruction + confidence mask
Output: refined current frame (delta from temporal)
Parameters: ~1M
Strengths: complements non-ML baseline, very fast
Weaknesses: depends on quality of temporal reconstruction

## Export targets

| Format | Backend | Priority |
|--------|---------|----------|
| ONNX opset 17+ | MiGraphX, ONNX Runtime | Required |
| MIGX (compiled) | MiGraphX native | Required |
| NCNN param/bin | NCNN Vulkan | Optional |
| SPIR-V compute | Vulkan compute | Future |

## Metrics

| Metric | What it measures | Threshold |
|--------|-----------------|-----------|
| PSNR | Pixel fidelity | > baseline |
| SSIM | Structural similarity | > baseline |
| LPIPS | Perceptual quality | > baseline |
| Temporal flicker | Frame-to-frame variance | < baseline |
| Edge stability | Text/edge preservation | > baseline |
| Inference time | ms/frame on RX 7900 XTX | < 50ms for preview |
| VRAM usage | Peak allocation | < 4GB |

## Training infrastructure

- ROCm PyTorch on AMD GPU
- Mixed precision (fp16/bf16)
- Learning rate: cosine schedule, warmup 1000 steps
- Batch size: 4–8 (GPU dependent)
- Optimizer: AdamW, weight decay 1e-4
- Loss: L1 + perceptual (VGG) + temporal consistency term

## Evaluation protocol

1. Compare against spatial-only baseline (single frame upscale)
2. Compare against non-ML temporal reconstruction
3. Compare against hybrid (temporal + AI)
4. Evaluate on each clip category separately
5. Document failure cases honestly
