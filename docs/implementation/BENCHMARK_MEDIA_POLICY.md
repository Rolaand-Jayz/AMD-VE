# Benchmark Media Policy

## Purpose

Define what benchmark evidence requires and what transfer-archive media state proves.

## Benchmark Claims Require

- fixture path,
- fixture source or generation method,
- fixture hash,
- command used,
- environment (OS, GPU, driver, ROCm, MiGraphX, FFmpeg versions),
- output log,
- date,
- whether the result is maintainer-verified or community-submitted.

## Transfer Archive Media

benchmarks/ave_benchmark_960x540_20s.mp4 may appear modified, missing, or altered in transfer archives because benchmark media was removed or changed to save space. This does not change source-code handoff status. Agents must not treat the transferred benchmark media state as proof of benchmark reproducibility.

## Without Restored Media

Without restored media and hashes, benchmark docs may describe protocol but must not claim reproducible benchmark results from the transfer archive alone.

## Substitute Clips

A substitute clip may be used only for smoke testing or protocol demonstration unless its source or generation method, hash, command, environment, output log, date, and verification label (maintainer-verified or community-submitted) are recorded.

## Current Benchmark Fixture

- Path: `benchmarks/ave_benchmark_960x540_20s.mp4`
- Description: 600 frames, 960x540, 30 FPS, 20 seconds
- Source: generated via `tools/create_benchmark_clip.sh`
- Hash: not recorded in this handoff (fixture exists locally but hash was not captured during audit)
- Status: maintainer-verified on reference system only

Future benchmark evidence should include the fixture hash.
