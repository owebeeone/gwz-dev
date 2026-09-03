# R2-D Step 2.1 review — LeafObserver production implementation (State axis, escalation)

- **Date:** 2026-08-22
- **Reviewer:** second-axis escalation reviewer, **State axis** (durable-state
  semantics), convened per the adopted plan §4 Phase-2 gate rule after the
  Code-lead round-1 NO-GO (one P1, since remediated). Not peer-blind: this
  review builds on `GwzM5-8R2DStep21-Review.md` and re-verifies rather than
  re-litigates the axes that report settled.
- **Object:** working tree of `gwz-core` (own repo, base **`ae4e143`**):
  NEW `src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs`
  (663 lines — the round-1 object **plus** the post-review platform-routed
  E9 flush `flush_observed_leaf` `:282-322` and its `platform_tests`
  `:605-663`, and the landed Code P3-1/P3-2 remediations `:232-240`/`:191-195`),
  NEW `provider/tests_leaf_observation.rs` (844), NEW
  `provider/tests_leaf_fault_matrix.rs` (497); the Step 2.1 lines of
  `provider.rs` (the 5-line `#[allow(dead_code, …Step 2.4 converts its
  production caller")] mod leaf_observation;` block and the two `#[cfg(test)]`
  mod declarations); and the `durable_leaf` region of
  `interface_tests/fault_expected_keys.rs` (activation flip
  Reserved→Executed `:218-222` with its comment, the `leaf_observation.rs`
  `FAULT_INJECTION_SOURCES` entry, `durable_leaf` in the executed-set
  literal). The tree is live and multi-lane: the 2.2 hunks of the same two
  shared files (`namespace_mutation.rs` wiring, the `namespace` flip) and all
  `namespace/**`/`workspace_ops/**` changes are **other lanes'** and are not
  judged here; live-gate noise is attributed below.
- **Contract:** `GwzM5-8R2DInterfaceFreeze.md` §3.3 (frozen seam — `leaf.rs`
  byte-identical), §3.5 (11 `durable_leaf.*` keys, no minting), §4.1 P2/P5
  rows, §4.3 rows E8-E11; `GwzM5-8R2D-Plan.md` §4 Step 2.1;
  ConsumerCheckpoint §8 :232-237.

## Verdict

**GO.** Findings: **0 P0, 0 P1, 1 P2, 2 P3.**

The headline question this escalation exists for — whether the Windows arm of
`flush_observed_leaf` may substitute a documented no-op for the frozen E9
`sync_all` — is adjudicated **GO**: the substitution's two premises are
code-true on this tree for every writer gwz itself has, the arm's claim
structure is honest about what the P5 anchor does and does not order, and the
no-op is the only implementable arm inside the frozen constraint set. The one
P2 is that the proof's per-platform meaning for leaves gwz did **not** write
lives today only in a `cfg(windows)` comment; the freeze §4.3 E9 annotation
already owed at the settling update (Code round-1 P1-1 remediation) must carry
that caveat as a consumer-facing obligation before Step 2.4 binds the
production caller. Everything else on the state axis — the crash-window
matrix, the two-sided proofs, post-barrier re-derivation, the deviation's
deferral map, and boundedness — holds under attack, with my own re-execution
of the evidence.

## 1. HEADLINE — the Windows durability substitution (E9 no-op arm)

The arm under adjudication, `leaf_observation.rs:302-322`, performs no flush
and argues (`:310-318`):

> "the leaf's *writer* opened it through `durable_write_options`, which sets
> `FILE_FLAG_WRITE_THROUGH` (`directory_mutation.rs`), so the bytes this
> observation reads are already through the cache before the observation
> begins — there is no unflushed writer state for the observer to order. What
> the observer still owes the caller is ordering against what follows, and
> that is edge E10's scheduled namespace barrier, which on Windows is the P5
> anchor round-trip rather than a directory fsync."

### 1.1 Premise (a) — writer trace: code-true for every gwz writer

Every production writer that can put bytes at a leaf this observer can be
pointed at was traced:

| Writer | Open | Flush at write | Verdict |
| --- | --- | --- | --- |
| catalog record writers, `mutation.rs:131/:230` | `durable_write_options` `:377-394` — `custom_flags(FILE_FLAG_WRITE_THROUGH)` on Windows | `sync_all` `:163/:236` on the write handle | durable at write time, doubly |
| catalog directory-staging writers, `directory_mutation.rs:526` | `durable_write_options` `:703-718` (identical twin) | `sync_all` `:561` | same |
| admission record writer, `admission_mutation.rs:404` | `durable_write_options` | `sync_all` `:419` ("NTFS includes metadata") | same |
| legacy source/goal scratch writers, `residue.rs:349-352/:439-442/:510` | plain `create_new` + `write` — **no** write-through | `sync_all` `:377/:477/:526` on the write handle (`GENERIC_WRITE`, legal on Windows) | durable at write time |
| Windows anchor writer, `platform.rs:441-453` | write-through | `sync_all` `:453` | same |
| runtime bootstrap files, `bootstrap/runtime/paths.rs:226-231/:268` | read+write, no write-through | n/a | **outside the observable class** — `runtime.*` subsystem files, never reached through a `RetainedParentV1` in this observer |

So for the class *"leaves gwz wrote"* the premise holds in every crash
interleaving — and holds even stronger than the comment states: gwz writers
do not merely write through, they also `FlushFileBuffers` their own write
handles, so data **and** MFT metadata are on disk before any observation can
begin. A write interrupted mid-`WriteFile` under `FILE_FLAG_WRITE_THROUGH`
leaves a prefix that is itself durable; there is no interleaving in which a
gwz-written leaf presents cached-only bytes to this observer.

**The gap is the complement class.** For a leaf whose bytes were last written
by a *foreign* writer (external process, user tooling) with buffered I/O, the
premise is false, and on Windows nothing in the recipe repairs it: the no-op
arm forces nothing, and the P5 anchor forces the volume's metadata journal
(dirent state), **not** another file's cached data pages. Concrete sequence:

1. Foreign process creates or rewrites leaf `L` with exactly the expected
   bytes, buffered (no write-through, no flush).
2. `observe_exact_durable` on Windows: content matches (`:166-169`), no-op
   flush (`:172`), anchor barrier (`:177` — dirent state now durable),
   post-barrier stability all passes → `ExactDurable` returned (`:245-249`).
3. Power loss before the lazy writer flushes `L`'s pages.
4. Restart: `L` resolves (metadata durable via the log) but its data reads
   zeros past ValidDataLength or stale pages — the returned proof asserted
   durably-exact bytes that were never durable.

On macOS/Linux the identical sequence yields a **true** proof: `fsync` on the
read-only descriptor (`:291-300`) forces the foreign bytes to disk regardless
of who wrote them. The same typed value, `DurableLeafProof::ExactDurable`,
therefore carries a writer-independent claim on Unix and a
writer-conditional claim on Windows.

### 1.2 Premise (b) — what the anchor orders: honest as written

`platform.rs:380-604`: the anchor round-trip is a write-through anchor-file
create/write/`sync_all`/delete/verify on a handle the anchor owns with write
access. Forcing the newest metadata transactions forces the NTFS log's
earlier records with them — a volume-wide **namespace** barrier, the accepted
P5 doctrine (freeze §4.1 P5 row; prior-art reviewed in the durable-cursor and
exact-evidence platform packages). It does not and cannot order another
file's data pages. The Windows arm's comment claims exactly "ordering against
what follows" for E10 and never claims data ordering — the claim structure
matches the mechanism. Note also the seam discipline: this object never
performs the barrier itself (`cross_namespace_barrier` `:326-342` calls only
`namespace.barrier(...)`; zero `private_barrier`/`sync_parent` references),
so E10's Windows discharge is owed by the production `NamespaceProtocol`
(Step 2.2's backend, freeze E10/E14 win cells), not by this file.

### 1.3 Contract position and adjudication

- The frozen seam gives `ExactDurable` **no denotational definition** —
  `leaf.rs:38-46` is bare, and the only doc contract, `leaf.rs:70-73`, is a
  handle-ownership statement ("owns the single retained leaf handle across
  exact proof, flush, namespace barrier, and exact reobservation").
  ConsumerCheckpoint §8 :232-235 defines the recipe operationally
  ("…bounded, fingerprinted, **flushed**, reobserved…").
- The freeze's own §4.3 E9 win cell froze `sync_all`, which round 1 proved
  structurally un-executable on a read-only handle (`FlushFileBuffers`
  requires `GENERIC_WRITE`; pinned by the object's own Windows platform test,
  which asserts os error 5 and self-invalidates if the constraint ever
  lifts, `:646-662`). Some substitution is therefore *forced*; the frozen
  cell cannot be satisfied as written.
- The chosen substitution is the P2 family's already-accepted doctrine
  applied to the family's newest edge — freeze §4.1 P2 win cell: "Windows
  durability is the write-through open plus the anchor barrier in P5" — on
  the exact `sync_parent` precedent (`platform.rs:372-377`), with an in-file
  argument and a pinning test. Within the gwz-writer class the substituted
  proof is exactly as strong as the fsync proof.
- ConsumerCheckpoint §8's "No successful path calls the legacy Windows no-op
  parent sync" is **not** violated: that sentence bans the legacy
  `sync_parent` call pattern; the observer calls neither `sync_parent` nor
  any platform barrier. The new arm is a distinct, documented, doctrine-cited
  no-op — but reconciling checkpoint §8's "flushed" with the E9 Windows arm
  belongs in the settling annotation (P2-1 below).
- The alternatives were correctly rejected: demanding `GENERIC_WRITE` widens
  the observation's access demand and fails on genuinely read-only artifacts
  (round-1's rejected alternative, and the arm's own doc `:288-290`); an
  unbuffered re-read verification arm or a volume flush would be a new
  primitive, which §4.4/§6 forbid this step from minting.

**Ruling: GO on the substitution.** It is sound for every leaf gwz writes —
verified code-true above, in all crash interleavings — and it is the only
implementable arm inside the frozen constraints. For foreign-written leaves
the Windows proof is strictly weaker (content match + namespace ordering,
without data-durability), the arm's comment states the premise as fact rather
than as a class condition, and no contract line a Step 2.4 author must read
carries the caveat — that is finding [P2-1], the annotation-content
obligation, not a code defect. No false proof is reachable in this package:
the observer has no production caller (`provider.rs` `dead_code` block), and
restart re-observation of a data-lost leaf refuses typed
(`LengthMismatch`/`ContentMismatch`), fail-closed.

## Findings

### [P2-1] The Windows proof's writer-class condition is real but recorded only in a `cfg(windows)` comment; the owed freeze §4.3 E9 annotation must state it as a consumer-facing obligation

`leaf_observation.rs:310-315` states the premise as a fact about "the leaf's
*writer*". Verified: true for every gwz writer on this tree (§1.1 table);
false for foreign writers; and consequently `ExactDurable`'s data-durability
component is writer-independent on Unix (fsync arm) but writer-conditional on
Windows (no-op arm). Nothing a consumer author reads — not `leaf.rs` (frozen,
bare), not ConsumerCheckpoint §8 (says "flushed" unconditionally), not the
freeze §4.3 E9 row (still says `sync_all` in its win cell) — records the
divergence; a Step 2.4 author building the production caller on a Unix host
never even compiles the arm that carries the argument.

The remediation vehicle already exists and is already owed: Code round-1
P1-1's accepted remediation ends "Annotate the freeze §4.3 E9 win cell in the
settling update." **This finding binds that annotation's content.** It must
state: (i) the platform split (Unix: handle fsync on the read-only
descriptor; Windows: documented no-op on the P2 doctrine, citing the arm and
its pinning test); (ii) the writer-class condition — on Windows,
`ExactDurable` certifies data durability for leaves written by gwz's own
write paths (`durable_write_options` writers and the legacy `sync_all`
writers), and for bytes last written by any other writer it certifies exact
content plus namespace ordering only; (iii) the reconciliation of
ConsumerCheckpoint §8 :233 "flushed" with (i). Optionally (cheap, in-object
now or at the settle): one clause at `leaf_observation.rs:171` ("the observed
object is durable before the barrier orders it") deferring to the helper's
platform argument, and one clause in the Windows arm reframing the premise
from fact to class condition. Not a P1: no production caller exists, the
false-certificate case needs a foreign writer inside the managed namespace (an
actor with strictly greater direct power) plus a crash plus a
not-yet-written destructive consumer, restart re-derivation refuses
fail-closed, and the settling gate re-reviews against both axis reports —
this is a tracked-debt sharpening, on the P3-3 pattern, promoted to P2
because losing it makes Step 2.4 bind a consumer to an overclaimed proof.

### [P3-1] The two-sided absence proof's window meaning is contract-adequate but its negative space is unstated for consumers

`MissingDurable` for a leaf created **and** removed strictly between the two
sides is correct under the contract as written: ConsumerCheckpoint §8
:234-235 names the proof's shape ("Durable absence uses the matching
two-sided namespace proof"), `observe_durable_absence`'s doc says "Anything
resident on the second side is a fact about the name, not a proof"
(`:252-254`), no two-point protocol can exclude the window, and the
appears-across-the-barrier direction is tested to refuse
(`tests_leaf_observation.rs:418-439`, injected *at* the barrier boundary). But
no consumer-facing line states the negative — that `MissingDurable` does
**not** assert nothing transiently existed — and the named downstream
consumers (duplicate recovery, proof-only completion, checkpoint §8) could
misread two-sided as continuous. **Remediation:** one sentence in the same
settling annotation as P2-1. (The no-replace publication primitive's own
collision refusal, not this proof, is what protects the publish edge — worth
saying in that sentence.)

### [P3-2] Inherited stat-then-open classification race; a raced FIFO can block the observer (liveness, not state)

`open_leaf` classifies from `symlink_metadata` (`:389-397`) and then opens
no-follow (`:399-412`); kind/mode are not re-derived from the opened handle.
Between the two calls a same-kind swap survives to the identity/content
checks (which refuse it), but (i) the `Executable`/`WrongKind` classification
can be computed on the pre-swap object, and (ii) a FIFO landed in the window
blocks a Unix `O_RDONLY` open indefinitely — an unbounded **wait**, though
never an unbounded read or allocation. Mitigations already present: a
resident FIFO/directory/device is refused at the stat before any open (fail
closed — verified by the non-canonical tests on both routes,
`tests_leaf_observation.rs:645-686`), the post-barrier side re-runs the same
stat grammar so a persisting swap is refused there, the shape is byte-for-
byte the legacy observer's own (`observation.rs:210-230` — same stat, same
grammar `:216`, same non-Unix `executable() == false`), and the racer must
already hold write access inside the managed namespace. Inherited accepted
posture, not new unsoundness. **Remediation (optional, downstream):**
re-derive kind/mode from the opened handle's metadata as a second-chance
refusal, or record the posture in the settling update; no change required in
this object.

## 2. Crash-window enumeration (mandate 2)

All eleven boundaries enumerated against the code: `first_open` `:375`,
`first_identity` `:377-379` (announced only when an identity was actually
taken — absence side announces `first_open` alone, correctly),
`first_content`/`content_revalidate` (one announce site per key,
phase-selected at `:491-499`), `file_flush` `:174`, `namespace_barrier`
`:340`, `parent_revalidate` `:350`, `name_revalidate` `:185`,
`handle_revalidate` `:204`, `length_revalidate` `:220`,
`missing_revalidate` `:274`. The observer owns **no durable state**: it
creates, renames, and removes nothing (module doc pinned by the source-scrape
test and by censuses), and its two durable edges are idempotent orderings. An
interruption at any boundary therefore leaves the tree byte-identical, and
the restart's proof is a pure function of tree state — single-valued restart
semantics by construction, and by evidence: the matrix's 14 rows × 2 target
variants assert per row that the interruption genuinely fired, that the
restart re-derives the **identical** proof (`Eq` over identity, length,
sha256), that action-root and catalog-root censuses are unchanged, and that a
third settle changes nothing (`tests_leaf_fault_matrix.rs:399-419`); 2 keys ×
12 rounds × 2 variants of repeated same-boundary crashes hold the row set
stable (`:435-477`). **Re-executed by me on this tree: all four matrix tests
green** (4.76s, appendix). Two scoping notes, neither a finding: the matrix's
barrier is the test protocol's `sync_directory_edge` (real directory fsync on
Unix; the documented P2 no-op on Windows, `directory_mutation.rs:733-742`),
so Windows-native runs exercise the observer's state machine with both edges
discharged as documented no-ops — the production P5 anchor is Step 2.2's
backend, and native fault evidence is already the R2-F release gate (plan §5
item 4); and stale-anchor crash residue inside a real Windows barrier is P5
family territory (E22, Phase 4), not this object's.

## 3. Identity/content/length stability semantics (mandate 3)

All post-barrier facts are live re-derivations, none cached: fresh name open
with identity from the new handle (`:183`), retained-handle identity
re-derived by syscall and compared as the **encoded pair** — refusing
same-name substitution even under durable-id reuse (`:201-207`, tested at
`:469-487` with an injected same-byte substitution) — fresh `metadata()`
length (`:215-223`), second streamed comparison against a second freshly
opened reader (`:228-231`; the never-rewind rule of `leaf.rs:54-56`,
enforced by `opens() == 2`). The proof's `durable` identity is taken from the
first open and is legitimate because identity stability is re-proven across
the barrier. The two landed Code-round remediations are verified sound from
the state side: the `revalidated != first` comparison's new comment
(`:232-240`) is *accurate*, not decorative — under an expectation whose
`len()`/`sha256()` drift between its two `open()` calls, each phase pins to
its own declarations and only the fingerprint comparison catches the drift,
so the check is reachable and substantive; and the three-arm post-barrier
match (`:191-195`) now passes `WrongKind`/`Substituted`/`Executable` through
with `NameChanged` reserved for a name that stopped resolving — mirroring the
absence arm, tested per-fact at `:516-559`. The typed kind/mode facts are
durable-state-sound against the frozen grammar: `LeafOther`'s vocabulary is
frozen (`leaf.rs:8-17`), and `non_canonical` (`:424-434`) reproduces the
legacy grammar exactly (`observation.rs:216`), including `executable() ==
false` off-Unix. The `ContentPhaseV1` announce mirroring gives each of the
two content keys exactly one announce site; a phase mix-up is caught by the
matrix's "fault point was not reached" assertion on the
`content_revalidate` row.

## 4. The recovery-without-re-admission deviation (mandate 4)

The deferral map is real, row by row: **Step 2.4** — plan :347-352 ("Depends
on 2.1"), named verbatim in the `provider.rs` `allow` reason; **Step 3.3** —
plan :379-385 ("wires machinery; it does not convert consumers");
**R2-E** — plan §5 item 1, and the matrix's own comment names it for mid-life
resume ("teaching `resume_or_admit` to tolerate a mid-life action is the
action-lifecycle question R2-E owns", `tests_leaf_fault_matrix.rs:210-215`),
grounded in the §7 exactness predicate (`interior.rs:497-500`). Nothing in
this package silently depends on re-admission working: every attempt runs
`recover(…)` → `recover_or_create` only (catalog recovery, no admission
edge), the retained parent is re-minted per attempt from the live directory
through the `#[cfg(test)]` issuer — the same trust shape a production restart
has (capability re-issued by the owner) — and the admission handoff's
directory identity is proven against the live directory once at `prepare`
(`:308-313`). Decisively for the state axis: the census assertions would fail
if recovery repaired, re-admitted, or otherwise touched the mid-life action
directory, so the evidence itself proves the deviation's premise (recovery
leaves a mid-life action alone) rather than assuming it.

## 5. Boundedness as a state property (mandate 5)

No path lets a hostile leaf force unbounded allocation or read. Bounded
route: fallible `try_reserve_exact(max_bytes)` (`:84-89`),
`take(max_bytes + 1)` with `checked_add` refusal (`:573-580`), over-budget
refused as a typed length fact (`:515-517`). Durable route: nothing
materialised — two fixed 8 KiB stack windows (`:541-542`) against
`take(content.len() + 1)`; the expectation reader is consumed at most
budget + one window because `fill` reads one window per iteration and the
loop ends at observed-side EOF or first divergence. A growing file is cut at
the budget and refused on length; a giant expectation is the caller's own
stated budget (the conflation test drives a payload 4× every protocol record
bound and pins by source scrape that no record kind is named). No
`read_to_end` outside the budgeted bounded route, no `read_to_string`, no
`fs::read`, no `with_capacity`, no `unsafe` (grep-verified). Static
FIFO/device/directory leaves are refused at the stat before any open — fail
closed; the raced-FIFO liveness residue is [P3-2].

## 6. Gates re-executed on this tree (State round 1)

- **Frozen seam:** `leaf.rs` SHA-256 `4193c06f…` identical to `ae4e143`
  (and to the Code round's hash at `c13f773` — unchanged across the rebase).
- **Tests (my run):** `provider::tests_leaf` → **ok. 20 passed; 0 failed**
  (16 observation + 4 matrix, both target variants, 4.76s);
  `provider::leaf_observation` → **ok. 1 passed** (the Unix E9 arm — a real
  `fsync` on the read-only observation handle).
- **Key discipline:** `EXPECTED_KEY_COUNT` 165 unchanged; `fault_v1.rs`
  unmodified from base; all 11 `DurableLeaf*` variants pre-exist at
  `ae4e143`; the 11 `durable_leaf.*` stable keys sit at
  `fault_expected_keys.rs:94-104` exactly as frozen — **no key minted**.
- **Checker (live tree):** findings are the known protected pre_catalog tree
  pin (this object's new files plus 2.2's `namespace_mutation.rs`;
  fail-closed by design; the multi-lane settling package owes the re-pin —
  Code P3-3 stands) and other lanes' files only (`catalog.rs`,
  `workspace_ops/*` trees, allowlist, publication seam — the last matching
  2.2's new publication callers, owned by that step's review). Nothing new
  attributable to this object.
- **Wiring diffs:** `provider.rs` carries exactly the 2.1 mod block + two
  test mods; the `fault_expected_keys.rs` `durable_leaf` region is exactly
  the flip + comment (`:209-222`), the `include_str!` source entry, and the
  executed-literal member.

## Verification appendix (commands, this host, 2026-08-22)

```
git -C gwz-core show ae4e143:src/checked_artifact/leaf.rs | shasum -a 256   # == live file (4193c06f…)
CARGO_TARGET_DIR=<scratch>/rev21s-target cargo test --offline -p gwz-core --lib \
  'checked_artifact::capability::pre_catalog::provider::tests_leaf'         # ok. 20 passed; 0 failed (4.76s)
CARGO_TARGET_DIR=<scratch>/rev21s-target cargo test --offline -p gwz-core --lib \
  'checked_artifact::capability::pre_catalog::provider::leaf_observation'   # ok. 1 passed (unix E9 arm)
python3.13 scripts/checks/check_checked_artifact_boundaries.py --source src # live: pre_catalog pin + other lanes only
git -C gwz-core diff src/checked_artifact/fault_v1.rs                       # empty; 11 DurableLeaf* at base
# writer trace: durable_write_options users + every production sync_all site
grep -rn durable_write_options src/checked_artifact --include='*.rs'
grep -rn sync_all src/checked_artifact --include='*.rs'
```
