# Tests

## Schema fixture validation

Run from the archive root:

```bash
python tests/validate_schemas.py
```

The validator checks both JSON Schema structure and semantic invariants that standard JSON Schema cannot express cleanly, including:

- family `safe_defaults` subset of `capabilities`,
- preview `end_seconds > start_seconds`,
- cached preview manifests requiring enhanced frame paths,
- ordinary modes requiring deterministic default family/profile keys.

In the target AMD-VE repository, add an equivalent CI job after Phase 0 maps the real test framework.
