# Agent Prompt — Phase 1B Fused Planner

You are implementing Phase 1B only: fused family planner behavior.

Prerequisites:

- Phase 1A mode/family metadata exists.
- Tests can run or known blockers are documented.

Read:

- docs/19_IMPLEMENTATION_TASKS.md: P1-E2-T1, P1-E2-T2, P1-E2-T3
- docs/20_TECHNICAL_CONTRACTS.md section 3
- docs/22_VALIDATION_MATRIX.md Phase 1B

Task:

1. Add planner tests first for:
   - non-fused family
   - fused non-selective exact
   - fused non-selective partial
   - fused selective subset
   - unsupported capability
   - raw advanced model path
2. Implement fused group planning.
3. Add diagnostics.
4. Expose planner preview to CLI and GUI advanced/debug output if practical.
5. Preserve ordinary stage behavior.

Do not:

- change UI layout broadly
- remove raw model paths
- implement Temporal Forge internals

Return PR notes using templates/PR_TEMPLATE.md.
