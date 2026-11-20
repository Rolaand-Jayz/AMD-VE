# Phase 3 — AMD Outreach and Community Program

## M3.1: AMD outreach package

### One-page pitch

AMD Video Enhancer is a clean-room, open-source, AMD-first video restoration and upscaling workstation for Linux. It uses ROCm/MiGraphX, Vulkan compute, and ONNX Runtime to deliver production-quality video enhancement on AMD GPUs.

Key differentiators:
- Temporal Forge: video-native temporal reconstruction (not frame-by-frame)
- Clean-room: no leaked/redacted source from any vendor
- Open source: GPLv3, community-driven
- AMD hardware first: ROCm, MiGraphX, RDNA-optimized Vulkan

### Ask list for AMD

1. Guidance on official FidelityFX SDK integration for optical flow
2. Feedback on MiGraphX model compilation workflow
3. Possible GPUOpen collaboration for temporal reconstruction techniques
4. MiGraphX roadmap awareness for model compatibility planning
5. Access to AMD hardware for benchmarking (RDNA 2/3/4 matrix)

### IP cleanliness statement

All Temporal Forge code is clean-room. No FSR, DLSS, or XeSS source code (leaked, redacted, or otherwise) has been used. Architecture decisions are based on publicly published papers and techniques.

### Demo requirements

- Before/after clips: 1080p source upscaled to 4K with temporal reconstruction
- Side-by-side comparison: spatial-only vs temporal vs hybrid
- Failure cases shown honestly
- Runtime benchmarks on RX 7900 XTX

## M3.2: Public technical paper

### Paper outline

1. Introduction: video restoration problem space
2. Temporal-first approach: why frame-by-frame is suboptimal
3. Architecture: motion estimation, depth guidance, mask pipeline, reconstruction
4. Non-ML baseline: reprojection + accumulation results
5. ML enhancement: confidence-guided AI refinement
6. Evaluation: quantitative metrics (PSNR, SSIM, LPIPS) + qualitative analysis
7. Failure cases: fast motion, scene cuts, text rendering
8. Performance: AMD GPU benchmarks, real-time preview capability
9. Conclusion and future work

### Publishing requirements

- arXiv preprint
- Self-hosted PDF with reproducible benchmarks
- All benchmark clips redistributable (Creative Commons)
- No overclaiming

## M3.3: Community testing program

### Test matrix

| Hardware | Generation | Driver | Status |
|----------|-----------|--------|--------|
| RX 7900 XTX | RDNA 3 | ROCm 6.x | Primary |
| RX 7800 XT | RDNA 3 | ROCm 6.x | Secondary |
| RX 6800 XT | RDNA 2 | ROCm 6.x | Community |
| RX 6700 XT | RDNA 2 | ROCm 6.x | Community |
| Ryzen 7 7800X3D | CPU only | N/A | Fallback |
| RX 9070 XT | RDNA 4 | ROCm 6.x | Future |

### Known-good test clips

| Clip | Resolution | Content | Purpose |
|------|-----------|---------|---------|
| nature_1080p.mp4 | 1920x1080 | Nature landscape | General quality |
| anime_720p.mp4 | 1280x720 | Cel animation | Edge preservation |
| archival_vhs.mp4 | 720x480 | Compressed SD | Artifact removal |
| sports_1080p.mp4 | 1920x1080 | Fast sports | Motion handling |
| film_grain.mp4 | 1920x1080 | 35mm film scan | Grain policy |
| game_capture.mp4 | 1920x1080 | Game with HUD | Text protection |

### Issue templates

- Bug: backend failure, crash, incorrect output
- Quality: enhancement artifacts, temporal flicker, edge smearing
- Performance: slow frame time, high VRAM, thermal throttling
- Hardware: new GPU support, driver compatibility

### Benchmark submission format

```json
{
  "hardware": "RX 7900 XTX",
  "driver": "ROCm 6.2",
  "clip": "nature_1080p.mp4",
  "preset": "balanced",
  "frames_per_second": 22.5,
  "vram_peak_mb": 3200,
  "psnr_db": 34.2,
  "ssim": 0.92,
  "temporal_flicker": 0.003,
  "notes": ""
}
```
