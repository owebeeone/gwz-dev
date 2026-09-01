# GwzM5-8 R2-E Record-Root Amendment — the merge record's atomic-replace carve-out

Date: 2026-09-02. Author: the implementation lane (Fable, lane owner).
Status: **ADOPTED 2026-09-02 — dual complete, both axes GO** (round
1: Code GO-WITH-CONDITIONS 1 P2 / 4 P3, 34 claims verified / 0 refuted;
State GO-WITH-CONDITIONS 0 P0 / 0 P1 / 3 P2 / 3 P3; all conditions
folded by the lane owner; round 2: Code axis GO on text-diff
confirmation, zero new-claim flags; State axis waived round 2 on the
folds — both reports verbatim at `-ReviewCode.md` / `-ReviewState.md`).
Lands with E4.3-B's package and the six root-side records (§7). Tier: peer-blind Code+State, Fable×2, per the
refined tier policy's amendment-tier line (checkpoint, 2026-08-22:
Fable at program-level dual gates and amendment tiers); this dual sits
OUTSIDE plan §2's two-dual budget on that line's authority (precedent:
the E5-era "minted by amendment with dual review" determination).
Operator ruling authorizing this direction: 2026-09-01/02, option (c)
of the E4.3 decision packet, verbatim: "proceed with (c)".

Object: ONE dated exception to `GwzM5-8R4bR2ConsumerCheckpoint.md` §10
row `:280`'s "no legacy raw writer" clause, scoped to the merge
record's rewrite path; the pins that make the exception fail-closed;
one new binding clause generalizing [P3-C1] for the remaining E4
conversions; the routing of the rejected alternative into O14's fork.

## 1. The finding this amendment answers (driven, not argued)

E4.3 as chartered converted `v1_lifecycle/store/rewrite.rs::commit` —
the rewrite of the exact existing `MergeStore` record — onto the
checked boundary via `entry::rewrite_merge_store_record` →
`CheckedArtifact::acquire` + `replace_exact(&Bytes(source), goal)`.
The candidate is preserved at gwz-core
`probe/e4-3-detach-window-evidence` (`c9a7303`, full battery green,
366/500 whole lines, never on main). Its own builder self-declared
NO-GO with two driven/structural defects; the lane owner verified both
mechanisms in the tree (2026-09-01/02):

**1a. The record-root crash window.** `replace_exact` on an existing
leaf DETACHES it into the private area (`transition.rs:290-326`), then
publishes the goal no-replace (`transition.rs:391-425`), durable
barriers between. Between those edges the open record does not exist.
Driven at the candidate's row
`an_interrupted_checked_rewrite_detaches_the_record_beyond_the_lifecycles_reach`
(`tests/store.rs:371`), transcript:
`fail_next_checked_artifact_at(BeforeManagedPublication)` on a real
converted commit → `.gwz/merge` EMPTY; `load_open = Err(IoError
NotFound)`; an identical re-issued commit also errs at `read_regular`
(the boundary's `RecoverableDetached` convergence is unreachable — the
`(expected, goal)` pair it needs is derived from the record that is
gone); `classify_open_record` (`merge/store/mod.rs:211-241`)
enumerates `.gwz/merge/*.yaml` and reports **no open merge**;
`gwz merge --abort` cannot discover the id; the three residue files
(`.authority`/`.source`/`.goal`, the `.source` byte-equal to the prior
record) sit in `.gwz/checked-artifacts`, which is
git-status-dirt-exempt (asserted at `policy.rs:33-45`; enforced at
`git/gitbackend/repository_support.rs:122-151`, the untracked-exempt
counting, and `preservation_image.rs:236-248`, the image exclusion) —
durable, invisible, and closed by no shipped reconciler. The retired writer, `rename_durable(temp, path,
replace=true)`, was atomic and had no such window.

**Why this bites exactly one §10 row.** Every other converted leaf is
reconciled FROM the merge record; the record has no outer artifact
above it. The boundary's detach-then-publish shape structurally cannot
serve an artifact that is its own discovery root. (E4.2's create side
was safe: `MissingReplace` never detaches.)

**1b. Abort gains the capability probe.** `commit` is shared by the
activated forward arms and the plain reverse arms
(`service.rs:116-121`), and `CheckedArtifact::acquire` takes a durable
object identity (`observation.rs:96,105`), refusing
`UnsupportedOperation` on a non-admitted filesystem. Under the
conversion, every STANDARD-PATH v1 abort needs an admitted filesystem
(the one corner: an abort of an already-terminal record responds
Terminal before the service loop commits anything, `service.rs:101-108`,
and carries no probe), where today a pre-publication abort constructs
no `CheckedArtifact` at all
(`abort/evidence.rs:39-41`; `rollback_prefix.rs:76-97,155-184`). That
contradicts E0.2 §5.2's frozen capability-free list (abort is on it)
and falsifies the shipped v0.12-lineage remedy sentence
(`capability.rs:45-55`: "`gwz merge --abort`, which needs no such
filesystem").

**1c. The charter defect, owned.** The E4.3 charter carried a jointly
unsatisfiable clause pair — "abort/plain-lease paths untouched and
capability-free" and "the leaf write rides the same `CheckedArtifact`
boundary the converted siblings use" (charter text as issued; this
amendment is its record — the estate holds no other copy) — because
`commit` is one function under both leases. The lane owner's drafting miss, on the
record; the builder followed the mandatory direction and surfaced the
cost instead of forking the store silently, which is the correct
stop-and-report behavior.

## 2. The amended clause

§10 row `:280` reads, verbatim: *"v1 checked store/root/bundle paths |
same purposes and artifact actions | test-gated until A1; no legacy
raw writer"*. This amendment adds ONE dated exception to its third
cell — with the second cell's "artifact actions" read through that
same exception for the one carved-out path (stated inside the
exception) — and nothing else:

> **RECORD-ROOT EXCEPTION (2026-09-02, this amendment, operator-
> authorized option (c)).** The merge record's REWRITE of an exact
> existing leaf — `v1_lifecycle/store/rewrite.rs::commit`, and only
> it — retains `rename_durable(replace=true)` + `sync_dir` as its
> publication primitive. Ground: the record is the ROOT of
> reconciliation — every other artifact recovers FROM it and it
> recovers from nothing IN THE SHIPPED TREE — so the boundary's
> detach-then-publish replacement shape opens a discovery-dead window
> that no shipped reconciler closes (§1a, driven; the Code axis
> enumerated every discovery path — `classify_open_record`,
> `discover_open`, dispatch — all enumerate `.gwz/merge` only, and
> recovery support ignores private residue,
> `recovery_support.rs:360-363`). The residue is content- and
> discovery-sufficient for a reconciler that COULD be built; that is
> §5's option (a), deliberately deferred to O14's fork, not a
> possibility this ground denies. Atomic in-place replacement is the
> correct durability shape for the root as shipped. On this one path
> the publication is deliberately NOT an artifact action: the second
> cell's "artifact actions" clause is read through this same exception
> for the rewrite leaf, and "same purposes" remains binding; every
> other v1 path keeps the full second cell. The exception is otherwise
> an exception to the WRITER-PRIMITIVE clause only: the `:274` rewrite
> row's own clauses ("no parent creation; unknown fields and exact
> reread preserved") and the creation path's full conversion (E4.2)
> are unaffected and remain binding. Acknowledged latent: the raw
> path's `create_temporary` retains an `fs::create_dir_all`
> (`rewrite.rs:171`) that creates nothing in any driven behavior —
> `read_regular` at `:89` proves the record, hence its parent, resident
> — and is race-conditional only against an outside actor removing
> `.gwz/merge` under the held lease; P-2 bounds it (§3), and E4.3-B
> may replace it with a refusal (§7). The exception
> is re-examined at O14's fork (E4.6 chartering) *[2026-09-02: the fork
> lives at DR-1 now — `GwzM5-8R2E-CapabilityFreeAmendment.md` §4/§5]*: if converted leaf
> writes move onto the write authority with a reader-side residue
> reconciliation (§5), the record root converts there or the
> exception is re-affirmed there, deliberately.

Row `:274` ("merge record rewrite | exact existing `MergeStore` | no
parent creation; unknown fields and exact reread preserved") is NOT
textually amended: its three clauses are properties the raw path
already satisfies and pins already hold. Its CONVERSION obligation is
read through the exception above: discharged for the rewrite path by
the exception's record plus §3's pins, not by a boundary routing.

## 3. The pins that make the exception fail-closed (E4.3-B's build)

The exception is enforced, not narrated:

- **P-1, the O13 pin row becomes PERMANENT-DOCUMENTED.**
  `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` keeps
  `store/rewrite.rs: {rename_durable: 2, sync_dir: 2}` with its
  marker changed from retire-on-conversion to
  RECORD-ROOT-EXCEPTION-this-amendment. Fail-closed BOTH directions,
  re-probed: count growth fires (a new raw writer is not blessed);
  count SHRINKAGE also fires (a partial conversion of the carved-out
  path may not land without re-opening this amendment — the red-probe
  direction that used to demand retirement now demands the amendment's
  own revision). Scope of the class: P-1 enforces the `durable_fs`
  writer class; a std::fs-direct writer on the carved path is
  backstopped by the refreshable tree digest *[Record note 2026-09-02 — a correction that was itself WRONG,
  withdrawn the same day: a bracket folded here from the
  capability-free amendment's Code axis ([P2-5]) claimed
  `store/rewrite.rs` is digest-pinned in neither table. The E4.3-B review
  ([P1-1]) drove that false three ways: `PROTECTED_SOURCE_TREE_DIGESTS`
  roots named `mod.rs` digest their whole parent subtree, so
  `workspace_ops/merge/v1_lifecycle/mod.rs`'s digest covers
  `store/rewrite.rs` (main's pinned value reproduces from the tree
  containing it; perturbing it alone moves the digest; the live checker
  fires "protected source tree changed: …/v1_lifecycle/mod.rs" on every
  edit). The sentence above is CORRECT AS WRITTEN: the backstop is that
  TREE digest, property-free, not a per-file entry — `store/rewrite.rs`
  is indeed absent from `PROTECTED_SOURCE_DIGESTS`, which is the per-file
  table the withdrawn bracket conflated with the tree table. P-1's counts
  and P-2's tripwire remain the PROPERTY pins; the digest remains the
  byte backstop.]*
  (`PROTECTED_SOURCE_TREE_DIGESTS`, property-free — every edit is
  noticed, not classified). A property pin for that class is optional
  hardening, not required by this amendment.
- **P-2, the negative tripwire** (house source-scan idiom, CRLF-
  normalized at the read per the f715ddf lesson): a
  `v1_lifecycle` interface test asserting `store/rewrite.rs`'s commit
  path names `rename_durable` and does NOT name the checked boundary
  door (`rewrite_merge_store_record` absent from production sources) —
  the candidate's crash row, inverted into the pin that the window can
  never be reintroduced silently. Builder notes (Code axis [P3-4]):
  the tripwire's own needle names the door, so the absence half needs
  the house self-excluding dodge (a split-string needle, or the seam
  scan's `text[..match.start()] != "fn "` idiom at checker
  `:1178-1186`), or is scoped to the `rewrite.rs` + `entry.rs` shapes
  — the `entry.rs` byte-pin and the four equality inventories
  (`:1299-1381`) already make the door un-addable silently, so the
  tripwire adds the PROPERTY, not the only belt; the CRLF normalize is
  required the moment the needle is region-scoped or multi-line, and
  costs nothing otherwise. P-2 also asserts the latent
  `create_dir_all` stays bounded to `create_temporary`'s shape on the
  commit path (or absent, if §7's refusal option is taken).
- **P-3, the shipped-text truth stands.** `capability.rs:45-55` and
  the E4.1 runbook/release-notes sentences remain TRUE by
  construction (no probe on the shared commit); no text moves.

## 4. New BINDING clause for E4.4-E4.6 (the [P3-C1] class, generalized)

> **PLAIN-LEASE PROBE CLAUSE (2026-09-02).** No E4 conversion may
> place the durable-identity probe (or any admitted-filesystem
> requirement) on a path reachable from the PLAIN `V1MutationLease`
> — the abort/reverse arms — without an explicit dispositive record
> in its own charter: either the shared function is split by lease,
> or the reverse-arm reach is proven structurally absent, or the
> capability-free list is amended at this amendment's tier. Charter
> authors must enumerate the lease-reachability of every function
> they convert BEFORE the build starts. E4.4 is the first bound
> step: `archive_terminal` is reached from abort's `respond()` on the
> plain lease, so the archive conversion must carry its disposition
> in the charter, not discover it at delivery. *[2026-09-02: E4.4 as a
> conversion is GONE and the option "the capability-free list is amended
> at this amendment's tier" is CLOSED by the operator's ruling
> (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`); the clause survives, binding E4.5/6-B — `GwzM5-8R2E-CapabilityFreeAmendment.md` §4.]*

## 5. The rejected alternatives, routed — not lost

Option (a), reader-side residue reconciliation (discovery/recovery
learn to reconcile the record root from the private area), is the
architecturally general cure and is RECORDED AS THE NAMED CANDIDATE at
O14's fork (E4.6 chartering) *[2026-09-02: at DR-1 — `GwzM5-8R2E-CapabilityFreeAmendment.md` §5]*, where the write-authority architecture
is decided with every converted shape in view; the record-root
exception is re-examined there (§2). Option (b), an atomic-replace
publication shape inside the boundary, amends the frozen transition
semantics of the 165-key census and is declined as disproportionate to
one leaf class; it re-enters only as part of (a)'s design if taken.

## 6. The accepted-residual record, re-scoped

The O13 dated accepted-residual (`GwzM5-8R2E-Plan.md` §1.1,
2026-08-27: the v1 store as a production raw durable writer on the
no-ff path, "expires with E4.2/E4.3's landings") re-scopes as follows,
superseding its expiry clause only: the residual EXPIRES for the
creation path at E4.2 (landed `7f28907`) and NARROWS at E4.3-B to
exactly the documented record-root exception of this amendment (plus
`store/archive.rs`/`archive.rs`, which were always E4.4's per the
2026-09-01 C2 correction). What remains FOR THE RECORD-ROOT PATH is no
longer an accepted residual awaiting conversion; it is a pinned,
reviewed exception with its own re-examination point (O14/E4.6). This
sentence does NOT cover the two archive files: `store/archive.rs` and
`archive.rs` keep their accepted-residual, retire-on-conversion marker
in the O13 inventory until E4.4 converts them — E4.4's charter author
may not cite this section against that duty. *[INVERTED 2026-09-02: the
archive files are themselves CARVED OUT permanently by `GwzM5-8R2E-CapabilityFreeAmendment.md` §3
(every writer runs from a listed operation); no conversion duty remains
for E4.4, which does not start; the O13 inventory is permanent-documented
in all three rows.]* O1's close cites this
amendment alongside O14's disposition.

## 7. E4.3-B, chartered by this amendment

On the dual's GO: a small package from main `7f28907`, split by repo.
**gwz-core (the builder's, interior single-axis Opus review):** P-1's
checker re-shape; P-2's tripwire; ONE dated `///` sentence at
`store/rewrite.rs` recording the exception at the file (doc text is
IN-BOUNDS of the no-production-change bound); the stale allow-reason
at `bootstrap/managed.rs:44-47` ("shrink as E4.3-E4.6 convert" →
E4.4-E4.6, since E4.3 now converts nothing — owner: E4.3-B, named);
and the ONE production-code change the bound admits, at the builder's
option: replacing `create_temporary`'s `create_dir_all` with a
refusal (P-1's `durable_fs` counts unaffected; the digest moves). No
other `store/rewrite.rs` production code moves (the raw path never
left main — the candidate was never merged). **gwz-dev root (the lane
owner's, with the landing record):** this document; plan §1.1 O13 —
BOTH C2-bracket sentences (":90 empties across E4.2–E4.4" and
":101-102 do complete that store's conversion") re-annotated for the
carve-out, not just the accepted-residual record; the plan's Phase-E4
preamble universal + a pointer to §4's clause at the E4 gate-note
block (ruling: this amendment is the clause's AUTHORITY, the gate note
its POINTER); plan §1.1 O14 row — the record-root re-examination
agendum at the E4.6 fork; `GwzM5-8R2E-E7-Acceptance.md` §4 — dated
bracket at the O13 row (the O12 nit-1 bracket precedent; the O1 row
recommended); the E4.3 step annotation; the CurrentProgramCheckpoint
position entry carrying the operator's ruling VERBATIM and the tier
recording. The amendment itself carries the dual. The candidate branch
`probe/e4-3-detach-window-evidence` is retained as the driven
evidence; its salvageable rows (the unknown-fields independent-reread
row, re-pointed at the raw path if it strengthens the existing pins)
are the builder's option, inside the cap, not a mandate.

## 8. Review record

**Round 1 (2026-09-02), peer-blind, Fable×2 — both axes
GO-WITH-CONDITIONS; reports verbatim at
`GwzM5-8R2E-RecordRootAmendment-ReviewCode.md` and `-ReviewState.md`.**
Code axis: 1 P2 (the §2 ground overstated "no reconciler CAN close" —
true of the shipped tree, false of what could be built; re-scoped
above with option (a) cross-referenced), 4 P3 (latent `create_dir_all`
acknowledged + bounded by P-2; P-1's `durable_fs`-class scope stated
with the tree-digest backstop; P-2's self-excluding dodge spelled for
the builder; two cite-precision nits folded); 34 claims verified, 0
refuted; all three P-1 fail-closed directions fired under red-probe.
Its battery note is UNFILLED: the reviewing session ended at a harness
restart before the evidence worktree's full `cargo test` completed —
recorded as such, not fabricated; the candidate's own delivery battery
(builder-run, green at `c9a7303`) stands as the evidence tree's record,
and it is not load-bearing for this amendment, whose package carries no
converted code. State axis: 3 P2 (row `:280`'s second cell
"artifact actions" undispositioned — the sentence now sits inside the
exception; §6 overreached the E4.4 archive files — scoped; §7's charter
omitted six records — legs (a)-(f) now named by home), 3 P3 (the tier
citation corrected to the policy's actual line and the two-dual-budget
authority recorded; §1c marked a first filing; the `managed.rs:44-47`
allow-reason owned by E4.3-B); the `:274` reading ruled DEFENSIBLE
without textual amendment, conditional on the second-cell sentence;
corpus sweep 17 hits, 7 must-cure now in §7. Folds executed by the
lane owner 2026-09-02; the State axis waived round 2 on the folds;
the Code axis's round-2 text-diff confirmation follows below.

**Round 2 (2026-09-02), Code axis — GO** (a fresh Fable session
inheriting the round-1 report after the harness restart; text-diff
confirmation only, gwz-core untouched): [P2-1]'s re-scoped ground
"states exactly the two readings round 1 verified … and explicitly
disclaims the one reading round 1 drove false, so the overstatement is
fully cured; and because the exception never needed impossibility,
only an unshipped and non-trivial cure, the ground it stands on is
carried intact"; [P3-2]–[P3-5] each discharged (P3-2 "exceeds the
ask"); the §8 battery recording judged honest and sufficient — the
full battery "would only attest the never-merged candidate's health,
which becomes load-bearing at E4.3-B's own delivery, not here"; every
factual cite in the folded text mapped to round 1's verified set, zero
new-claim flags; two harmless precision notes recorded without cure
(the `!= "fn "` idiom shorthand vs the checker's literal at `:1180`;
"four equality inventories" labeling a range that also covers the
reference-set equalities). Record at `-ReviewCode.md` `:469`. The
State axis's round-1 conditions were executed as lane-owner folds
under its explicit waiver ("On those folds this axis is GO without a
further round"). **THE DUAL IS COMPLETE; THIS AMENDMENT IS ADOPTED.**
