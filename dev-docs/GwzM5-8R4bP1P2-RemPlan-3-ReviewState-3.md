# R4b-P P1/P2 remediation-plan final state confirmation

Date: 2026-08-12

Reviewed:

- revised `GwzM5-8R4bP1P2-RemPlan-3.md`;
- `GwzM5-8R4bP1P2-RemPlan-3-ReviewState-2.md`; and
- the frozen service-loop, transition, reverse-lifecycle, and interface-
  amendment attempt/observation contracts.

This was a read-only, narrow confirmation review of the prior P2-1. I did not
modify the remediation plan or production/test code.

## Verdict

**NO-GO for implementation.**

The revision closes the central omission from `ReviewState-2`: ordinary
physical rows now have an explicit executor-attempt dimension, the important
`Failed + After` and `Success + Before` non-authority outcomes are pinned, R4
generates the cross, and exit criterion 8 requires it. One P2 completeness gap
remains in the declared relation itself. There is no P0 or P1 finding and no
separate P3 finding.

## Finding

### [P2-1] The claimed complete relation still omits legal no-attempt cells and admits an impossible causal `After`

**Where:** revised RemPlan-3 section 3.6, especially the row alphabets and
ordinary physical relation table; R4 steps 3-5; exit criterion 8; frozen
service observation/attempt matrix; and interface amendment 2's required
empty-parent durability rule.

**Evidence.** The ordinary physical table defines `none + Before` and
`none + After`, but it does not define either of these legal cells:

- `none + real Ambiguous`; or
- `none + operational read error`.

It defines ambiguity and read-error handling only when a matching attempt is
present. The frozen service matrix requires no-attempt ambiguity to enter the
position's representable recovery form (or its required halted predecessor),
and an operational observer error must retain the owner without execution or
successor publication.

The causal-parent prose says the same cross applies and that only matching
`Success + AfterNeedsDurability` satisfies the barrier, but it never pins the
required `none + AfterNeedsDurability -> execute the bound idempotent barrier
exactly once, then reobserve` cell. That cell is explicitly required by the
accepted interface amendment. Its no-attempt ambiguity and operational-error
outcomes are likewise left implicit.

Finally, `CausalParentTransition` declares ordinary `After` as a legal fresh
observation. Under the frozen classifier every required empty final parent is
always `AfterNeedsDurability`; source-equals-goal and optional/already-
established parents are separately classified as `ProofOnly`. There is
therefore no legal ordinary `After` fact for a causal-parent row. Keeping it in
the alphabet conflicts with R4's instruction to generate only legal forms and
can create synthetic coverage rather than production evidence.

**Impact.** R4 can satisfy its stated cross and cardinality gates without
proving what happens when an unattempted physical row is already ambiguous or
cannot be observed, and without proving that an unattempted required-parent
durability obligation executes rather than rejects or stalls. It can also
count an unrepresentable causal `After` row. That means the executor-
diagnostic correction is not yet coverage-closed despite exit criterion 8's
claim.

**Required correction.** Make the row relations literal and exhaustive:

1. Add `none + real Ambiguous` and `none + operational read error` outcomes
   for every ordinary physical row, including exact recovery/no-rewrite,
   retained-owner, successor-byte, and zero-call assertions.
2. Give causal-parent rows their own explicit table. At minimum pin
   `none + Before`, `none + AfterNeedsDurability`, no-attempt ambiguity/read
   error, matching `Success`/`Failed` over every legal fresh fact, and
   stale/mismatched/consumed-second rejection. Only matching
   `Success + AfterNeedsDurability` may advance; with no attempt the pending
   durability fact executes its bound barrier once.
3. Remove ordinary `After` from `CausalParentTransition`, unless the plan can
   identify a concrete production classifier that legally emits it without
   contradicting the frozen rule. Keep optional/already-established parent
   cases in `ProofOnly`.
4. Make the independent R4 cardinalities and assertions count these exact
   legal cells, and keep exit criterion 8 tied to that explicit relation.

## Exit decision

The prior executor non-authority defect is structurally addressed, but P2-1
is not fully closed until the missing no-attempt outcomes and the causal-parent
alphabet are corrected. No wire/model or broader architectural amendment is
indicated. After that small declarative correction, this state checkpoint is
GO for implementation.

## Checks performed

- read-only comparison of section 3.6, R4, and exit criterion 8;
- read-only comparison with the frozen observation/attempt and required-parent
  durability rules; and
- no tests were run because this is an interface-plan checkpoint, not settled
  implementation.
