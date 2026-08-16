# Faster-progress operator instruction — merge-lane implementor

Date: 2026-08-16

Status: **operator instruction to the current merge-lane implementor.**
Passing this file is the L1-28 decision. Quote it into
`CurrentProgramCheckpoint.md` before you change any other controlling
text. Do not wait for a second conversation to “confirm.”

Audience: the Claude (F5) merge/catalog implementor now executing
`GwzM5-8R2D-Plan.md` Phase 0.

This is not a new product design and not a request to stop R2-D. It
changes what A1 waits for, what you may add in front of A1, and how
R2-D is reviewed. Quality rules listed in §5 do not change.

## 1. Why you are getting this

`gwz merge` on the released v0.10.5 line already does start, status,
continue, abort, preserve, conflict prediction, `--ff-only`, and custom
messages. The open program is the M5–M8 v1 writer and the checked-artifact
conversion behind it.

The 15 August process plan (`GwzProcessOptimization.md`) speeds review
cycles. It does not shorten the remaining gate chain. Completing A1
faster is a scope decision, then enforcement of caps already adopted.

Do not spend this session re-deriving that diagnosis. Execute §3.

## 2. The decision (quote this block)

**Thin A1.** A1 enables the v1 writer and `--no-ff` on the accepted R4b
lifecycle after R2-D settles and M5b’s already-bound proofs (T-6 +
clean-tree re-cut) are green. R2-E, R2-F, R3, R4, R5, and R6 are **not**
A1 gates. They remain real work; they are hardening and consumer
conversion, scheduled after A1 or in parallel, not in front of it.

Residual you are ordered to accept and name: merge store, archive,
stash bundles, and related consumers keep their current call graphs
through A1; `recover_or_create` stays without a production caller;
legacy writers may still mutate inside `.gwz/checked-artifacts` until
R2-E. That residual is already the R2-D defer-out (`GwzM5-8R2D-Plan.md`
§5 items 1–5). Coupling those items to A1 was a later gate-chain
sentence, not a physical dependency of writing v1 records.

**No further pre-A1 I2 contract trains.** A1 ships on the I2 contracts
already frozen, plus amendments already accepted (including the durable
cursor). The drafted operator-escape amendment
(`GwzM5-8OperatorEscapeAmendment.md`, ~1,760 lines) and any further
panic-invariant or escape-wire freeze are **not A1 gates**. Keep the
v0 wedge runbook owed on its own (Q9). Do not launch mandated dual
review of a third I2 train as a prerequisite to A1.

**R2-D review caps (already adopted; now binding on this package).**
Dual peer-blind review only at (a) the Phase 0 interface freeze, (b)
the Phase 1 admission kernel if you still treat Idle↔Preparing as a
durable-transition kernel, and (c) the Phase 5 settled-tree gate.
That is three duals maximum, not four. Interior steps are single-axis
with automatic escalation on P0/P1/P2. Two-round remediation cap:
a third new architectural root cause on the same object is
redesign-or-accept, not RemPlan-5. P3s file and continue; they do not
become packages and they do not enlarge R2-D. While Phase 0 is in
review, start Phase 1 failing tests (`GwzProcessOptimization.md` §4.4).

**Track P before the freeze is reviewed.** Before
`GwzM5-8R2DInterfaceFreeze.md` goes to dual review, spike the admission
publish/retire path on macOS and Windows against the already-sealed
`publish_verified_no_replace` family. Do not freeze the four
`managed_operation_unavailable` defaults into required methods until
each new physical edge names an admitted primitive per platform
(`GwzProcessOptimization.md` §3.1). Policy is already in force; apply
it to this freeze.

**Lane split.** You remain the sole writer on merge/catalog/R2-D
(L1-06). Pre-A1 docs (escape amendment review, panic-conversion
packages, runbook) and M5b proof/doc work are a second lane. Do not
pick them up. Record the split in the checkpoint. If a second agent
is not yet handed off, leave those items listed as “blocked on
operator handoff” and continue R2-D.

## 3. What to do, in order

Work from `/Users/owebeeone/limbo/gwz-dev`. You may edit documentation
and then continue R2-D implementation. Do not start R2-E, R2-F, R3–R6,
A1 implementation, or a new I2 freeze.

### Step A — record the decision

Update `CurrentProgramCheckpoint.md`:

1. Add an “Operator decision 2026-08-16 — thin A1” section that quotes
   §2 of this file and names this file as the authority.
2. Replace the live sentence that R4b-G / A1 wait on RemPlan-4’s full
   R2–R6 chain (`CurrentProgramCheckpoint.md` “SCOPE CORRECTION
   2026-08-16” and the resume-order paragraph that sequences R3–R6
   before R4b-G) with: R2-D settle is the last catalog gate on the A1
   path; R2-E through R6 are post-A1 / parallel hardening.
3. Record R2-D review tiers as the three duals in §2, not the four
   listed in `GwzM5-8R2D-Plan.md` §6.
4. Move operator-escape amendment dual review, further I2 trains, and
   panic-conversion packages off the A1 gate list. Leave them as
   tracked non-gating work on the second lane.
5. Keep the file the single current-state authority (rulebook §7.3).
   Do not paste a new status novel into `GwzM5-8Refactor.md`.

### Step B — bounded gate-chain amendment

Write one short amendment, not a new program document:

```text
dev-docs/GwzM5-8ThinA1Amendment.md
```

It must satisfy L1-08: name the superseded clauses, state the new
controlling rule, say why, list affected documents, and take the same
class of review as a process/scope amendment (mandated dual — this is
an amendment, `GwzProcessOptimization.md` §4.2). Keep it small. The
superseded clauses are at least:

- `GwzM5-8R4bP1P2-RemPlan-4.md` §4 / the stop that R4b-G begins only
  after R2–R6;
- `CurrentProgramCheckpoint.md` SCOPE CORRECTION 2026-08-16 (the
  sentence you are replacing in Step A);
- any sentence in `GwzM5-8R2D-Plan.md` that treats R2-D as merely the
  next link in an A1-blocking R2–R6 chain rather than the last catalog
  gate on the A1 path.

Do not reopen I2 wire, do not add types, do not fold C3/R2-E/escape
into this amendment. After the dual review, put a one-line
supersession banner on RemPlan-4 pointing at the amendment. Do not
rewrite RemPlan-4.

If you are mid-Phase 0 freeze drafting, you may file the amendment and
its review prompts, then continue the Track-P spike (Step C) while
those reviews run.

### Step C — Track-P spike, then the Phase 0 freeze

1. Spike admission publish/retire (and any other R2-D physical edge
   the freeze will newly require) on macOS and Windows against the
   sealed primitive family already proven in C2. Record the primitive
   name per edge per platform in the freeze memo. If an edge needs a
   new primitive, stop the freeze and spike that primitive first.
2. Then finish `GwzM5-8R2DInterfaceFreeze.md` + failing-test
   scaffolding under **one** mandated dual review.
3. Record the freeze tiers in the checkpoint when the freeze lands,
   not mid-lane.

### Step D — continue R2-D under the caps

Execute the adopted R2-D phases. C3 remains Phase 1. Do not enlarge
R2-D with P3 residuals, exact-evidence OPEN DECISIONS, MAX_PATH
relocation, operator escape, or panic conversion.

Pipeline: Phase 1 failing tests start as soon as Phase 0 is in review.
A third architectural root cause on the freeze, the admission kernel,
or the settled tree stops the lane for redesign; it does not start
another rem-plan series.

R2-D’s own stop clauses still bind (RemPlan-4 :1082–1085;
ConsumerCheckpoint §14): no successful converted path may call the
Windows no-op parent sync, do an unbounded protocol read, allocate a
fresh retry scratch name, lack pre-reserved cleanup capacity, or
mutate the private/managed tree before capability and collision
acceptance. Thin A1 does not waive those.

## 4. What you must not do

- Do not treat this file as permission to skip P0/P1/P2, skip executed
  fault evidence, skip handle-bound publication, or compile the v1
  writer into a release build before A1.
- Do not start R2-E consumer conversion, production `recover_or_create`,
  legacy-path deletion, or a new Windows/Linux fault-evidence campaign
  as an A1 prerequisite.
- Do not launch dual review of `GwzM5-8OperatorEscapeAmendment.md` as
  an A1 gate. If that review is already in flight, record it as
  non-gating and do not let it block Phase 0.
- Do not raise an R2-D ceiling unless the same sentence names what was
  descoped (`GwzProcessOptimization.md` §2.1).
- Do not take the second lane’s files. Do not wait idle for a second
  agent; continue R2-D.
- Do not create tags, push, or otherwise exceed the git operation you
  are explicitly given.

## 5. What does not change

These remain in force and are not waived:

- `AgentProcessRules.md` L1-03, L1-13, L1-14, L1-15, L1-16, L1-17,
  L1-19, L1-26, L1-32
- wire-format freeze-first and the retained-reader harness (L2-04)
- sealed source-associated publication (`GwzM5-8R2CCatalogBootstrapAmendment.md`
  §4.1)
- v1 remains `cfg(test)` until the A1 activation review
- D2 foreign-filter policy stays release-gated, not A1-gated
- M5b stays semantics-installation-only under its zero-production-line
  ceiling; its settle still requires T-6 and a clean-tree re-cut

## 6. Done for this instruction

This instruction is discharged when all of the following are true:

1. `CurrentProgramCheckpoint.md` quotes §2 and states that R2-D settle
   is the last catalog gate on the A1 path.
2. `GwzM5-8ThinA1Amendment.md` exists, names the superseded gate-chain
   clauses, and has been sent to mandated dual review (or the reviews
   are filed).
3. The R2-D Phase 0 freeze memo names a Track-P primitive per new
   physical edge per platform, or records a stop because one is
   missing.
4. You are back on R2-D execution (freeze review in flight and/or
   Phase 1 failing tests started).
5. Operator-escape, further I2 trains, and panic conversion are listed
   as second-lane, not-A1-gating work.

Reply to the operator with: the checkpoint commit or working-tree
pointer, the amendment path, the freeze/spike status, and the single
next R2-D action. Do not propose additional pre-A1 trains in that
reply.
