# Temporal Engine Architecture

## Architecture overview

```text
Temporal Forge Engine
├── TemporalFrameWindow
├── BufferProviderRegistry
│   ├── ColorProvider
│   ├── MotionProvider
│   ├── DepthProvider
│   ├── ReactiveMaskProvider
│   ├── ConfidenceProvider
│   ├── ExposureProvider
│   └── SceneResetProvider
├── TemporalCache
├── ReconstructionBackend
│   ├── AMD/FidelityFX-inspired backend
│   ├── Vulkan compute backend
│   ├── CPU reference backend
│   └── future vendor backends
└── MLRefinerBackend
    ├── MiGraphX
    ├── ROCm/HIP ONNX Runtime
    ├── NCNN/Vulkan
    └── portable ONNX backend
```

## TemporalFrameWindow

Represents the frames needed for reconstruction.

Fields:

```cpp
struct TemporalFrameWindow {
    Frame current;
    std::vector<Frame> previous;
    std::vector<Frame> future; // optional for offline mode
    int64_t frameIndex;
    double timestampSec;
    double frameDurationMs;
};
```

## TemporalFramePacket

The unit passed to reconstruction.

Fields:

```cpp
struct TemporalFramePacket {
    Frame colorCurrent;
    Frame colorPrevious;
    OptionalFrame precleanCurrent;
    MotionBuffer motionCurrentToPrevious;
    DepthBuffer proxyDepth;
    Mask reactiveMask;
    Mask transparencyCompositionMask;
    Mask occlusionMask;
    Mask confidenceMap;
    ExposureInfo exposure;
    SceneResetInfo reset;
    TemporalHistoryRef history;
};
```

## Buffer formats

Recommended internal formats:

| Buffer | Format | Notes |
|---|---|---|
| Color | RGBA16F or RGB16F | avoid repeated 8-bit quantization |
| Motion | RG16F / packed block vectors | start with block vectors if using FidelityFX Optical Flow |
| Depth | R16F or R32F | normalized proxy depth, not metric depth |
| Masks | R8_UNORM or R16F | R8 for UI masks, R16F for confidence if needed |
| Exposure | scalar + histogram | auto exposure / luminance stability |
| History | implementation-specific | reconstructed frame + locks/confidence |

## ReconstructionBackend interface

```cpp
class TemporalReconstructionBackend {
public:
    virtual bool isAvailable(std::string& reason) const = 0;
    virtual bool initialize(const TemporalBackendConfig&, std::string& error) = 0;
    virtual TemporalResult processPacket(const TemporalFramePacket&, std::string& error) = 0;
    virtual void resetHistory() = 0;
};
```

## MLRefinerBackend interface

```cpp
class MLRefinerBackend {
public:
    virtual bool supportsMaskedRefinement() const = 0;
    virtual RefinedFrame refine(
        const Frame& temporalResult,
        const Frame& sourceCurrent,
        const Mask& aiAllowedMask,
        const RefinerParams& params,
        std::string& error) = 0;
};
```

## AMD Golden Path

Initial production preference:

```text
Video decode/frames
  → pre-clean via existing pipeline if needed
  → AMD Optical Flow or equivalent motion provider
  → depth/proxy provider from 2D→3D path
  → masks/confidence
  → Vulkan/compute reconstruction
  → MiGraphX masked refiner
  → preview/export
```

## Hardware-agnostic path

Do not hardcode the engine to AMD-only types.

Use provider/backend interfaces so later implementations can support:

- Vulkan compute
- CPU reference
- ONNX Runtime
- NCNN Vulkan
- OpenVINO
- DirectML
- future vendor APIs

## Clean-room rule

Implement based on:

- public FSR/temporal reconstruction concepts
- official SDK docs
- public papers
- original code
- original training/evaluation work

Do not copy redacted FSR4 source into public mainline without AMD permission/legal clearance.
