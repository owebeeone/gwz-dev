# GWZ M5b — v1 no-ff interface design (freeze-first half)

Date: 2026-08-16. Revised: 2026-08-16 (M5b-IF dual-review remediation
round 1, document-only: Code F-1..F-8 (`GwzM5-8M5bNoFf-ReviewCode.md`,
NO-GO, 1×P1 + 1×P2 + 6×P3) and State P2-1/P2-2/P3-1..P3-4
(`GwzM5-8M5bNoFf-ReviewState.md`, NO-GO, doc-only), plus the lane-owner
adjudications: F-1 disposition = the parallel "v0 forged-action resume
gate" package (§3.5a); Q6 reconciled tier rule (§8.1); Q8 ratified with no
ceiling exception (§8.3)).

Status: **DRAFT — pending focused re-review.** Round-1 verdicts: State
NO-GO (2×P2, both document corrections; "no semantic, wire, boundary, or
sequencing defect was found"), Code NO-GO (F-1 P1 contract-vs-production
divergence + F-2 P2 proof-plan hole; "the machinery claims all held under
adversarial checking"). This revision applies every finding from both
reports. This is the interface-freeze design for
the "M5b — v1 no-ff implementation, activation disabled" milestone
(`GwzM5-8Refactor.md:2275-2289`). It is a design document: it freezes
meaning, wire posture, determinism semantics, reconciliation authority,
activation boundaries, proof obligations, and the freeze/review sequence. It
deliberately contains no implementation phasing beyond the interface-freeze
plan in §8. Nothing in this document is "frozen" or "accepted" until the
mandated dual review passes (L1-07, `AgentProcessRules.md`).

Authorities this design binds itself to, in precedence order:

- `GwzM5-8Refactor.md` — behavioral authority; M5b obligations at
  §"M5b" (2275-2289), §6 (293-297, 370-376), §16.1 (1852-1872),
  §22.4 item 4 (3181-3182).
- The four I2 contracts **as amended**: `GwzM5-8I2RecordContract.md`
  (amended 2026-08-09, 2026-08-11), `GwzM5-8I2ActionJournalContract.md`
  (amended 2026-08-09, 2026-08-11, 2026-08-16),
  `GwzM5-8I2CompatibilityContract.md` (amended 2026-08-10 via
  Amendment-1 §4), `GwzM5-8I2ProtocolContract.md`.
- `GwzM5-8ExactEvidencePlatformAmendment.md` (accepted 2026-08-16) where its
  clauses touch the observation/checkout edges M5b reconciles through.
- Process: `AgentProcessRules.md` + `GwzProcessOptimization.md`
  (tier rules §4.2-§4.4); position: `CurrentProgramCheckpoint.md`.

## 1. Verbatim obligations and reading of scope

`GwzM5-8Refactor.md:2277-2289` obliges M5b to:

1. Add v1 `NoFf` integration intent **without adding it to writable v0**.
2. For every otherwise fast-forwardable participant, **freeze the
   deterministic two-parent commit message/tree/signatures and typed
   prepared action before its mutation can begin**.
3. **Reconcile continue/abort/preservation from that exact v1 action** rather
   than re-deriving merge mode from live analysis.
4. **Prove** normal/divergent/up-to-date behavior remains exact and prove the
   fast-forwardable result is a deterministic two-parent commit.
5. Keep **CLI/protocol start activation and the v1 production writer
   unreachable until A1**.

Plus the framing constraints:

- "M5b may merge behind the same hard publication boundary as R3/R4b. It is
  not a v0 release." (`GwzM5-8Refactor.md:2288-2289`)
- "R2a should first group only the existing M4/custom-message intent; M5b
  adds no-ff intent only to v1." (`GwzM5-8Refactor.md:293-295`) and "R2a …
  does not make `NoFf` writable in v0. M5b adds executable mode to v1.
  Neither serializes placeholder checkout, branch-ownership, or
  optional-participation values." (`GwzM5-8Refactor.md:374-376`)
- "M5b then installs deterministic no-ff while start remains unreachable. A1
  alone activates the v1 writer floor, eligible v0 migration, and public
  no-ff surface in one reviewed change."
  (`GwzM5-8I2CompatibilityContract.md:283-286`)
- A1 gate: "M5b's deterministic no-ff prepared action is installed but
  unreachable until the activation change." (`GwzM5-8Refactor.md:3181-3182`)

**Reading.** M5b is a *semantics-installation* milestone inside the already
test-gated v1 lifecycle, not a wire milestone and not a protocol milestone.
The I2 contracts already froze every byte the no-ff feature persists (§3
proves this field by field). A substantial part of the executable machinery
already landed — reviewed and accepted — with R4b-X, which "executes checked
fast-forward and deterministic two-parent no-ff actions, and reconciles
completed actions after restart without re-execution"
(`GwzM5-8R4bTransitionDesign.md:1209-1219`). This design therefore does three
things: (a) states exactly which frozen wire rows M5b makes executable and
which stay dormant for A1; (b) freezes the determinism and reconciliation
semantics against the current tree with citations, so review can check the
claim rather than the intention; (c) enumerates the *delta* M5b still owes —
mostly closed-matrix proofs, reverse-path no-ff coverage, and unreachability
tripwires — as named suites.

## 2. Current-tree inventory: what exists, what M5b owes

Every claim cites the tree at the authored basis, gwz-core
`f36d20dda7af631f6d9a5657affd481022bcebed` (run-11 train). Both round-1
reviews re-derived every citation at gwz-core HEAD
`43c37bcff338daf95f053eedbc7467e6c9368eff`; the delta `f36d20d → 43c37bc`
is exactly one file (`.github/workflows/platform-matrix.yml`, no `src/`
change), so the two bases are byte-identical for every `src/` citation
below (Code review §0; F-8). Tree-condition note, stated honestly per the
State review's tuple: at review time the gwz-core **working tree** carried
uncommitted modifications from an in-flight lane (including
`git/gitbackend/merge_recovery.rs` and
`v1_lifecycle/authority/observe/reverse/rollback.rs`); one cited file
(`merge_recovery.rs`, cited at :424-433 for §3.4/§4.4/§4.6) does carry
uncommitted hunks, but they fall at :6/:149+/:224+ inside
`abort_merge`/`set_branch_target_checked` and overlap no cited range
(round-2 Code N-2); all citations here are against **committed** blobs,
and the M5b-IF freeze tuple must be re-cut against a clean committed
tree at acceptance (State P3-2).

### 2.1 Already present and accepted (M5b consumes, must not re-open)

| Fact | Where |
| --- | --- |
| `MergeExecutionMode::{Normal, FfOnly, NoFf}` is one shared closed enum serialized as `mode:` with `no_ff` spelling, omitted when normal, at the same YAML path in v0 and v1 | `gwz-core/src/workspace_ops/merge/model/lifecycle.rs:5-28`; `model/v0.rs:44-45`; `model/v1/record.rs:29-30` |
| v0 start rejects `--no-ff` before record creation (`MergePhaseUnsupported`, "no_ff requires the v1 record lifecycle and is not yet activated") | `gwz-core/src/workspace_ops/merge/validate.rs:12-14`, pinned by test `validate.rs:201-225` |
| Open v0 `mode: no_ff` is rejected before adaptation/migration (`UnsupportedLegacyMode`) — **on the test-gated R3 adaptation path only**: the gate at `adapter.rs:88-96` sits inside `#[cfg(test)] mod open_v0` (`record_wire/mod.rs:13-14`), `UnsupportedLegacyMode` is raised nowhere else in the crate, and the production resume path has no mode gate (Code F-1; §3.5a) | `gwz-core/src/workspace_ops/merge/record_wire/open_v0/adapter.rs:88-96`; contract promise at `GwzM5-8I2CompatibilityContract.md:145,163-165` |
| The durable typed action vocabulary: `PendingMergeAction` (kind, target_branch, before_commit, source_commit, commit_message, expected_result, commit_spec) with `PendingCommitSpec { tree_oid, author, committer }` and `PendingGitSignature { name, email, time_seconds, timezone_offset_minutes }` — all with unknown-field `extensions` | `gwz-core/src/workspace_ops/merge/model/v0.rs:98-148` |
| The frozen v1 mode/action matrix, including the no-ff rows ("no-ff, up to date → verify-up-to-date"; "no-ff, fast-forwardable or clean true merge → true-merge / commit / exact two-parent commit spec") and the two-parent sentence ("the two parents implied in order by `before_commit` and `source_commit` … do not reclassify it as a fast-forward") | `GwzM5-8I2RecordContract.md:324-344` (§7) |
| Executable v1 validator of that matrix: `mode: no_ff` forbids a durable `FastForward` kind; exact shape/state legality; commit-spec canonicality incl. signature bounds | `gwz-core/src/workspace_ops/merge/model/v1/validate/action.rs:41-116`, tests `action_tests.rs:44-104` |
| Mode-aware backend preparation: `GitPreparedMergeMode::{AllowFastForward, ForceMergeCommit}`; `ForceMergeCommit` over a fast-forwardable analysis prepares a `GitPreparedMerge::Commit` whose tree is the **source commit's tree** and whose signatures are captured at preparation | `gwz-core/src/git/gitbackend/contract.rs:140-147`; `gwz-core/src/git/gitbackend/merge_prepared.rs:140-195` (ff arm 168-180) |
| Read-only revalidation accepts a frozen Commit spec over a fast-forwardable analysis **only** when the frozen tree equals the source tree; every class change is a typed mismatch | `gwz-core/src/git/gitbackend/merge_support.rs:98-152` (arm 101-112) |
| Execution creates the two-parent commit from the frozen spec and self-verifies parents/message-bytes/tree/author/committer before publishing the ref; a frozen Commit spec can never execute as a plain fast-forward (that arm requires `prepared == FastForward`) | `gwz-core/src/git/gitbackend/merge_prepared.rs:296-336` (self-verify 314-327; ff arm guard 251) |
| Restart adoption of an already-created commit compares two parents, message bytes, tree, and both signatures against the frozen spec | `gwz-core/src/git/gitbackend/merge_prepared.rs:39-69`; consumed via `status/pending.rs:117-152` |
| The v1 lifecycle prepares mode-aware actions from `record.mode` (`prepared_mode`: `NoFf → ForceMergeCommit`), maps ff-able+forced to effective true-merge, persists the prepared action through an issued proof before execution, and executes only the decoded frozen action | `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/forward.rs:118-236`; `v1_lifecycle/forward/execute.rs:8-90` |
| `FfOnly` + true-merge preparation is rejected (`validate_mode`) | `forward.rs:220-228` |
| Reconciliation from the persisted action (never from live re-analysis): NotStarted/Completed/ExpectedConflict/Ambiguous classification; completion maps durable kind `TrueMerge → Merged` | `gwz-core/src/workspace_ops/merge/status/pending.rs:26-237` (Ambiguous fall-through to :237); `forward.rs:274-371` (kind mapping :357-362) |
| Abandonment machinery: a pending forward action retires without an outcome only through the bound exact `NotStarted` proof consumed by abort/preserve entry (`AbandonNotStartedAndBeginRollback/Preservation`), illegal after preservation | `GwzM5-8I2RecordContract.md:346-363` (amended 2026-08-09); `v1_lifecycle/transition/mod.rs:153-158`; `transition/reverse_entry.rs:82-88` |
| Two end-to-end no-ff tests exist: two-parent creation and exact restart adoption | `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/forward.rs:81-100,103-139` |
| Unreachability idiom: whole-module `#[cfg(test)] mod v1_lifecycle;` + compile sentinel; module-local `#![allow(dead_code, reason = "v1 lifecycle remains test-reachable until A1 activates production dispatch")]`; production decoder pins `PRODUCTION_R3` and treats a v1 dispatch as unreachable | `gwz-core/src/workspace_ops/merge/mod.rs:23-26`; `v1_lifecycle/mod.rs:1-5`; `record_wire/decode.rs:82-86` |
| v1 wire acceptance of `mode: no_ff` (the §1 record-contract sentence "v1 validation of `mode: no_ff`") is implemented — no v1 validator rejects the mode itself, only the matrix shapes | `model/v1/validate/action.rs:62-68`; absence of any other `NoFf` check in `model/v1/validate/` (grep-verified this tree) |

### 2.2 The M5b delta (what this freeze authorizes building)

D1. **Closed-matrix executable proofs** for mode × analysis × restart ×
abandonment — the record contract's exit-test bullet "the complete
normal/ff-only/no-ff action matrix and two-parent restart reconciliation"
(`GwzM5-8I2RecordContract.md:420-421`) has named suites only partially
implemented (two no-ff forward tests exist; no no-ff up-to-date,
conflict-row, abandonment, rollback, preservation, or determinism-of-bytes
suites). §7 names them.

D2. **Reverse-path no-ff coverage**: abandonment of a pending two-parent
action via abort and preserve-abort; rollback (`ResetIntegrated`) and
preservation (backup-ref/stash anchor) of a *completed* two-parent
participant. No journal wire change (§3.3); only proofs and, if review
finds gaps, observation-glue inside the test-gated module.

D3. **Determinism statement + byte-level proof** (§4, §7 suite P-DET):
the commit OID as a pure function of the frozen action.

D4. **Unreachability tripwires** (§6): the boundary is already structural;
M5b adds the no-ff-specific negative assertions so A1's activation diff has
an exact checklist to invert.

D5. **A document-only additive amendment to the I2 record contract** (§3.4)
pinning two clarifications (timestamp freeze wording; no-object-signing
wording) that the wire already implies but the text does not state.

D6. **Ledger row**: `GwzM5-8ChangeBudget.md` has frozen ceilings only through
R4b-P0.2 (grep of its `###` sections, this tree); an M5b package row with
numeric LOC/file ceilings and declared wire/protocol deltas (both zero) must
be added at freeze time per the R0 acceptance pattern
(`GwzM5-8Refactor.md:3080-3081`). Proposed numbers in §8.3.

Explicitly **not** in M5b (deferred, with owners): the version-selection
pure function `max(active_writer_floor, highest_requested_semantic_version)`
(`GwzM5-8I2CompatibilityContract.md:61-77`) is implemented nowhere in this
tree (grep: no `active_writer_floor` symbol) — it is A1's writer-floor
machinery, not no-ff semantics (open question Q2); protocol/start surface
(A1); v0 migration of no-ff rows (never — `GwzM5-8I2CompatibilityContract.md:145`).

## 3. WIRE — v1 record/journal posture and the proposed amendment

### 3.1 The no-ff intent on the wire: zero new fields

M5b adds **no new field, no new enum variant, no new YAML path, no new
container** to the v1 record or journal. The complete durable surface of the
feature is already frozen by the amended I2 contracts:

- **Intent**: the operation-level `mode` field, `snake_case` `no_ff`, at the
  identical v0/v1 path, omitted when normal
  (`GwzM5-8I2RecordContract.md:32-38`; `model/v1/record.rs:29-30`). Per
  `GwzM5-8Refactor.md:293-295,374-376`, the "NoFf integration intent" v1
  gains is this existing field becoming *executable* under v1 validation —
  the per-participant `MergeIntent { mode, … }` sketch in Refactor §6 is an
  illustrative eventual shape, explicitly not a v1 wire item. The R2a intent
  grouping (`IntegrationIntent`,
  `gwz-core/src/workspace_ops/merge/integration.rs:17-52`) is untouched:
  mode enters preparation from `record.mode` (`forward.rs:188-189`), which
  keeps one semantic owner for the mode decision and zero wire churn.
- **Typed prepared action**: the existing `PendingMergeAction` at its v0
  path, in the frozen §7 no-ff rows: kind `true_merge`, expected result
  `commit`, plus `PendingCommitSpec` (`GwzM5-8I2RecordContract.md:318-344`;
  types `model/v0.rs:98-148`). Parents are **not** serialized separately:
  they are implied in order by the action's `before_commit` and
  `source_commit` (`GwzM5-8I2RecordContract.md:339`), which the participant
  row must equal (`integration.rs:175-191`; validator `action.rs:23-39`).
- **Signatures**: `PendingGitSignature` already carries
  `time_seconds`/`timezone_offset_minutes` on the wire (`model/v0.rs:131-139`)
  — the timestamp *is* part of the frozen action today.
- **Journal**: no no-ff-specific variant exists or is needed (§3.3).

### 3.2 What M5b freezes/installs vs what A1 activates

This enumeration is the activation boundary the A1 review will invert. It is
part of the frozen interface.

**M5b freezes/installs (production-unreachable on the v1 side — request,
v1 record, preparation, and v1 writer surfaces; the one production-reachable
consumption lane, the forged-v0 record lane of §3.5a, is closed by the
parallel "v0 forged-action resume gate" package, not asserted away):**

1. v1 records carrying `mode: no_ff` are valid, decodable, validator-clean,
   and executable through the test-gated v1 lifecycle
   (`model/v1/validate/action.rs:62-68`; `v1_lifecycle` module).
2. The deterministic two-parent prepared action for every otherwise
   fast-forwardable participant, persisted before mutation, per §4 semantics
   (`forward.rs:85-116` proof issuance; store write precedes physical
   execution in the transition kernel).
3. Reconciliation/abandonment/rollback/preservation consumption of that
   exact action per §5 (v1 lifecycle; the v0-side forged-record consumption
   lane is §3.5a's package and tripwire T-6, outside M5b).
4. The complete proof corpus of §7.
5. The two documentation clauses of §3.4.

**A1 alone activates (M5b must leave untouched):**

1. Request-level acceptance of `--no-ff` — deleting the
   `validate.rs:12-14` rejection (and its `runtime/dispatch.rs:115-120`
   precedence shim) is the A1 diff, not M5b's.
2. The v1 production writer/decoder floor: `PRODUCTION_R3` →
   an A1 registry installing v1 (`record_wire/decode.rs:82-86`), removal of
   the `unreachable!` there, and the `*_for_r3_tests` renames — all pinned to
   the A1 change by `GwzM5-8ProgressReviewF5.md` §9 item 5.
3. The version-selection pure function and creation matrix rows (`no-ff →
   v1`) (`GwzM5-8I2CompatibilityContract.md:61-77`), subject to Q2.
4. Eligible open-v0 migration dispatch (mode `no_ff` stays ineligible
   forever: `GwzM5-8I2CompatibilityContract.md:145`).
5. Public protocol surface: **none exists for no-ff at all** — `MergeMode`
   already has the `NoFf` request variant (rejected at validate), responses
   project no mode field (`gwz-core/src/workspace_ops/merge/response.rs`,
   grep this tree: no mode projection), and the I2 protocol contract
   allocates no no-ff discriminant beyond error 47 and the wave string
   `A1 (v1 integration/acceptance/no-ff)`
   (`GwzM5-8I2ProtocolContract.md:17,45-47`). Answer to the standing
   question: **NoFf needs no protocol-contract change now or at A1**; A1
   changes only request *acceptance*. This matches Refactor §294: the intent
   joins the first group's (v1's) vocabulary, not the protocol's.

### 3.3 Journal contract: explicit zero-delta statement

Reverse ownership is mode-blind by construction, and M5b keeps it that way:

- Rollback of a completed no-ff participant is the existing
  `PendingRollbackActionV1::Participant { action: ResetIntegrated,
  terminal_state: rolled_back }`; its observation anchors on the *recorded
  result commit* — which for no-ff is the two-parent commit — and the exact
  `before_commit` restore, exactly as frozen
  (`GwzM5-8I2ActionJournalContract.md:44-62, 222-231`).
- Preservation owner anchors are "the participant result"
  (`GwzM5-8I2ActionJournalContract.md:175-177`) — again the two-parent
  commit; `BackupRef`/`Stash`/`ResetAttachedRef` payloads are unchanged.
- Abandonment of a not-started no-ff action is the record-contract §7
  mechanism (2026-08-09 amendment), not a journal variant
  (`GwzM5-8I2RecordContract.md:354-363`).

Therefore M5b proposes **no amendment** to
`GwzM5-8I2ActionJournalContract.md`. Its §3/§4 observation edges apply to
no-ff participants without textual change; the platform interplay is §5.4.
Scope note (State P2-2): "zero delta" here means zero **no-ff** deltas
proposed by M5b — it does not freeze the journal contract's current text
against the separately adopted D3 durable-cursor amendment train, whose
non-collision argument and landing-order independence are recorded as
freeze positions in §8.2.

### 3.4 Proposed record-contract amendment M5b-W1 (document-only, additive)

To be applied as a dated banner + two clauses in
`GwzM5-8I2RecordContract.md` §7, in the style of the 2026-08-09 amendment
("no v1 wire shape change"):

> Amended <date> by `GwzM5-8M5bNoFfDesign.md` (M5b interface freeze): §7's
> "exact tree, author, committer" is clarified to include the exact
> signature **timestamps**: a commit action's author/committer freeze
> comprises name, email, epoch seconds, and timezone offset as serialized in
> `PendingGitSignature`; preparation captures them once, and execution,
> restart adoption, and reconciliation reuse the frozen values without
> re-stamping. Second clarification: neither integration-commit
> construction site (the two-parent execution path nor the conflict
> resolution path) attaches a cryptographic object signature — both are
> direct libgit2 `commit()` calls, which ignore `commit.gpgsign` — so
> "signatures" throughout §7 means the Git author/committer identity
> lines; the AD1 porcelain commit/tag fallbacks elsewhere in the backend
> deliberately honor signing config and are not integration paths. §7's
> restart adoption and completion matching are **field-wise** over the
> frozen fields (parents, message bytes, tree, both identity lines), not
> commit-OID/byte-wise; an externally created twin commit that matches
> every frozen field may be adopted even if it carries additional headers.
> The amendment changes no v1 wire shape, no validator outcome, and no
> journal semantics.

Rationale: all three facts are already true of the tree (§4.2, §4.4, §4.6)
and already implied by the wire type and matchers, but the contract
sentence names only "author, committer", and the Refactor's obligation
sentence ("message/tree/signatures", `GwzM5-8Refactor.md:2278-2280`) is
only dischargeable byte-for-byte if timestamps are inside the freeze.
Pinning the words prevents a future reviewer reading "author" as
identity-sans-time, "signatures" as GPG, or "deterministic" as an
OID-equality claim about adopted foreign twins (Code F-4/F-6; State P3-1).
Additive-only: no shape, spelling, or legality cell changes; every
existing fixture remains byte-valid.

### 3.5 Compatibility argument

Per the record contract's unknown-field rules and the compatibility
contract:

1. **v0 never writes it.** No v0 writer can serialize `mode: no_ff`:
   the request is rejected before record creation (`validate.rs:12-14`;
   M5a acceptance criterion `GwzM5-8Refactor.md:3096-3097`), the M5a creation
   row is "unsupported" (`GwzM5-8I2CompatibilityContract.md:69-74`), and R2a
   explicitly did not make `NoFf` writable in v0
   (`GwzM5-8Refactor.md:374-375`). For a hand-forged open v0 `no_ff` row,
   the contracts promise `UnsupportedLegacyMode` before resume/adaptation/
   migration/mutation (`GwzM5-8I2CompatibilityContract.md:163-165`;
   `GwzM5-8I2RecordContract.md:343-344`; fixture obligation
   `GwzM5-8Refactor.md:2081-2082`); on the R3 adaptation path the mode gate
   fires at `adapter.rs:88` **before** structural validation at
   `adapter.rs:97` (envelope-validated only — Code F-5 corrects this
   design's earlier "structurally validated, then rejected" ordering).
   **That gate is today `cfg(test)`-only, and the production resume path
   does not discharge the promise — see §3.5a for the truth and the
   adjudicated closure.** A terminal archived v0 `no_ff` decodes read-only
   from durable evidence (`GwzM5-8I2CompatibilityContract.md:96-98`).
2. **No retained reader can observe a v1 no-ff row as anything but a closed
   envelope rejection.** The no-ff action rows exist only inside
   `gwz.merge-operation/v1`/`1` bodies; v0.10.2 (the pinned durable-v0
   baseline) supports exactly the v0 pair and must reject v1 with the frozen
   envelope behavior (`GwzM5-8I2CompatibilityContract.md:266-273`). This is
   precisely the §16.1 property "v1 persists enough typed intent/action
   evidence that a v0 reader cannot silently reinterpret it"
   (`GwzM5-8Refactor.md:1866-1867`) — discharged at the envelope, before any
   body row is reachable.
3. **Unknown-field survival is already specified for every container M5b
   touches**: `pending_action`/`commit_spec` survive while the same action is
   pending and retire on exact reconciliation or checked abandonment
   (`GwzM5-8I2RecordContract.md:375-388`, row 382); the participant row
   survives for the record lifetime; M5b introduces no container, so no new
   retirement rule and no new collision beyond the five already-frozen
   top-level names (`GwzM5-8I2RecordContract.md:367-373`).
4. **Within-v1 compatibility**: every M5b-executable shape is already
   decode-legal in this tree's v1 validator; M5b adds proofs, not legality
   flips, so no staged record written by the current test corpus changes
   meaning.

### 3.5a Production resume-side truth (Code F-1, P1) and the adjudicated gate

**The truth this freeze must state rather than assert away.** In today's
production v0 binary, the two-parent execution machinery is reachable
through a forged open v0 record, and the contracts' "before resume"
promise is not implemented on that lane:

- Production load performs no mode or (mode, kind) gating: `store/mod.rs:244-247`
  → `decode_production_v0` (`NoFf` is a valid enum variant) →
  `validate_record` (`store/mod.rs:197-214`) checks merge-id/schema/filename
  only; `validate_v0_structure` is called solely from the cfg(test) adapter
  (`adapter.rs:97`); nothing on the production resume path consumes
  `record.mode` after plan time (`plan.rs:211-241`).
- A forged participant row — retry-eligible state, `pending_action` kind
  `true_merge` / expected `commit` / `commit_spec.tree_oid` = the source
  tree, over a genuinely fast-forwardable `(before_commit, source_commit)`
  pair, with or without the `mode` field — rides the durable continue
  branch (`continue_op/execution.rs:42-58`), maps `TrueMergeCommit(spec)`
  straight to `GitPreparedMerge::Commit` without re-preparation
  (`continue_op/execution.rs:170-173`), classifies NotStarted through the
  `(FastForward, Commit)` arm (`merge_support.rs:101-112`), and executes a
  two-parent commit where fast-forward was possible
  (`merge_prepared.rs:296-336`) — no-ff semantics in the production v0
  binary, contradicting `GwzM5-8I2CompatibilityContract.md:163-165` and
  `GwzM5-8I2RecordContract.md:343-344` as they read today.

**Adjudicated disposition (lane owner, incorporated verbatim as a freeze
position).** The doctrine-consistent fix is a **small production gate,
landing as its own package outside M5b** — the **"v0 forged-action resume
gate"** package, implemented in parallel with focused State review: v0
continue preflight rejects a durable two-parent-over-fast-forwardable
action outright (v0 has no legitimate writer of that shape), with a typed
error and forged-row fixtures for **both** the `mode: no_ff` row and the
modeless forged action. The contract promise is discharged by that named
package, **not** by the cfg(test) adapter and not by M5b. Consequences
inside this freeze:

- §3.2's unreachability header is scoped to the v1 side (done above);
- tripwire **T-6** (§6) references the package's landed gate and its named
  suites, and M5b's settled acceptance re-runs them — the gate package
  therefore lands before M5b settled acceptance;
- Q8's 0-production-line ceiling is **ratified with no ceiling exception**:
  the gate is production lines, so it lives in the parallel package's own
  budget row, never quietly absorbed into M5b (§8.3);
- M5b's Refactor bullet 1 ("without adding it to writable v0") remains
  satisfied — the F-1 lane concerns forged records, not writers.

## 4. DETERMINISM — what the frozen two-parent commit is

### 4.1 Definition

For an otherwise fast-forwardable (or cleanly true-mergeable) participant
under `mode: no_ff`, "deterministic" means: **after the durable action write,
the full byte content of the commit object — and therefore its OID — is a
pure function of the persisted `PendingMergeAction`.** Every input to
`git commit-tree` is either in the action or implied by it in a frozen
order:

| Commit-object input | Source in the frozen action | Citations |
| --- | --- | --- |
| parent[0] | `before_commit` | `GwzM5-8I2RecordContract.md:339`; `merge_prepared.rs:301,311` |
| parent[1] | `source_commit` | same; execution passes `[&head_commit, &source_object]` (`merge_prepared.rs:304-313`) |
| tree | `commit_spec.tree_oid` — captured at preparation: the **source commit's tree** when the analysis is fast-forwardable (`merge_prepared.rs:168-173`), the in-memory merge index tree written to the ODB when a clean true merge (`merge_prepared.rs:181-192`) | revalidation pins ff-able tree == source tree (`merge_support.rs:101-112`) and true-merge tree == recomputed clean index by empty diff (`merge_support.rs:120-141`) |
| message | `commit_message` — the R2a-frozen final bytes (`final_member_commit_message`: normalized custom body or default, plus the mandatory `GWZ-Merge-ID`/`GWZ-Operation-ID` recovery identity), NUL-rejected again at execution | `integration.rs:193-227`; `merge_prepared.rs:228-233`; message-byte equality on self-verify `merge_prepared.rs:318` |
| author | `commit_spec.author` = name, email, **epoch seconds, offset** | `model/v0.rs:131-139`; captured `merge_support.rs:296-317` |
| committer | `commit_spec.committer`, same fields | same |
| encoding/extra headers | none are ever set; libgit2 `commit()` writes the canonical form | `merge_prepared.rs:304-313` |

### 4.2 How timestamp determinism is achieved

The timestamps are **pinned at preparation and persisted**, not re-stamped:

- Preparation resolves author/committer via `merge_signatures`
  (`merge_support.rs:278-294`): the request's
  `OperationAttribution.git_author/git_committer` when present (protocol
  identity with optional `time_ms`/offset —
  `gwz-core/src/model/mod.rs:340-377`, materialized at
  `merge_support.rs:356-386`), else the **effective Git identity**
  (repo-local → global → system config resolution) stamped **now**
  (`repo.signature()`; State P3-3), else the stable fallback
  `gwz <gwz@localhost>` (`merge_support.rs:271-276`).
- `prepared_signature` copies `when().seconds()` and offset into the durable
  spec (`merge_support.rs:296-317`); `to_pending`/`pending_commit_spec`
  serialize them (`integration.rs:229-246`).
- Execution reconstructs `git2::Signature` **from the frozen values**
  (`signature_from_prepared`, `merge_support.rs:319-344`) and self-verifies
  the created commit's signatures with second/offset equality
  (`same_signature`, `merge_support.rs:388-393`; check at
  `merge_prepared.rs:320-321`).
- Restart adoption compares seconds/offset too
  (`signature_matches_prepared`, `merge_support.rs:346-354`;
  `commit_matches_prepared_merge`, `merge_prepared.rs:39-69`).

Consequence, stated precisely so it cannot be read as "every crash point
resumes automatically" (State P2-1): every commit object the frozen action
ever produces — first execution, or re-execution after a crash that left
the repository exactly at NotStarted — is **byte-identical**, and an
already-created commit is adopted, never re-executed; the wall clock never
re-enters. One mid-execution crash window does **not** auto-resume: a crash
between worktree materialization and ref publication
(`merge_prepared.rs:328-333` — `checkout_tree` done, ref transaction not
committed) leaves the worktree at the frozen tree with the ref at
`before_commit`, which classifies Ambiguous; continue is refused
(`RecoveryEvidenceMismatch`, `forward.rs:497-501`) and abandonment is
refused (not NotStarted) — a typed, fail-closed operator stop. After the
operator restores the before-state worktree, reconciliation re-yields
NotStarted and re-execution produces the **identical OID** — the pinned
suite is §7's
`no_ff_crash_between_worktree_materialization_and_ref_publication_classifies_ambiguous_and_refuses_abandonment`.
This is "from-freeze" determinism. It is deliberately *not*
cross-operation reproducibility: two operations frozen at different moments
carry different `time_seconds` by design (Q4 records the alternative and why
this design rejects it by default; both round-1 review axes affirmed the
from-freeze meaning).

### 4.3 Commit identity source and the fixture precedent

Production commit construction reads identity exactly as above; nothing else
in the tree constructs merge commits. The fixture precedent the program
already relies on: `pin_fixture_identity`
(`gwz-core/src/workspace_ops/tests/g02.rs:847-856`) pins
`user.name`/`user.email` ("GWZ Test"/"gwz@example.invalid") in repos whose
commits are created by **production** code, because CI runners carry no
machine-level identity. M5b's determinism suites (§7 P-DET) must do the
same — pin repo identity (or pass explicit attribution with pinned
`time_ms`) so the *frozen-action → OID* function is the thing under test,
not the runner's environment. Note `signature_from_prepared` also
revalidates the frozen identity through `GitObjectIdentity.validate()`
(`merge_support.rs:322-333`) and the wire validator bounds offsets to
±1440 minutes and forbids control bytes (`model/v1/validate/action.rs:103-109`)
— a forged spec fails closed before mutation.

### 4.4 No cryptographic signing of integration commits

Scoped precisely (Code F-4; State P3-1 — the earlier "no gpg/signing call
sites in gwz-core/src" sentence was overbroad and is withdrawn): **neither
integration-commit construction site signs.** Both are direct libgit2
`repo.commit(...)` calls, which ignore `commit.gpgsign` — the two-parent
execution path (`merge_prepared.rs:304-313`) and the conflict-resolution
path (`merge_recovery.rs:424-433`). Integration commits gwz executes are
therefore unsigned Git objects on every platform, and a user's
`commit.gpgsign=true` cannot break determinism or self-verification.
Elsewhere the backend deliberately honors signing: the AD1 porcelain
fallbacks for ordinary commits and tags shell out to `git` precisely so
"hooks / signing / committer config are honored" (`repository.rs:293`
comment; `refs.rs:171`; `contract.rs:813,865-866`) — those are not
integration paths and are untouched by M5b. §3.4 pins the scoped wording
in contract text. For what adoption of an externally *signed* twin commit
means, see §4.6.

### 4.5 Tree-object lifetime

For the clean-true-merge arm, preparation writes the tree into the object
database before the record write (`merge_prepared.rs:186`). A crash between
tree write and record write leaves an unreferenced tree — inert, ordinary
`git gc` fodder, observed by nothing (the action was never persisted, so
restart re-prepares). After the record write, revalidation requires that
exact tree to exist and still equal the recomputed clean merge
(`merge_support.rs:122-141`); a pruned tree object is a typed
`MergeRecoveryRequired` mismatch, never a silent re-derivation. For the
fast-forwardable arm the tree is the source commit's own tree, alive as long
as the source commit the action pins.

### 4.6 Adoption is field-wise, not OID-wise; forged-spec edges (Code F-6)

Two edges of the adoption/validation model, stated so the determinism
claim cannot be over-read:

- **Field-wise adoption.** Restart adoption and completion matching compare
  the frozen fields — two parents in order, message bytes, tree, author and
  committer identity lines with seconds/offset
  (`merge_prepared.rs:39-69`; `merge_support.rs:346-354`) — not the commit
  OID or raw bytes. A hand-crafted twin commit matching every frozen field
  but carrying an additional header (e.g. a `gpgsig` added by an external
  signer) is adopted Completed, with an OID different from the §4.1 pure
  function's. This is fail-safe in evidence terms — every frozen field
  still matches, so no wrong evidence is adopted — but the **byte-identity
  claim of §4.2 covers gwz-executed commits only**; adopted foreign twins
  are field-verified, not byte-verified. §3.4's amendment pins this
  wording.
- **Non-canonical forged specs fail closed, availability-only.** The wire
  validator forbids `\0\n\r` and bounds offsets (`action.rs:103-109`) but
  does not reject leading/trailing spaces in name/email; libgit2's
  `Signature::new` trims that "crud", so a forged space-padded spec can
  execute — and the created commit then never satisfies
  `signature_matches_prepared` **on any restart or re-verification
  before the outcome write** (an uninterrupted run completes its outcome
  write normally; the permanent Ambiguity materializes when
  restart-based reconciliation of the still-pending action intervenes —
  round-2 State P3): wrong evidence is never adopted; availability is
  lost until operator action. M5b pins this with the §7 negative row
  `forged_non_canonical_signature_spec_executes_then_never_reconciles`;
  a validator trim/canonicality rule is deliberately **not** taken in M5b
  (it would be a production validator edit — dual-tier under §8.1's rule
  and outside the 0-production-line ceiling) and is flagged to the A1 /
  validator lane instead.

## 5. RECONCILIATION — continue/abort/preservation consume the frozen action

### 5.1 Authority rule (frozen)

After the durable action write, **the persisted `PendingMergeAction` is the
sole mode authority for that participant**. Live analysis is used only to
*verify* the frozen action's continued validity — never to re-choose
fast-forward vs merge-commit. Concretely:

- The executor decodes the frozen action and refuses to run if the persisted
  row no longer matches its execution authority
  (`v1_lifecycle/forward/execute.rs:8-26,14`;
  `verify_participant_action`, `forward.rs:39-83`).
- `execute_prepared_merge_upstream_checked` re-classifies the live pair
  only to *check* it against the frozen variant; the arms are closed and a
  frozen `Commit` can never take the fast-forward publication path
  (`merge_prepared.rs:240-263,296-336`; closed classifier
  `merge_support.rs:98-152`). A live class drift (up-to-date/ff/true-merge
  flip, tree drift, moved head, dirtiness) is a typed
  `MergeRecoveryRequired`/`MergeDrift`, not a reclassification.
- Restart reconciliation (`reconcile_pending_action`,
  `status/pending.rs:26-237`) classifies exactly NotStarted /
  Completed / ExpectedConflict / Ambiguous **from the frozen action**: a
  pending two-parent action is NotStarted only at the exact `before_commit`
  with the frozen spec still valid (`pending.rs:117-133`), Completed only
  when the live head matches the frozen two-parent commit field-exactly —
  parents in order, message bytes, tree, both identity lines with
  seconds/offset (`pending.rs:134-151`; field-wise, not OID-wise — §4.6);
  everything else is Ambiguous and authorizes nothing. Completion state
  maps from the **durable kind** (`TrueMerge → Merged`,
  `forward.rs:357-362`) — a no-ff result is never relabeled
  `FastForwarded`.
- A property the State review verified and asked stated (worth freezing as
  words): between the two *pinned* OIDs, `classify_merge`
  (`merge_support.rs:4-30`) is a pure function of immutable ancestry — the
  frozen pair's classification can never drift; only object loss can
  intervene, and that fails typed. The ff-able no-ff arm is additionally
  immune to merge-algorithm version skew, because its revalidation is tree
  equality **by OID** with no recomputation (`merge_support.rs:101-112`);
  the clean-true-merge arm recomputes the merge and fails closed on skew
  (`merge_support.rs:120-141`).

This satisfies "reconcile … from that exact v1 action rather than re-deriving
merge mode from live analysis" with one deliberate nuance the review should
bless: *before* an action exists (participant `Planned`/`Failed`/
`Unattempted`), preparation legitimately runs live analysis
(`forward.rs:174-204`) — mode still comes from the record
(`prepared_mode(current.record().mode)`, `forward.rs:189,230-236`), and the
recorded inputs must equal the analysis (`forward.rs:179-187`). The freeze
point is the durable action write; the Refactor's sentence binds everything
after it.

### 5.2 Continue

Extends nothing: the existing v1 forward phases (`ParticipantPreparation`,
`ParticipantAction`, recovery verification) already carry the no-ff arms
(§2.1). New **exact observations required: none**. The one M5b-specific
observation subtlety is already closed: a pending `TrueMerge/Commit` over a
*fast-forwardable* analysis validates NotStarted through the
`(FastForward, Commit)` arm's source-tree equality
(`merge_support.rs:101-112`) — i.e., the observer proves "this frozen
two-parent action is still exactly executable" without ever asking "what
mode would I choose now". The two disagreement windows are pinned by named
§7 suites (State P2-1 — this paragraph previously referenced nonexistent
"F-2/F-3" suites, repaired here): post-freeze **external divergence**,
doctrinally centrally the target branch externally fast-forwarded to
`source_commit` while the two-parent action is pending, must classify
Ambiguous and never be adopted
(`no_ff_external_fast_forward_is_ambiguous_never_adopted` — sound in code
at `pending.rs:117-151`, where `commit_matches_prepared_merge` fails on
parent shape); and gwz's **own mid-execution crash window** between
worktree materialization and ref publication must classify Ambiguous with
both continue and abandonment refused
(`no_ff_crash_between_worktree_materialization_and_ref_publication_classifies_ambiguous_and_refuses_abandonment`,
§4.2). Completion → adoption is pinned by the existing restart suite
(`tests/forward.rs:103-139`).

### 5.3 Abort / preserve-abort (abandonment) and rollback

- A pending no-ff action may retire without an integration outcome **only**
  via the bound exact `NotStarted` observation consumed atomically by
  abort/preserve entry — recording deliberate abandonment, leaving
  pre-action state untouched, fabricating no result
  (`GwzM5-8I2RecordContract.md:354-363`; machinery
  `transition/mod.rs:153-158`, `transition/reverse_entry.rs:82-88`,
  effects `transition/effect.rs:38-39`). The NotStarted proof for the
  two-parent action is the same closed classifier as §5.2 — so an
  already-created two-parent commit can never be "abandoned": it
  reconciles Completed first (`forward.rs:296-371`), and ambiguous
  observations authorize no reverse entry
  (`GwzM5-8I2ActionJournalContract.md:139-166`).
- Rollback of a completed no-ff participant is `ResetIntegrated` anchored on
  the recorded two-parent result: not-started is ref/HEAD at the recorded
  result with the clean result index/worktree; completed is `before_commit`
  with the exact clean before state; any third state is ambiguous
  (`GwzM5-8I2ActionJournalContract.md:222-227`). Mode never appears in the
  classifier — the *recorded result commit* is the authority, which is the
  point.
- Preservation of a completed no-ff participant anchors backup refs on the
  immutable owner anchor = the participant result (the two-parent commit)
  (`GwzM5-8I2ActionJournalContract.md:175-177`); a pending forward action
  remains forbidden in effective `Preserving`
  (`GwzM5-8I2RecordContract.md:346-352`), so the preservation cursor never
  meets a half-frozen no-ff action.

### 5.4 Exact-evidence doctrine interplay (ExactEvidencePlatformAmendment)

Where the accepted amendment's clauses touch M5b:

- **Clause A leaves the forward no-ff edges filtered.** The checked upstream
  fast-forward and the post-true-merge `checkout_tree` in
  `merge_prepared.rs` are enumerated "untouched — forward direction"
  (`GwzM5-8ExactEvidencePlatformAmendment.md:94-95`); the two-parent commit's
  worktree materialization (`merge_prepared.rs:328-329`) is that same
  forward class. Consequence M5b must state, not fix: on a Windows
  autocrlf host, a no-ff integration materializes filtered bytes, and the
  raw-byte reverse observers (`observe_v1_participant_rollback` et al.) can
  later classify unchanged-but-smudged paths Ambiguous — the amendment's
  tracked open exposure ("unrewritten smudged files", State F1 = Code P2,
  `GwzM5-8ExactEvidencePlatformAmendment.md:99-130`). Fail direction is
  closed (availability, never wrong evidence). M5b adds **no new rewrite
  edge and no new filter decision**, so it neither widens nor narrows the
  tracked set; its rollback proofs run under the Unix lanes plus the
  fixture pins the matrix already standardized (`pin_fixture_autocrlf`,
  `g02.rs:832-845`). Updated for the adopted A1 decisions (Code F-3; State
  §3): **D1** (Option B creation-time filter neutralization, adopted
  2026-08-16, `CurrentProgramCheckpoint.md:122-141`) narrows the practical
  exposed population — gwz-born repos are LF-from-birth — without touching
  the doctrine M5b freezes; Clause A stays delta-only and forward stays
  filtered, so **no §5.4 statement changes**, and the residuals (adopted
  worktrees, attribute-driven smudge) keep "M5b must not claim Windows
  reverse-path closure" exactly right. The D1 package itself carries the
  un-pinned CRLF matrix sentinel, which is the class-level sentinel Q7's
  resolution relies on (§9 Q7 — resolved: no M5b-specific sentinel; one
  classification-ledger sentence noting `mode: no_ff` makes the commit-arm
  materialization reachable for otherwise-ff-able participants, recorded
  as an M5b-IMPL ledger obligation since both the ff path and the commit
  path were already class members — bookkeeping, not new exposure).
- **Clause A covers the rollback of a no-ff result** exactly as any
  `ResetIntegrated`: the recovery-grade checkout is blob-exact, so the
  before-state the two-parent rollback restores is verified raw-byte
  (`GwzM5-8ExactEvidencePlatformAmendment.md:87-88`).
- **Clause B′ reaches no-ff verification already**: `verify_merge_result`
  post-verifies forward integration at `merge_prepared.rs:248,261,335` and
  now tolerates checked-artifact private-area residue
  (`GwzM5-8ExactEvidencePlatformAmendment.md:257-264`) — the two-parent
  commit path (`:335`) is inside that enumerated reach; M5b inherits it and
  must not re-tighten it.
- The R4b doctrine sentence M5b's reconciliation lives under: "persist a
  typed owner before every participant … mutation and accept a result only
  through the matching opaque exact-observation proof"
  (`GwzM5-8Refactor.md:2256-2258`).

## 6. UNREACHABILITY — the activation boundary M5b preserves

M5b mirrors the existing idiom exactly; it introduces **no new gating
mechanism** and no runtime flag (the boundary is compile/call-graph, per
`GwzM5-8I2CompatibilityContract.md:275-280`):

1. **Module gate**: all M5b lifecycle code lands inside
   `#[cfg(test)] mod v1_lifecycle;` with the compile sentinel
   (`merge/mod.rs:23-26`) and the module-local dead-code allowance naming
   A1 as the activation point (`v1_lifecycle/mod.rs:1-5`).
2. **Wire gate**: the production decoder keeps
   `InstalledMergeRecordVersions::PRODUCTION_R3` (v1 uninstalled) and the
   `unreachable!` v1 arm (`record_wire/decode.rs:82-86`); v1 body decode,
   open-v1 helpers, and migration stay `cfg(test)` per the Amendment-1 §4
   normal-build split (`GwzM5-8I2CompatibilityContract.md:7-13`;
   `record_wire/mod.rs:13-23` and sibling `cfg(test)` exports).
3. **Request gate**: `validate.rs:12-14` keeps rejecting `--no-ff` at start
   (`MergePhaseUnsupported`), pinned by
   `custom_messages_validate_while_no_ff_remains_reserved`
   (`validate.rs:201-225`). This is the "CLI/protocol start activation"
   boundary; both CLI and protocol drivers funnel through it
   (`runtime/dispatch.rs:114-134`).
4. **Backend arm reachability**: the `ForceMergeCommit` arm is
   production-compiled (`merge_prepared.rs:140-195` is not test-gated) but
   production-unreachable: every production caller uses
   `prepare_merge_upstream_checked`/`AllowFastForward`
   (`merge_prepared.rs:121-138`; v0 start/continue via
   `start/execution.rs:151-167`, `continue_op/execution.rs:117`; pull-head
   preflights), and every `ForceMergeCommit` construction/caller site is
   cfg(test) code — the v1 lifecycle (`forward.rs:198,232`) and the g12
   backend test (`git/tests/g12.rs:217`), plus the declaration and the
   default-trait **rejection** arm (full-crate enumeration re-derived by
   the round-1 Code review §4). The default
   trait impl even rejects it for non-implementing backends
   (`contract.rs:277-279`). M5b keeps this a call-graph property and hands
   its verification to the R4b-G call-graph gate
   (`GwzM5-8Refactor.md:2243-2244`) plus tripwire T-4 below. This gate
   covers the preparation arm only — see the scope statement after gate 5
   for the consumption lane it does not cover.
5. **Writer gate**: no v0 serialization path can emit `mode: no_ff`
   (§3.5 item 1); v1 serialization exists only under `cfg_attr(test,
   derive(Serialize))` (`model/v1/record.rs:19`) and the cfg(test) checked
   store.

**Scope of these gates, stated honestly (Code F-1):** gates 1-5 protect the
request surface, the v1 record surface, the preparation arm, and the v1
writer. They do **not** close the forged-v0 record lane into the
production-compiled two-parent *consumption* machinery (§3.5a). That lane
is closed by the parallel "v0 forged-action resume gate" package; tripwire
T-6 binds M5b's settled acceptance to its landed gate. No gate in this list
may be cited as covering that lane.

**M5b tripwires** (all cheap, all inverted by A1's checklist except T-6,
which survives A1 for the v0 lifecycle's lifetime):
T-1 start-rejection stays (exists, cited above); T-2 production decoder
rejects a v1 no-ff record file with typed `required_wave: A1` context
(exists for v1 generally in R3 suites; add the no-ff body variant); T-3
**no writer-produced or positive fixture** in the corpus serializes
`mode: no_ff` — reworded per Code F-7: the corpus-wide "no v0 fixture"
scan would fail today against g23's **deliberate negative fixtures**,
which forge `no_ff` rows precisely to prove rejection
(`workspace_ops/tests/g23/compatibility_v0.rs:94-97`,
`compatibility_v0_edges.rs:38,68,164-180`, `atomic_upgrade_v0.rs:79-85`);
the assert therefore scans writer outputs and positive-path fixtures only,
with the negative-fixture files enumerated as the exact allowlist;
T-4 call-graph assertion that `ForceMergeCommit` construction sites are
`v1_lifecycle`-only — **resolved variant: the structural scan**
(boundary-checker idiom: enumerate construction sites, assert the
allowlist), reverted from round 1's `#[cfg(not(test))]` compile-shim per
the round-2 Code finding N-1: the shim is compiled into production
builds (contradicting the ratified §8.3 zero-production-line ceiling)
and has no coherent zero-production form, since production code
legitimately matches on the variant (`merge_prepared.rs:140-195`,
`contract.rs:269-280`) — a poisoned variant is a compile break, not a
tripwire. The State axis's machine-checked-not-convention-checked
requirement is preserved: the structural scan executes in CI on every
push via the checker lane;
T-5 retained-reader manifest lane: v0.10.2 rejects the
v1 no-ff envelope pair exactly as the frozen matrix requires
(`GwzM5-8I2CompatibilityContract.md:266-273` — fixture addition to the
existing harness, not a new harness, per `GwzProcessOptimization.md` §5.2);
T-6 **(new, Code F-1 / adjudicated §3.5a)**: the "v0 forged-action resume
gate" package's landed production gate and its named suites —
`v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`
(modeless forged action) and `v0_resume_rejects_forged_no_ff_mode_row`
(mode row) — re-run green in M5b's settled gate, proving the production v0
continue preflight rejects a durable two-parent-over-fast-forwardable
action with the package's typed error before any Git mutation. T-6 red or
the package unlanded blocks M5b settled acceptance (§8.2).

## 7. PROOF PLAN — named suites the Refactor's M5b bullets demand

Grouped by obligation; names follow the existing suite conventions
(`v1_lifecycle/tests/forward.rs`, `model/v1/validate/*_tests.rs`,
`tests/acceptance_v0`, `transition_matrix_v0.rs`). "Exists" marks work
already accepted with R4b-X that M5b's gate re-runs rather than rewrites.

**P-FWD — forward matrix exactness** (`v1_lifecycle::tests::forward`):

- `no_ff_fast_forward_creates_a_two_parent_merge_commit` — exists
  (`tests/forward.rs:81-100`).
- `no_ff_restart_adopts_the_exact_prepared_merge_commit` — exists
  (`tests/forward.rs:103-139`).
- NEW `no_ff_up_to_date_adopts_verify_up_to_date_without_execution`
  (mirror of `tests/forward.rs:55-78` under `mode: no_ff`; proves the "no-ff,
  up to date" row and zero Git actions).
- NEW `no_ff_true_merge_conflict_row_and_resolution_commit` (divergent no-ff
  → expected-conflict, then resolution; proves the divergent row is
  byte-identical to normal mode's).
- NEW `no_ff_clean_true_merge_matches_normal_mode_bytes` (Code F-2 — the
  clean-true-merge half of the frozen matrix cell
  `GwzM5-8I2RecordContract.md:331`, unproven end-to-end because both
  existing no-ff suites use `Kind::FastForward` fixtures
  (`tests/forward.rs:82,104`): divergent-clean under `mode: no_ff` produces
  a commit byte-for-byte equal to normal mode's — tree is the merge-index
  tree, not the source tree — discharging the exit bullet's *complete*
  matrix demand).
- NEW `no_ff_external_fast_forward_is_ambiguous_never_adopted` (State
  P2-1a — the target branch is externally fast-forwarded to
  `source_commit` while the two-parent action is pending: classification
  is Ambiguous, adoption never occurs, no mutation is authorized; §5.2).
- NEW `no_ff_preparation_persists_the_frozen_action_before_any_git_mutation`
  (crash injected between the durable action write and execution; restart
  observes NotStarted from the frozen spec and completes with the
  spec-determined commit).
- NEW `ff_only_and_normal_matrices_are_unchanged_under_the_m5b_tree` (mode ×
  analysis sweep asserting `prepared_mode`/`validate_mode` rows:
  ff_only+true-merge rejected, normal ff stays a durable FastForward —
  `forward.rs:220-236`).

**P-DET — determinism of the two-parent commit**:

- NEW `no_ff_commit_oid_is_a_pure_function_of_the_frozen_action`: with
  pinned identity (per `g02.rs:847-856` precedent) construct the commit
  offline via git2 from the persisted action's seven inputs (§4.1 table) in
  a second clone; assert OID equality with the production result. This is
  the strongest form of "deterministic two-parent commit" and subsumes
  byte-level message/tree/signature equality.
- NEW `no_ff_reexecution_after_crash_is_byte_identical` (execute → crash
  **after ref publication**, before the outcome write → restart adopts;
  assert the adopted OID equals the offline-computed OID; extends the
  existing restart test with the byte claim. Crash-point scoping per State
  P2-1: this row deliberately lands its crash after the ref move — the
  pre-publication window is the separate row below).
- NEW `no_ff_crash_between_worktree_materialization_and_ref_publication_classifies_ambiguous_and_refuses_abandonment`
  (State P2-1b — crash in the `merge_prepared.rs:328-333` window:
  worktree/index at the frozen tree, ref still at `before_commit` →
  Ambiguous; continue refused (`RecoveryEvidenceMismatch`,
  `forward.rs:497-501`) **and** abandonment refused (not NotStarted) —
  typed, fail-closed stop; then the operator restores the before-state
  worktree → reconciliation re-yields NotStarted → re-execution produces
  the **identical OID**, the strongest witness of the determinism claim;
  §4.2).
- NEW `frozen_signature_timestamps_survive_restart` (explicitly asserts
  `time_seconds`/offset equality through persist → decode → execute →
  adopt; pins amendment M5b-W1's first clause).

**P-REV — reverse-path consumption** (`v1_lifecycle::tests::reverse` family):

- NEW `abort_abandons_a_not_started_no_ff_action_via_bound_exact_observation`
  and `preserve_abort_abandons_a_not_started_no_ff_action` (record-contract
  §7 abandonment; asserts pre-action state untouched, no fabricated
  outcome, unknown fields under the retired action container retire with it
  per `GwzM5-8I2RecordContract.md:382`).
- NEW `abort_refuses_to_abandon_a_created_two_parent_commit`
  (repo already carries the frozen commit → reconciliation Completed →
  abandonment illegal; ambiguity path asserts typed stop).
- NEW `rollback_resets_a_no_ff_merged_participant_to_before_commit`
  (ResetIntegrated over the two-parent result; observation anchors per
  journal contract §3; Clause A blob-exact expectation on the restored
  bytes).
- NEW `preservation_backup_ref_anchors_on_the_two_parent_result`
  (owner anchor equality, journal contract §2).
- **Order-independence instruction (State P2-2, binding on M5b-IMPL):**
  the P-REV preservation suites must **not** assert the absence of
  preservation-cursor rows and must tolerate the adopted D3 wire (per-owner
  no-op skip rows + reset-completion bit,
  `GwzM5-8DurableCursorAmendment.md`) landing **first** — the two pre-A1
  amendment trains touch the same journal contract, and M5b's suites must
  pass in either landing order (§8.2).

**P-WIRE — validator/decoder rows** (`model/v1/validate::action_tests`,
`record_wire` suites):

- matrix accept/reject rows — exist (`action_tests.rs:44-104`).
- NEW `forged_non_canonical_signature_spec_executes_then_never_reconciles`
  (Code F-6b negative row: a space-padded name/email spec passes the wire
  bounds (`action.rs:103-109`), executes through libgit2's crud-trim, and
  the created commit never satisfies `signature_matches_prepared` —
  permanently Ambiguous, typed stop, no wrong evidence; §4.6).
- NEW `two_parent_restart_reconciliation_rows` closing the record-contract
  exit bullet (`GwzM5-8I2RecordContract.md:420-421`): decode → reconcile
  classification table for the no-ff shapes, including intent-mismatch and
  merge-head-mismatch rejections (`integration.rs:175-191`).
- NEW round-trip: v1 record with `mode: no_ff` + frozen action + unknown
  fields inside `pending_action`/`commit_spec` survives rewrite and retires
  on reconciliation (extends the §8 retirement matrix suites).

**P-V0 — v0 exactness untouched** ("prove normal/divergent/up-to-date
behavior remains exact"):

- Re-run, unmodified, the M4 characterization corpora —
  `tests/acceptance_v0`, `transition_matrix_v0.rs`, and the §15.3.2 fixture
  suites — with a diff-guard that M5b's merge introduces zero production-
  reachable change outside the cfg(test) module (the same
  byte-equivalence discipline R4a/R4b used, `GwzM5-8Refactor.md:2264-2265`).
  M5b's claim here is *absence* of change; the suites are the proof.
- T-1..T-6 unreachability tripwires (§6). T-6's forged-row fixtures — both
  the `mode: no_ff` row and the modeless forged two-parent action — are
  authored and land in the "v0 forged-action resume gate" package (§3.5a);
  M5b's settled gate re-runs them rather than duplicating them.

Placement note: all NEW suites extend existing harnesses; none adds a
bespoke apparatus (`GwzProcessOptimization.md` §5.2). Suites P-FWD/P-DET
must be written against the `service::run` seam so A1 can re-point them at
production dispatch without rewriting them (Q5).

## 8. FREEZE PLAN

### 8.1 What freezes, when, and at what tier

1. **M5b-IF (this document + amendment M5b-W1)** — the interface checkpoint.
   Content frozen: §3 wire posture and A1/M5b activation split, §4
   determinism semantics, §5 reconciliation authority rule, §6 boundary +
   tripwire list, §7 suite obligations, §8.3 budget row. Review: **dual
   peer-blind, cross-model** — interface freezes and amendments are the
   mandated tier (`GwzProcessOptimization.md` §4.2 at :106-118, §4.3); the
   tier is recorded in `CurrentProgramCheckpoint.md` at freeze per the same
   rule. M5b-W1 lands on the record contract only after both reviews accept
   (amendments are themselves mandated-tier items).
2. **M5b-IMPL** — one implementation package behind the frozen interface
   (single coherent owner per L1-06; files: `v1_lifecycle/**` tests +
   minimal glue, `model/v1/validate/*_tests.rs`, fixture additions; no
   production-reachable file outside the §6 boundary). **Review tier —
   reconciled rule, recorded at freeze, not chosen mid-lane (supersedes
   this design's earlier draft posture; lane-owner adjudication of Q6
   reconciling the two round-1 axis positions): M5b-IMPL is mandated-dual
   BY DEFAULT; single-axis is permitted only when the diff is strictly
   confined to test modules/files/fixtures (`v1_lifecycle/tests/**`,
   `*_tests.rs`, fixture corpora); any edit to non-test items under
   `v1_lifecycle/{authority,forward,transition,checked}/**`,
   `model/v1/validate/*.rs`, or `git/gitbackend/**` is dual.** Rationale
   recorded with it (State §5): the 0-production-line ceiling is an
   *unreachability* guarantee, not a *semantics-neutrality* guarantee —
   inside the module-root `cfg(test)`, "observation-glue" could rewrite
   A1-destined state semantics while satisfying the ceiling, and the next
   dual gate after IMPL would otherwise be A1 itself, the exact cliff
   `GwzM5-8ProgressReviewF5.md` §3.2 warns about.
3. **M5b settled acceptance** — suites green, ledger row reconciled,
   tripwires T-1..T-6 in the gate (T-6 requires the §3.5a package landed),
   `CurrentProgramCheckpoint.md` updated; freeze tuple re-cut against a
   clean committed tree (§2, State P3-2). Per the release sequence this is
   the direct prerequisite of A1 (`CurrentProgramCheckpoint.md:315-316`;
   `GwzMergeCheckpoint-v0.10.5.md:119-130`).

### 8.2 Dependency statement (explicit, per the task)

**Independent of R4b-G settling — may proceed now:**

- This freeze document, its dual review, and amendment M5b-W1 (documents
  only; pipeline-lane rule `GwzProcessOptimization.md` §4.4 explicitly
  permits starting N+1 behind frozen interfaces while reviewers hold N).
- Everything M5b consumes that is already individually accepted:
  the I2 wire rows (I1/I2 accepted, re-frozen 2026-08-11 with banners —
  `CurrentProgramCheckpoint.md:71-76`), the mode-aware backend
  (`merge_prepared.rs`/`contract.rs`, accepted with R4b-X:
  `GwzM5-8R4bTransitionDesign.md:1209-1219`), R2a message bytes, and the
  exact-evidence amendment (accepted 2026-08-16).
- Drafting (not merging) the §7 suites.
- The parallel **"v0 forged-action resume gate" package** (§3.5a): its own
  lane, its own focused State review, its own budget row — R4b-G-independent
  (it edits the v0 continue preflight, not the `v1_lifecycle` tree). M5b's
  freeze does not wait for it; M5b's **settled acceptance** does (T-6).

**Dependent on R4b-G settling — must wait:**

- Merging and settled-review of M5b-IMPL. Reasons: (a) the program's frozen
  resume order is R4b-G → M5b → A1 (`CurrentProgramCheckpoint.md:315-316`;
  `GwzMergeCheckpoint-v0.10.5.md:128-130`); (b) M5b-IMPL edits the same
  `v1_lifecycle` tree whose P2/P3/P4 lanes (~3.5k production lines) are
  implemented but **not yet independently accepted**
  (`GwzM5-8ProgressReviewF5.md` §2.1) — landing M5b there first would give
  R4b-G's settled-tree/call-graph/byte-equivalence gates a moving target and
  alias finding ownership; (c) M5b's own unreachability argument leans on
  R4b-G's call-graph gate (§6 item 4).
- The R4b-G gate itself is behind the RemPlan-4 R2-R6 chain (scope
  correction of 2026-08-16, `CurrentProgramCheckpoint.md:108-120`); M5b
  inherits that transitively and takes no dependency on its internals.

**Adopted A1 decisions — authority refresh (State P2-2; Code F-3; re-dated
2026-08-16).** This design's round-1 draft froze the exact-evidence
amendment's foreign-filter policy and real-Windows satisfiability as "two
OPEN DECISIONS … tracked review debts". That language is superseded: the
**A1 decision packet's three recommendations were ADOPTED 2026-08-16**
(gwz-dev `0298edb`; `CurrentProgramCheckpoint.md:122-141`;
`GwzM5-8A1DecisionPacket.md`):

- **D1** — real-Windows satisfiability = Option B, creation-time filter
  neutralization (autocrlf/eol pins at `create_repo`, clone filters-off at
  the transport funnel, post-A1 renormalize command, permanent fail-closed
  doctrine note, and the un-pinned CRLF matrix sentinel). Scheduled with D2
  as one filter-policy package. Effect on M5b: §5.4 unchanged in doctrine;
  Q7 resolved at class level.
- **D2** — foreign-filter policy = A′ refined refusal, **release-gated,
  not A1-gated**. No M5b interaction beyond §5.4's unchanged posture.
- **D3** — durable preservation cursor = minimal durable cursor
  (**per-owner no-op skip rows + reset-completion bit**) as a pre-A1,
  mandated-dual I2 ActionJournal/Record amendment; under mandated dual
  review — round-1 NO-GO/NO-GO with the wire shape affirmed on both
  axes, text remediation applied and committed (`e9396a9`), focused
  re-verdicts pending (`GwzM5-8DurableCursorAmendment.md`;
  reports `GwzM5-8DurableCursor-Review{Code,State}.md`). This
  composition position **binds on the property (mode-blind,
  anchor-preserving marker rows), not on D3's internal write edges** —
  D3 text rounds within that property (e.g. the round-1 marker-backfill
  remedy) do not re-open M5b-IF (round-2 State P4). **Amendment-train
  interaction M5b must not alias (State P2-2):** D3 will amend the very
  journal-contract sentences near those §3.3/§5.3 cite (e.g.
  `GwzM5-8I2ActionJournalContract.md:156-166`, "persists neither no-op
  owner skips nor a separate reset completion bit"). M5b's zero-delta claim
  is therefore scoped precisely: **zero *no-ff* deltas** — M5b proposes no
  journal change and no frozen M5b sentence forbids D3's. Non-collision
  argument, recorded as a freeze position: D3's cursor rows are
  **mode-blind** and **anchor-preserving** (per-owner skip rows and a reset
  bit; owner anchors — the participant result — unchanged), so no §5.3
  statement becomes false when D3 lands; conversely nothing in M5b freezes
  the absence of cursor rows. The two pre-A1 trains may land in either
  order: the §7 P-REV order-independence instruction binds M5b-IMPL's
  suites to tolerate the D3 wire landing first, and this paragraph is the
  single authority statement A1 reviewers should read for how the trains
  compose.
- Not a dependency either way: the D1/D2 residuals M5b neither blocks on
  nor closes (§5.4); M5b's proofs must simply not claim Windows
  reverse-path closure.

### 8.3 Ledger row (Q8 — RATIFIED, no ceiling exception)

For `GwzM5-8ChangeBudget.md`, package M5b: production-reachable delta
**0 lines / 0 files** (hard ceiling — everything lands behind `cfg(test)` or
in test files; the only permitted shared-file edits are new `#[cfg(test)]`
items); test/fixture delta ≤ 1,200 lines across ≤ 10 files; wire delta:
none; protocol delta: none; docs: this design + M5b-W1 banner + checkpoint
row. **Ratified by lane-owner adjudication with NO ceiling exception**: the
F-1 production gate is real production lines and lives entirely in the
parallel "v0 forged-action resume gate" package with **its own budget
row** (§3.5a) — it is never absorbed, quietly or otherwise, into M5b's
0-line ceiling; any M5b implementation need to exceed the ceiling still
forces a return to this freeze. Estimation health: prior packages overran
2-10x (`GwzM5-8ProgressReviewF5.md:429`); the 0-production ceiling is the
overrun-proof part, the test budget is the honest risk.

### 8.4 M5b-IF exit criteria (checklist for the two reviewers)

1. §3's zero-new-fields claim verified against the amended contracts and
   the tree, on a freeze tuple re-cut against a clean committed tree
   (State P3-2) — round 1 verified the claim itself on both axes.
2. §4's determinism table confirmed complete: no commit-object input exists
   outside the frozen action (including the scoped no-signing clause, the
   no-re-stamping clause, and §4.6's field-wise-adoption boundary of the
   byte-identity claim).
3. §5's authority rule confirmed against every consumer of
   `PendingMergeAction` in the tree (the freeze fails if any post-freeze
   site re-derives mode from live analysis) — round 1 State traced every
   consumer and confirmed it.
4. §6's boundary list confirmed exhaustive for no-ff **on the v1 side**,
   with exactly one production-reachable consumption lane acknowledged —
   the §3.5a forged-v0 lane, closed by the parallel gate package and bound
   by T-6. Any *other* production-reachable no-ff surface found = NO-GO.
   (Round 1's F-1 found precisely the §3.5a lane while the draft asserted
   it away; the criterion now names it instead of denying it.)
5. §7's suite list accepted as discharging Refactor M5b bullets 2-4 and the
   record-contract exit bullet — including the round-1 additions: the F-2
   clean-true-merge row, the two P2-1 disagreement-window rows, the F-6b
   negative row, and the P-REV order-independence instruction; A1-runnable
   seam requirement (Q5) resolved.
6. The A1-activates list (§3.2) accepted as the exact inverse checklist the
   A1 change will implement — no item may live in both columns.
7. Remaining open questions dispositioned by the program owner: Q1-Q5
   (round-1 lane positions recorded in §9); Q6/Q7/Q8 are already
   dispositioned at this revision (reconciled rule §8.1; class-level
   resolution §5.4; ratified ceiling §8.3) and need only sign-off that the
   recorded texts are the adopted ones.

## 9. Questions for the program owner (round-1 dispositions recorded)

Round-1 review positions and lane-owner adjudications are recorded inline.
Q6, Q7, and Q8 are dispositioned at this revision; Q1-Q5 remain open for
owner sign-off, each carrying its round-1 lane position.

- **Q1 — Amendment necessity.** Is M5b-W1 (§3.4) wanted as a formal record-
  contract amendment (dual-review cost), or is this design document's §4
  citation trail sufficient given `PendingGitSignature.time_seconds` is
  already wire-frozen? Default if unanswered: file the amendment — the
  Refactor's "signatures" wording has already confused one reading, and
  document-only amendments are cheap relative to a wrong A1 reviewer
  assumption. *Round-1 position (Code §7): sound — file it, with the
  F-4/F-6a scope corrections folded into its text (done in this revision).*
- **Q2 — Version-selection function timing.** The
  `max(active_writer_floor, highest_requested_semantic_version)` pure
  function and creation matrix (`GwzM5-8I2CompatibilityContract.md:61-77`)
  are implemented nowhere in this tree. Install it at M5b behind `cfg(test)`
  with the `no-ff → v1` row (so A1's diff is wiring, not construction), or
  keep the whole function in A1's budget? This design assumes A1 (§3.2) but
  the M5b freeze is the last cheap moment to move it. *Round-1 position
  (Code §7): sound as assumed — keep it in A1; symbol absence verified; it
  is activation machinery, not no-ff semantics.*
- **Q3 — Intent grouping.** Keep mode consumption at record level
  (`prepared_mode(record.mode)`, current, zero churn) or extend the R2a
  `IntegrationIntent` group with mode now, anticipating Refactor §6's
  eventual `MergeIntent`? This design freezes the former; changing later is
  internal-only (no wire effect) but touches the frozen R2a seam ownership.
  *Round-1 position (Code §7): sound — record-level matches
  `forward.rs:188-189`; wire-neutral to revisit later.*
- **Q4 — Determinism scope.** Confirm "from-freeze" determinism (pinned
  prepare-time timestamps, §4.2) is the accepted meaning, and that
  cross-operation reproducibility (e.g., deriving signature time from
  `created_at`) is explicitly out — it would be a semantic change to
  already-frozen wire meaning and would decouple commit timestamps from
  reality for no stated requirement. *Round-1 position: AFFIRMED on both
  axes (State §2 "verified in code — affirm"; Code §7 "sound") — awaiting
  only owner sign-off of the affirmed meaning.*
- **Q5 — A1 reuse of M5b proofs.** Require now that P-FWD/P-DET suites be
  parameterized over the lifecycle entry seam so A1's "same next action
  through production dispatch" proofs (`GwzM5-8Refactor.md:2299-2300`) reuse
  them verbatim? Costs a little harness generality inside the existing
  apparatus; buys A1 its regression corpus for free. *Round-1 position
  (Code §7): sound, endorsed — the A1 reuse is real.*
- **Q6 — M5b-IMPL review tier. RESOLVED at this revision** by lane-owner
  adjudication reconciling the two axis positions (Code: mandated dual;
  State: conditional on diff content). The recorded freeze rule — verbatim
  in §8.1 item 2 — is: **mandated-dual by default; single-axis only for
  diffs strictly confined to test modules/files/fixtures
  (`v1_lifecycle/tests/**`, `*_tests.rs`, fixture corpora); any edit to
  non-test items under `v1_lifecycle/{authority,forward,transition,checked}/**`,
  `model/v1/validate/*.rs`, or `git/gitbackend/**` is dual. Recorded at
  freeze, not chosen mid-lane.**
- **Q7 — Windows sentinel. RESOLVED at this revision** (both axes
  affirmed class-level coverage; D1 adopted 2026-08-16): no M5b-specific
  windows-matrix sentinel — the adopted D1 package carries the class-level
  un-pinned CRLF sentinel — plus the one-sentence classification-ledger
  note recorded as an M5b-IMPL obligation in §5.4 (naming that
  `mode: no_ff` makes the commit-arm materialization reachable for
  otherwise-ff-able participants; bookkeeping, not new exposure).
- **Q8 — Budget ratification. RATIFIED at this revision, no ceiling
  exception** (lane-owner adjudication of Code F-1's Q8 interaction): the
  §8.3 ceilings stand as written; the F-1 production gate lives in the
  parallel "v0 forged-action resume gate" package's own budget row and is
  never absorbed into M5b's 0-production-line ceiling; exceeding the
  ceiling still forces a return to this freeze rather than a quiet
  widening.
