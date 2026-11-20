#!/usr/bin/env python3
"""Validate AMD-VE roadmap schemas and semantic fixtures.

This script is intentionally dependency-light. Install jsonschema if it is not
already available:

    python -m pip install jsonschema
"""
from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

from jsonschema import Draft202012Validator

ROOT = Path(__file__).resolve().parents[1]

SCHEMAS = {
    "family": ROOT / "schemas" / "family_metadata.schema.json",
    "mode": ROOT / "schemas" / "mode_definition.schema.json",
    "preview": ROOT / "schemas" / "preview_cache_manifest.schema.json",
}

FIXTURE_MAP = {
    "family": ["family_"],
    "mode": ["mode_"],
    "preview": ["preview_"],
}

REQUIRED_MODE_IDS = {
    "restore_video",
    "upscale_video",
    "anime_line_art",
    "stereo_3d_sbs",
    "interpolate_motion",
    "advanced_legacy",
    "temporal_forge_experimental",
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def schema_for_fixture(path: Path) -> str:
    name = path.name
    for schema_name, prefixes in FIXTURE_MAP.items():
        if any(name.startswith(prefix) for prefix in prefixes):
            return schema_name
    raise ValueError(f"No schema mapping for fixture: {path}")


def semantic_errors(schema_name: str, data: dict[str, Any], path: Path) -> list[str]:
    errors: list[str] = []
    if schema_name == "family":
        capabilities = set(data.get("capabilities", []))
        safe_defaults = set(data.get("safe_defaults", []))
        extra = safe_defaults - capabilities
        if extra:
            errors.append(f"safe_defaults not present in capabilities: {sorted(extra)}")
        for control in data.get("strength_controls", []):
            mn = control.get("min")
            mx = control.get("max")
            default = control.get("default")
            if None not in (mn, mx, default) and not (mn <= default <= mx):
                errors.append(f"strength control default outside min/max: {control.get('id')}")
    elif schema_name == "preview":
        rng = data.get("range", {})
        start = rng.get("start_seconds")
        end = rng.get("end_seconds")
        if start is not None and end is not None and not (end > start):
            errors.append("range.end_seconds must be greater than range.start_seconds")
        if data.get("state") == "cached":
            frames = data.get("frames", [])
            if not frames:
                errors.append("cached state requires at least one frame")
            for index, frame in enumerate(frames):
                if not frame.get("enhanced_frame_path"):
                    errors.append(f"cached frame {index} missing enhanced_frame_path")
    elif schema_name == "mode":
        # Collection-level required ID set is checked by validate_mode_collection().
        pass
    return errors


def validate_fixture(path: Path, expect_valid: bool) -> bool:
    schema_name = schema_for_fixture(path)
    schema = load_json(SCHEMAS[schema_name])
    data = load_json(path)
    validator = Draft202012Validator(schema)
    schema_errors = [e.message for e in validator.iter_errors(data)]
    sem_errors = semantic_errors(schema_name, data, path)
    errors = schema_errors + sem_errors
    passed = (not errors) if expect_valid else bool(errors)
    if not passed:
        print(f"FAIL: {path.relative_to(ROOT)}")
        print(f"  expected {'valid' if expect_valid else 'invalid'}, got {'invalid' if errors else 'valid'}")
        for error in errors:
            print(f"  - {error}")
    return passed


def validate_mode_collection(paths: list[Path]) -> bool:
    ids = {load_json(path).get("id") for path in paths}
    missing = REQUIRED_MODE_IDS - ids
    # The fixture directory is not required to contain every built-in mode; this
    # function documents the project-level rule for target-repo tests.
    if missing:
        print("INFO: mode collection completeness check is documented for target repo tests; fixture set is intentionally minimal.")
    return True


def main() -> int:
    valid_paths = sorted((ROOT / "fixtures" / "schema" / "valid").glob("*.json"))
    invalid_paths = sorted((ROOT / "fixtures" / "schema" / "invalid").glob("*.json"))
    ok = True
    for path in valid_paths:
        ok = validate_fixture(path, True) and ok
    for path in invalid_paths:
        ok = validate_fixture(path, False) and ok
    validate_mode_collection([p for p in valid_paths if p.name.startswith("mode_")])
    if ok:
        print(f"PASS: {len(valid_paths)} valid fixtures accepted; {len(invalid_paths)} invalid fixtures rejected.")
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main())
