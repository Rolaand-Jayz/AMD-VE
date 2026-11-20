# Release Gate Checklist

## Release target

- Phase/milestone:
- Commit:
- Date:

## Build/test

- [ ] Clean build passes
- [ ] Unit tests pass
- [ ] Integration/smoke tests pass
- [ ] GUI launch smoke passes
- [ ] CLI dry-run/profile smoke passes
- [ ] Package smoke passes or waiver recorded

## Product behavior

- [ ] User-facing behavior documented
- [ ] CLI/GUI parity verified
- [ ] Advanced/raw model path preserved
- [ ] Error states visible
- [ ] Long jobs show progress/cancel where applicable

## Data/cache/profile

- [ ] Schema versions correct
- [ ] Migration behavior documented
- [ ] Cache invalidation tested
- [ ] Old profiles/manifests handled or migration documented

## Risk/IP

- [ ] Risk register reviewed
- [ ] No leaked/redacted source
- [ ] Licensing/IP notes updated
- [ ] Claims match evidence

## Waivers

| Check | Waiver reason | Owner |
|---|---|---|
| | | |

## License/provenance registries

- [ ] Dependency license registry complete or waived
- [ ] Model license registry complete or waived
- [ ] Dataset/benchmark media provenance complete or waived
- [ ] SDK usage review complete or waived
- [ ] Waivers include owner, date, reason, and follow-up

## Schema/contract validation

- [ ] `python tests/validate_schemas.py` or target-repo equivalent passes
- [ ] Schema fixtures updated for changed contracts
- [ ] Semantic validator updated for cross-field invariants
