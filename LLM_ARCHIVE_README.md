# LLM-Digestible Source Archive

This `.tar` archive contains **only source code, documentation, configuration, and other text-based files** from the project.

The following categories were **intentionally excluded** to keep the archive small and LLM-digestible:

| Category | Why excluded |
| --- | --- |
| **Build output** (`build/`, `build_portable_beta/`, `build_release_root/`) | Compiled objects, binaries, generated files — not source |
| **Distribution packages** (`dist/`) | Pre-built native packages (`.deb`, `.rpm`, etc.) |
| **Git internals** (`.git/`) | Version-control metadata, not project source |
| **Model files** (`*.onnx`, `*.bin`, model caches) | Large binary model weights |
| **Binaries & shared libs** (`*.so`, `*.o`, `*.a`, `*.dll`) | Compiled artifacts |
| **Media files** (`*.mp4`, `*.avi`, `*.mkv`) | Video/media assets |
| **Python bytecode** (`*.pyc`, `__pycache__/`) | Derived, not source |
| **IDE/editor config** (`.vscode/`, `.idea/`, `.zed/`, `.crush/`) | Local editor state |
| **Cache directories** (`.cache/`, `.local/`) | Disposable caches |
| **CI/CD metadata** (`.azuredevops/`, `.github/` pipelines) | Infrastructure, not source logic |
| **Docker images / layers** | Container build artifacts |

To reconstitute the full project, clone the repository and run the build steps described in `README.md`.
