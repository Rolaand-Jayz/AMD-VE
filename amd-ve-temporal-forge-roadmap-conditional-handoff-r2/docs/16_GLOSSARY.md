# Glossary

## AMD Golden Path

The best-supported AMD-first implementation path. It should use AMD hardware, AMD-friendly APIs, MiGraphX/ROCm/Vulkan where appropriate, and should be the first path optimized and documented.

## AI allowed mask

A mask that controls where AI refinement may influence the final image. It should be strong in low-confidence regions and weak in high-confidence/protected regions.

## Confidence map

A per-pixel or per-region estimate of how much the engine trusts temporal reconstruction.

## FSR-style

A design inspired by temporal upscaling systems such as AMD FSR: motion vectors, depth, reactive masks, exposure, jitter, history, and reconstruction through time. This does not mean copying or branding as AMD FSR.

## Fused family

A model family that can perform multiple enhancement capabilities in one pass.

## Model family

A user-facing group of one or more model files that serve the same purpose or capability set.

## Proxy depth

A depth/disparity-like buffer generated from ordinary video. It is not true game-engine depth. It is used for edges, occlusion, and history safety.

## Reactive mask

A mask marking pixels where current-frame data should have more influence and history should be reduced.

## Temporal Forge

The proposed hero mode for AMD-VE: video-native temporal reconstruction with confidence-guided AI refinement.

## Temporal reconstruction

Reconstructing output using current and neighboring frames rather than treating each frame independently.

## TemporalFramePacket

The packet containing a frame plus all side buffers needed by the temporal reconstruction backend.

## Video-to-FSR adapter layer

The conceptual layer that synthesizes game-style FSR inputs from ordinary video.
