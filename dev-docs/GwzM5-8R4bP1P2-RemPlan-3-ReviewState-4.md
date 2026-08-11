# R4b-P P1/P2 remediation-plan final state confirmation 4

Date: 2026-08-12

Reviewed:

- current `GwzM5-8R4bP1P2-RemPlan-3.md`;
- `GwzM5-8R4bP1P2-RemPlan-3-ReviewState-3.md`; and
- the frozen service observation/attempt and required-parent durability
  contracts cited by that review.

This was a read-only, narrow confirmation review of `ReviewState-3` P2-1. I
did not modify the remediation plan or production/test code.

## Verdict

**GO for implementation.**

`ReviewState-3` P2-1 is fully closed. I found no P0, P1, or P2 contradiction
introduced by the correction.

## Closure audit

| Required correction | Result | Evidence in the current plan |
| --- | --- | --- |
| Ordinary physical `none + Ambiguous` | **Closed** | Section 3.6 now requires zero executions and no outcome, followed only by the position's representable recovery/no-rewrite rule. |
| Ordinary physical `none + operational read error` | **Closed** | Section 3.6 now retains the owner, issues no successor, and asserts zero executions. |
| Complete causal-parent relation | **Closed** | A separate causal-parent table explicitly crosses five attempt classes with `Before`, `AfterNeedsDurability`, real `Ambiguous`, and operational error. It pins no-attempt execution, matching `Success`/`Failed`, stale/mismatched, and consumed-second behavior. |
| Required-parent causal barrier | **Closed** | `none + AfterNeedsDurability` executes the bound idempotent barrier exactly once and reobserves; only matching `Success + AfterNeedsDurability` may advance. Matching failure retains the owner without retry or successor. |
| No synthetic causal `After` | **Closed** | Ordinary `After` has been removed from `CausalParentTransition`. Optional/already-established parents remain separate `ProofOnly` rows. |
| Exact legal cardinalities and assertions | **Closed** | Section 3.6 declares exactly 20 legal attempt/fact cells per ordinary physical row and 20 per causal-parent row. R4 step 4 pins both cardinalities and the distinct fact alphabets; step 5 pins call counts, exact successor/retained bytes, identities, journal/recovery disposition, and exhaustion. Exit criterion 8 repeats both 20-cell gates and requires real observers through terminal exhaustion. |

## Contract check

The corrected causal relation is consistent with the frozen durability rule:
the required empty final parent remains `AfterNeedsDurability`, an unattempted
pending barrier executes once, a matching success plus fresh causal proof may
advance, and diagnostics do not substitute for observation. Ambiguity and
observer errors cannot fabricate completion, while stale, mismatched, and
consumed-second attempts cannot progress or execute.

The ordinary physical additions likewise preserve executor non-authority:
unattempted ambiguity cannot mutate, and an observer failure cannot be treated
as either `Before` or `After`.

## Exit decision

The state-machine/C7 interface checkpoint is accepted for implementation. No
wire/model amendment or further state-plan correction is required by this
focused review.

## Checks performed

- read-only comparison of the corrected section 3.6 row alphabets and both
  relation tables with `ReviewState-3`;
- read-only comparison of R4 steps 3-5 and exit criterion 8 with the declared
  20-cell relations; and
- no tests were run because this is an interface-plan checkpoint rather than
  settled implementation.
