# Temporal Forge Backend Integration — M2.9/M2.10

Status: experimental stub

## M2.9: AMD-first production backend

### MiGraphX integration path

The Temporal Forge refiner model exports to ONNX, then compiles via MiGraphX:
1. Load ONNX model via `migraphx::load()`
2. Compile with `migraphx::compile()` targeting `gpu` target
3. Save compiled `.mxr` to `~/.local/share/ave/models/migraphx/`
4. At inference: load `.mxr`, allocate GPU tensors, run evaluation

### Vulkan compute reconstruction path

Non-ML temporal reconstruction (reprojection, accumulation, clamp) can run as Vulkan compute shaders:
- `temporal_reproject.comp` — warp history frame using motion field
- `temporal_accumulate.comp` — accumulate warped history with confidence weighting
- `temporal_sharpen.comp` — RCAS-style final sharpening pass

These shaders exist as stubs until frame I/O Vulkan interop is integrated.

### Cache-aware preview pipeline

Temporal Forge preview must not re-process unchanged frames:
1. Cache key includes: motion field hash, depth hash, confidence hash, preset, model version
2. Cache invalidation on scene change (full reset)
3. Preview segments reuse cached temporal packets when available

### Performance targets (AMD RX 7900 XTX)

| Component | Target | Notes |
|-----------|--------|-------|
| Motion estimation | < 5ms/frame | GPU optical flow |
| Depth estimation | < 15ms/frame | Depth Anything V2 Small |
| Temporal reconstruction | < 3ms/frame | Vulkan compute |
| AI refinement | < 20ms/frame | MiGraphX compiled |
| Total per frame | < 50ms | For real-time preview at 20fps |

## M2.10: Hardware-agnostic expansion

### Backend abstraction principle

The Temporal Forge pipeline uses provider interfaces (MotionProvider, DepthProvider, etc.) that are GPU-agnostic. The same pipeline runs on:
- AMD ROCm/MiGraphX (first-class)
- Vulkan compute (portable)
- CPU reference (testing, CI)
- ONNX Runtime (alternative inference)

### CPU reference path

All reference providers (CpuBlockMotionProvider, PseudoDepthProvider, CpuTemporalReconstructor) run on CPU. They are:
- Correct but slow
- Used for testing and CI
- Used for regression validation against GPU paths

### Vulkan portable path

Vulkan compute shaders compile via glslc and run on any Vulkan 1.2+ GPU. This provides:
- AMD GPU support (ROCm Vulkan)
- Intel GPU support
- NVIDIA GPU support (via Vulkan, not CUDA)
- ARM GPU support (mobile, embedded)

### ONNX Runtime path

ONNX Runtime with ROCm/HIP execution provider provides:
- Alternative to MiGraphX for model inference
- Broader model compatibility
- CPU fallback for testing

### Documentation separation

AMD Golden Path docs clearly separate:
- What requires AMD hardware (MiGraphX, ROCm, hipRTC)
- What is portable (Vulkan compute, ONNX Runtime, CPU reference)
- What is AMD-optimized but portable (Vulkan shaders tuned for RDNA)
