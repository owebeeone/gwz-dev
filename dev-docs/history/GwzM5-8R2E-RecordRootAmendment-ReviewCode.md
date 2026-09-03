# GwzM5-8 R2-E Record-Root Amendment — Dual Review, CODE Axis (Mechanism Truth)

Date: 2026-09-02. Reviewer: Code axis of the peer-blind amendment-tier dual
(Fable). Object under review: `dev-docs/GwzM5-8R2E-RecordRootAmendment.md`
(DRAFT). This axis did not see the State axis.

Trees: LIVE gwz-core main `7f28907` (read-only, no builds — only the
read-only boundary-checker scan was executed there); evidence worktree at
candidate `c9a7303` (`origin/probe/e4-3-detach-window-evidence`, one commit
atop `7f28907`, **not** an ancestor of main — all verified). Probes ran in the
worktree and in a scratchpad copy of the live tree; the worktree was left
**byte-clean** (`git status --porcelain` empty, `git diff HEAD` empty) and the
scratch copy deleted. No commits anywhere. Boundary checker via
`/opt/homebrew/bin/python3.13`.

Method: every load-bearing technical claim in §§1–7 was verified against the
tree at its cited line, adversarially; the §1a crash window was re-driven, and
the claims the candidate's row does NOT assert (error kinds, the re-issued
commit's death point, residue content sufficiency) were driven by a probe test
written for this review and reverted after. P-1's fail-closed directions were
red-probed by execution on a scratch copy of the live tree.

## 1. VERDICT

**GO-WITH-CONDITIONS.** One condition, [P2-1]: a wording-precision fix in §2's
ground sentence (one clause), below. Every mechanism claim the amendment
stands on is verified in the tree — 34 claims verified, 0 refuted (2 verified
with a precision caveat, both graded below). The exception is correctly
scoped, the pins are not merely implementable but are the checker's *existing*
mechanism (all three fail-closed directions fired under red-probe), the §4
clause binds a real hazard verified end to end, and E4.3-B's shape is
coherent against main.

## 2. Findings

### [P2-1] §2's ground sentence overstates: "no reconciler can close" / "it recovers from nothing"

The operative exception clause's ground reads: *"the boundary's
detach-then-publish replacement shape opens a discovery-dead window **no
reconciler can close** (§1a, driven)"* and *"**it recovers from nothing**"*.

Two true readings and one false one coexist in those words:

- **True (verified by enumeration):** no *shipped* reconciler exists. The
  legacy private area has exactly one production enumerator,
  `inspect_family` (`checked_artifact/residue.rs:120`), reachable only inside
  an acquired `CheckedArtifact` action already keyed by
  `(family, action, expected, goal)` — it verifies, it cannot discover.
  `policy.rs`'s `private_parent` has one production consumer
  (`observation.rs:93`). Every open-merge discovery path —
  `classify_open_record` (`merge/store/mod.rs:211-251`, `.yaml`-filtered via
  `record_files` `:419-435`), `discover_open`/`discover_open_before_manifest`
  (`abort/mod.rs:50,:95`; `recovery.rs:18-29`), dispatch (`runtime/
  dispatch.rs:328,:472-477`) — enumerates `.gwz/merge` only. Recovery support
  explicitly *ignores* private residue
  (`git/gitbackend/recovery_support.rs:360-363`). Verified: nothing in the
  tree discovers or closes the window.
- **True (verified in §1a's own scope):** the *lifecycle's* derivation of the
  `(expected, goal)` pair dies at `read_regular` — driven by this review's
  probe: the re-issued commit fails `ErrorCode::IoError` before reaching the
  boundary. §1a's own sentence ("unreachable — the pair it needs is derived
  from the record that is gone") is scoped to the lifecycle's reach and is
  exactly true.
- **False (driven false by this review):** that no reconciler *could* close
  it. The residue is **content-sufficient and discovery-sufficient**: this
  review's probe rebuilt the pair *from the residue alone* — `.source` bytes
  as `expected`, `.goal` bytes as `goal` — re-drove
  `entry::rewrite_merge_store_record`, and the boundary's
  `RecoverableDetached` convergence (`transition.rs:227-228` already-detached
  arm) **completed forward: the record returned at its path with the goal
  bytes and the residue was retired**. Discovery would even be deterministic,
  not heuristic: the `.authority` record encodes `canonical_path_identity`
  (`authority.rs:27,:92,:131`) as length-prefixed path components
  (`identity.rs:166-191`) — the leaf path is recoverable from the residue,
  and both residue bodies decode as v1 records carrying `merge_id`.

The amendment *knows* this — §5 names option (a), reader-side residue
reconciliation, as "the architecturally general cure" and routes it to O14's
fork. The defect is only that the frozen clause text carries the strong
reading. **Condition:** re-word the ground clause to the shipped-tree scope,
e.g. *"a discovery-dead window nothing that ships can discover or close —
the reader-side reconciler that could is exactly §5's option (a), declined
here and routed to O14's fork"* (or equivalent), and align "recovers from
nothing" the same way. One-clause edit; no structural change; the exception's
justification survives it fully, because the honest ground is sufficient:
the window is real, invisible to every shipped surface, and the only cure is
a deferred architecture decision.

**The cheapness question, answered honestly (the mandate's adversarial
core):** the *mechanical* re-drive is cheap — the probe is ~15 lines against
module internals. A *production* reconciler is not: it must (i) live inside
`checked_artifact` (authority decode and the door are module-scoped) as a new
entry-door package moving the byte-pin and reference pins deliberately;
(ii) decide convergence *direction* — forward completion silently lands a
rewrite whose caller already reported failure, backward restore is
inexpressible against the open authority's family (a different action on the
same family classifies foreign and refuses — verified at
`residue.rs:166-206`, `ensure_authority:242-248`); (iii) choose a hook —
every natural host (classify/status/abort discovery) is on the capability-free
list, and the reconciler itself needs the admitted-filesystem probe, so
placement re-opens exactly the §1b class the amendment exists to avoid; and
(iv) generalize across families sharing the private area. That is a design
step — §5's option (a) — not a teach. The amendment's *routing* of it to
O14's fork is the correct disposition; only the clause wording needs the fix.

### [P3-2] The raw commit path retains a latent `fs::create_dir_all` the §2 justification doesn't mention

`store/rewrite.rs::create_temporary` (`:167-186`, live) still calls
`fs::create_dir_all(parent)` (`:171`) and `commit` reaches it (`:103`). In
every driven behavior the parent pre-exists (`read_regular` at `:89` proved
the record resident), so it creates nothing — but if an outside actor removes
`.gwz/merge` between `:89` and `:103`, the raw path would *re-create the
parent and publish into it*, a corner-case breach of row `:274`'s "no parent
creation". §2's justification sentence — *"its three clauses are properties
the raw path already satisfies and pins already hold"* — is true of driven
behavior and of the unknown-fields/exact-reread clauses (rows at
`tests/store.rs:36,:260` on main), but for "no parent creation" the property
is race-conditional and **no pin names the primitive** (see [P3-3]: only the
refreshable tree digest notices). §7's "NO change to `store/rewrite.rs`
production code" then forecloses the one-line cleanup (refuse instead of
create) in the same breath. Recommend either a one-line acknowledgment in §2
(latent, race-only, lease-covered) or extending P-2's scan to assert
`create_dir_all` stays absent from / bounded on the commit path. Not
operative-clause text; P3.

### [P3-3] "A new raw writer is not blessed" is enforced for the `durable_fs` class only — std::fs-direct writers ride through

Red-probed on a scratch copy of the live tree: adding a
`std::fs::rename`-based function to `store/rewrite.rs` produced **no** O13
finding and no writer-class finding — only `protected source tree changed:
workspace_ops/merge/v1_lifecycle/mod.rs`, the subtree-digest "go look" bump
(`source_tree_digest` `:1025-1042`, enforcement `:1101-1103`), which the
checker's own comments say states no property and is refreshed on every
legitimate edit. P-1 counts only `rename_durable`/`rename_noreplace`/
`sync_dir` tokens on masked source, gated on the file naming `durable_fs` at
all (`:1188-1196`); P-2 as designed would also pass. This is **not** a gap
the amendment creates — O13's pin was always `durable_fs`-scoped, and the
E4.2 review's flag-6 ground 2 adjudicated "no legacy raw writer" as meaning
exactly `durable_fs` — but the amendment's "fail-closed" language inherits
the scope silently. Optional hardening for E4.3-B (inside P-2's budget):
assert commit's publication call shape (`rename_durable(&temporary, path,
true)`) uniquely, or count `std::fs::rename`/`fs::rename` at zero in the
file. P3, disclosure-grade.

### [P3-4] P-2's "absent tree-wide" needs one standard dodge the text doesn't spell

The tripwire's own needle must *name* `rewrite_merge_store_record`, so a
literal tree-wide-absence scan meets the tripwire itself; "present only in
this amendment's quotes" covers dev-docs, not the pin's own source. The house
has the dodges (split-string needle; the seam scan's own
`text[..match.start()] != "fn "` self-exclusion idiom at checker
`:1178-1186`); and the belt already exists regardless — the door cannot be
added to `entry.rs` without tripping the byte-pin
(`PROTECTED_SOURCE_DIGESTS`), the visible-item/import/call-graph equality
inventories (`:1299-1332`), and the per-symbol + module-user reference-set
equalities (`:1335-1381`), all verified in the checker's enforcement code.
The CRLF requirement is confirmed real and correctly cited: `f715ddf` is
exactly the normalize-at-read lesson (a multi-line needle missed a
CRLF-materialized checkout; `.gitattributes` pins `eol=lf` only for
`scripts/retained_readers/**` and `protocol/**`), and it becomes load-bearing
for P-2 the moment the needle is region-scoped or multi-line; for bare
single-token needles it is the house idiom at zero cost. Builder note; P3.

### [P3-5] Two cite-quality notes

(i) The dirt-exemption cite `policy.rs:33-45` points at the *asserting
comment*; the enforcing code is `git/gitbackend/repository_support.rs:122-151`
(`checked_artifact_private_status_path`, untracked-exempt counting) plus
`preservation_image.rs:236-248` (image exclusion). The claim itself is
verified true. (ii) §1b's "EVERY v1 abort needs an admitted filesystem" has a
pedantic corner under the candidate: an abort of an *already-terminal* record
can respond Terminal before the service loop commits anything
(`service.rs:101-108` → `respond` → archive on the raw E4.4 writers), so it
would carry no probe. The substantive claim — the standard abort path breaks
— is verified. Both are text-precision nits; P3.

## 3. The §1a crash window — re-driven, every consequence verified

The candidate's row
`an_interrupted_checked_rewrite_detaches_the_record_beyond_the_lifecycles_reach`
(worktree `tests/store.rs:371`) re-ran green in the evidence worktree:

```
test result: ok. 1 passed; 0 failed; ... 1831 filtered out; finished in 0.04s
```

Because the row does not assert everything §1a claims, this review appended
its own probe (reverted after; source preserved in §7 below), which **passed**:

```
test result: ok. 1 passed; 0 failed; ... 1832 filtered out; finished in 0.12s
```

Driving, in one pass on a real converted commit with
`fail_next_checked_artifact_at(BeforeManagedPublication)`:

| §1a claim | Result |
| --- | --- |
| `.gwz/merge` EMPTY between the durable edges | **verified** (probe assert) |
| `load_open = Err(IoError NotFound)` | **verified**: `ErrorCode::IoError`, message NotFound-shaped (probe assert on code + text) |
| identical re-issued commit errs at `read_regular` | **verified**: `ErrorCode::IoError` before any boundary call |
| `classify_open_record` reports no open merge | **verified structurally**: `record_files` filters `*.yaml` under `.gwz/merge` (`:419-435`), probe proved the directory empty, `:240-241` maps 0 → `Ok(None)`; stray `.v1.tmp` staging names are outside the filter |
| `--abort` cannot discover the id | **verified structurally**: every discovery path enumerates `.gwz/merge` only (cites in [P2-1]) |
| three residue files `.authority`/`.source`/`.goal`; `.source` byte-equal to the prior record | **verified** (probe asserts; `.goal` additionally byte-equal to the intended rewrite — the fact behind [P2-1]) |
| residue git-status-dirt-exempt | **verified enforced** at `repository_support.rs:122-151`; also preservation-image-blind (`preservation_image.rs:237`) |
| detach at `transition.rs:290-326`, publish at `:391-425`, durable barriers between | **verified**: detach rename `:290-301`, `sync_private`/`sync_dir` `:303-312`, post-detach exact reobservation `:313-325`, the `BeforeManagedPublication` fault point `:386-390` sits after all detach durability and before the goal rename `:391-402` |
| "E4.2's create side was safe: never detaches" | **verified**: `replace_exact` calls `detach_existing` only for `RetainedSource::Existing` (`transition.rs:91-92`); expected-`Missing` creates take the `:93-103` arm |
| retired writer atomic, no window | **verified**: see §4 below |

**The adversarial answer:** no existing production or recovery path can
reconcile the record from the residue — verified by enumerating every reader
and discovery path (listed under [P2-1]). A reconciler is **constructible**
from the residue alone and this review built one as a probe (it converged);
it is not *cheap* as a production artifact for the four reasons given in
[P2-1], and it is exactly §5's option (a), correctly routed to O14's fork.
The amendment's ground **holds** once the [P2-1] wording is scoped to the
shipped tree. Two mitigating discoveries, for the record: (a) the residue is
not perfectly inert — a later checked action on the *same leaf path*
classifies the stale family foreign and refuses
(`residue.rs:166-206`/`ensure_authority`), so a same-id re-create would be
refused typed rather than silently colliding; production merge ids are
operation-scoped (`dispatch.rs:491-516`, `merge_<opid>_<n>`), so no practical
collision arises; (b) this strengthens, not weakens, the rejection of the
conversion.

## 4. §1b abort reach and the cure — verified structurally on the live tree

- **(i) `commit` under BOTH leases:** `service.rs:116-121` — only
  `ResumeStart | Continue` take `acquire_activated`; `Abort`, `Preserve`,
  `Status`, `Archive` (`dispatcher.rs:17-24`) take plain `acquire`. The one
  run loop calls `store.commit(&lease, ...)` at `:140` and `:181` under
  whichever lease it holds. Verified.
- **(ii) the boundary's acquire probes identity:** `observation.rs:96`
  (root) and `:105` (parent) call `durable_identity` →
  `identity::object_identity`, refusing `ErrorCode::UnsupportedOperation`
  (`:363-365`, `:376-380`). Verified.
- **(iii) pre-publication abort constructs no `CheckedArtifact` TODAY:**
  `abort/evidence.rs:39-41` — `record.publication` `None` returns `Ok(())`
  before any evidence machinery; `rollback_prefix.rs:76-97` gates the
  evidence observation on candidate+composition_commit+rolled_back;
  `:155-184` gates the selected-root arm on an `@root` rollback. Verified on
  live.
- **Post-publication aborts already reach the boundary:**
  `evidence.rs:82-88` observes and `:148-160` writes through
  `root::artifact_facts` → `entry::{observe,replace,remove}_merge_root_artifact`
  → `CheckedArtifact::acquire`. The carve-out leaves their capability posture
  exactly as it stands. Verified. (A pre-existing edge, not this amendment's:
  a post-publication record physically relocated to a non-admitted filesystem
  already could not complete evidence rollback — E4.1-vintage,
  `OperationModel.md:147-149`'s "always be cleared" carries it; unchanged in
  both directions by this amendment.)
- **§5.2's frozen list carries abort:** confirmed verbatim in
  `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §5.2 ("… abort, GC and the
  mutation guard continue to work").

**The cure, by construction (live tree):** `commit` stages its temporary as a
*sibling* (`create_temporary`, `path.with_extension(...v1.tmp)` — outside the
`.yaml` filter), then `rename_durable(&temporary, path, true)` (`:132`) →
unix `std::fs::rename` (`durable_fs.rs:45-51`), the atomic-replace rename(2):
the name `.gwz/merge/<id>.yaml` resolves to old or new bytes at every
instant — **no absence window** — then `sync_dir` (`:139`). The path contains
**no identity call of any kind** (`fs` metadata + `canonicalize` + file I/O
only; `durable_fs.rs` is rename/fsync only). So the carve-out cures §1a (no
detach, record never absent) and §1b (no acquire, no probe; abort's plain-
lease commits stay capability-free) **by construction of the primitive**, as
claimed. Verified.

## 5. The pins (§3) — verified, with all fail-closed directions fired

- **P-1 is the existing mechanism, not new work.** The live map row
  `store/rewrite.rs: {rename_durable: 2, sync_dir: 2}` (checker `:345-349`)
  matches the live file exactly (use `:6` + calls `:132`,`:139`), counted on
  `mask_non_code` text (comments/quotes do not count, `:1190-1196`). The
  enforcement (`:1197-1214`) already fires on: set growth ("gained a raw
  durable_fs writer outside the O13 accepted residual"), any count move in
  either direction ("count moved and must move the pin with it", a `!=`
  comparison), and full retirement ("must be retired from the pin
  deliberately"). **All three fired under this review's red-probes on a
  scratch copy of the live tree** (transcripts §7): deleting the `sync_dir`
  call → count-moved with `actual={'rename_durable': 2, 'sync_dir': 1}`;
  stripping the writers → retirement arm; a new writer file → growth arm.
  E4.3-B's remaining work is the marker comment. Both-directions
  fail-closure: **verified by execution.** (Scope limit recorded as [P3-3].)
- **P-2 is sound as designed.** `rewrite_merge_store_record` has **zero**
  matches across the live tree (rs+py+docs). The house source-scan idiom
  (`include_str!` + `.replace("\r\n", "\n")`) exists at
  `interface_tests/capability_permit.rs:485`, `r2d_seam_freeze.rs:221-223`,
  `git/tests/g01.rs:261-270`; the `f715ddf` cite is confirmed the CRLF
  lesson verbatim. Belt-and-braces: even without P-2, silent re-routing is
  caught by the entry.rs byte-pin, the four ENTRY_* equality inventories, and
  the per-symbol/module-user reference-set equalities (`:1295-1381`) —
  `CheckedArtifact::acquire` is module-private, so v1_lifecycle can reach the
  boundary only through entry.rs doors. Implementation notes at [P3-4].
- **P-3 verified.** `capability.rs:45-55` is quoted accurately and is TRUE on
  the live tree; `OperationModel.md:144-159` carries the runbook sentences.
  Under the carve-out no probe lands on the shared commit, so every truth
  value those sentences have today is preserved identically — "remain TRUE by
  construction" is exact w.r.t. what the carve-out controls (the pre-existing
  post-publication relocation edge stands on both sides, §4 above). The
  candidate would have falsified them wholesale; verified that too (§4(i-ii)
  composition).

## 6. §2 scope, §4 premise, §6, §7 — verified

- **§2 tightness (adversarial):** the exception names one function in one
  file and one primitive pair, "and only it"; the row's other clauses and
  E4.2's conversion are explicitly kept binding; `:274` is left textually
  unamended with its conversion obligation read through the exception.
  Attempted stretchings fail: a new raw `durable_fs` writer → P-1 growth arm
  (fired in probe); a boundary re-route of `commit` → P-2 plus the reference-
  set equalities; the create path → explicitly excluded; raw *reads* were
  never in the writer clause's adjudicated referent (E4.2 review flag-6
  ground 2: "no legacy raw writer" means `durable_fs`). Residual soft spots
  are [P3-2] (latent `create_dir_all`) and [P3-3] (std::fs class), both
  disclosed above. **Nothing else in v1_lifecycle writes the record leaf**:
  exhaustive primitive scan of production v1_lifecycle finds writers only in
  `store/rewrite.rs` (create converted / commit carved) and the two E4.4
  archive files (`store/archive.rs` moves open→done; `archive.rs` sync_dirs)
  — the terminal-archive row, correctly outside this exception.
- **§4's factual premise is real, end to end:** `MergeOp::Abort` →
  `V1LifecycleRequest::Abort` → plain lease → disposition → `respond`
  (`start.rs:151-170`, `:107-125`) → Terminal/ArchiveReady →
  `archive::archive_terminal` (`archive.rs:70-104`) →
  `service::run(..., V1LifecycleRequest::Archive, ReverseRuntime)` → plain
  lease again (`service.rs:120`) → the physical archive on the two raw E4.4
  writer files (`store/archive.rs` `rename_noreplace`/`sync_dir`,
  `archive.rs` `sync_dir` — exactly the two rows remaining in the O13 map).
  Converting them onto the boundary would put `acquire`-class probes on this
  abort-reachable plain-lease path. The clause binds a real hazard; its E4.4
  sentence is a true premise, and it generalizes E4.1's carried [P3-C1]
  (plan `:420-422`) faithfully.
- **§6 re-scope:** matches the plan verbatim — O13's dated accepted-residual
  (`GwzM5-8R2E-Plan.md:102-108`, "expires with E4.2/E4.3's landings") and the
  2026-09-01 C2 correction (`:88-102`, archive files are E4.4's; E4.2 landed
  `7f28907` — confirmed at the tree). The narrowing-to-exception supersedes
  only the expiry clause; consistent.
- **§7 coherence:** main's `store/rewrite.rs` carries the raw commit
  (verified by read); `c9a7303` is not an ancestor of main and sits only on
  `probe/e4-3-detach-window-evidence` (verified) — so "NO change to
  `store/rewrite.rs` production code" is coherent: the raw path never left
  main. The candidate's 366 inserted lines match "366/500 whole lines". The
  salvage note is textually the builder's option ("not a mandate").
  The checker at both trees: live **ok (18 visible entries)**, candidate
  worktree **ok (19)**. Out-of-tree attestations this axis cannot verify and
  does not grade: the operator ruling quote ("proceed with (c)"), the
  builder's self-declared NO-GO, and §1c's ownership attribution (the
  *contradiction* itself is mechanism-verified: both charter horns are real
  standing constraints — the E0.2 §5.2 list plus E4.1's [P1-1] lease split on
  one side, row `:280`'s "same … artifact actions" plus the checkpoint's
  E4.3 launch direction on the other — and `commit` is one function under
  both leases).

## 7. Probe transcripts

**(a) Candidate row re-run (worktree, candidate `c9a7303`):**
```
cargo test an_interrupted_checked_rewrite_detaches
test result: ok. 1 passed; 0 failed; ... 1831 filtered out; finished in 0.04s
```

**(b) Review probe (appended to worktree `tests/store.rs`, run, then
reverted; worktree verified byte-clean after):** asserts, in order: merge dir
empty; `load_open` → `ErrorCode::IoError` with NotFound-shaped text;
re-issued commit → `ErrorCode::IoError` (dies at `read_regular`); residue
exactly 3 (`.authority`/`.source`/`.goal`); `.source` byte-equal prior
record; both `.source` and `.goal` decode with `merge_id: merge_1`
(content-sufficiency); then rebuilds `(expected, goal)` from the residue
alone and re-drives `entry::rewrite_merge_store_record` → convergence: record
restored at its path with the goal bytes, `load_open` ok, residue retired.
```
cargo test review_probe_crash_window
test result: ok. 1 passed; 0 failed; ... 1832 filtered out; finished in 0.12s
```

**(c) P-1 red-probes (scratch COPY of the live tree; live tree untouched):**
```
baseline:               checked-artifact boundary: ok (18 visible entries, 8 classified modules)
shrinkage (sync_dir 2→1):
- O13 raw-writer count moved and must move the pin with it:
  workspace_ops/merge/v1_lifecycle/store/rewrite.rs:
  expected={'rename_durable': 2, 'sync_dir': 2} actual={'rename_durable': 2, 'sync_dir': 1}
full retirement + new writer file:
- v1 lifecycle gained a raw durable_fs writer outside the O13 accepted residual:
  workspace_ops/merge/v1_lifecycle/probe_growth.rs
- O13 accepted-residual entry no longer names durable_fs and must be retired
  from the pin deliberately: workspace_ops/merge/v1_lifecycle/store/rewrite.rs
std::fs::rename evasion probe ([P3-3]):
- protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs
  (no O13 finding; the go-look digest only)
```

**(d) Boundary checker, both real trees (read-only):** live `7f28907` →
`ok (18 visible entries)`; candidate worktree `c9a7303` →
`ok (19 visible entries)`.

**(e) Full battery, evidence worktree:** see the dated note at the end of
this report.

## 8. Per-claim table

| # | Claim (cite as given in the amendment) | Status |
| --- | --- | --- |
| 1 | detach into private area, `transition.rs:290-326` | verified |
| 2 | goal published no-replace, `transition.rs:391-425` | verified |
| 3 | durable barriers between the edges | verified (`:303-312`) |
| 4 | driven row at `tests/store.rs:371` | verified, re-run green |
| 5 | `.gwz/merge` EMPTY after interrupt | verified by probe |
| 6 | `load_open = Err(IoError NotFound)` | verified by probe |
| 7 | re-issued commit errs at `read_regular` | verified by probe |
| 8 | `RecoverableDetached` convergence unreachable *from the lifecycle* | verified (probe + `transition.rs:55`) |
| 9 | "no reconciler can close" / "recovers from nothing" (§2 ground) | verified for the shipped tree; overstated as absolute — **[P2-1]**, reconciler driven constructible |
| 10 | `classify_open_record` enumerates `.gwz/merge/*.yaml`, reports none (`mod.rs:211-241`) | verified |
| 11 | `--abort` cannot discover the id | verified |
| 12 | 3 residue files, `.source` byte-equal | verified by probe |
| 13 | residue dirt-exempt (`policy.rs:33-45`) | verified true; enforcing code elsewhere — [P3-5](i) |
| 14 | retired writer atomic, no window | verified (`durable_fs.rs:45-51`, rename(2)) |
| 15 | every other converted leaf reconciled FROM the record | verified (root/marker/bundle doors keyed by record state; record has no outer artifact) |
| 16 | E4.2 create side never detaches | verified (`transition.rs:91-103`) |
| 17 | `commit` shared under both leases (`service.rs:116-121`) | verified |
| 18 | acquire probes durable identity (`observation.rs:96,105`) | verified |
| 19 | refuses `UnsupportedOperation` on non-admitted fs | verified (`:363-380`) |
| 20 | pre-publication abort constructs no `CheckedArtifact` (`evidence.rs:39-41`; `rollback_prefix.rs:76-97,155-184`) | verified on live |
| 21 | post-publication aborts already reach the boundary; posture unchanged by carve-out | verified |
| 22 | contradicts E0.2 §5.2 list (abort on it) | verified (list quoted in source doc) |
| 23 | falsifies `capability.rs:45-55` sentence (under conversion) | verified by composition |
| 24 | §1c charter horns jointly unsatisfiable | verified mechanically (one function, both leases); ownership attribution out-of-tree |
| 25 | carve-out cures §1a AND §1b by construction | verified (§4 of this report) |
| 26 | P-1 keeps `{rename_durable: 2, sync_dir: 2}`; both directions fail closed | verified; **all three arms fired under red-probe** |
| 27 | P-2 scan sound; door absent tree-wide; CRLF per f715ddf | verified (0 matches; commit confirmed); implementation notes [P3-4] |
| 28 | P-3 texts true and preserved under carve-out | verified (`capability.rs:45-55`; `OperationModel.md:144-159`) |
| 29 | §4 premise: `archive_terminal` from abort's `respond()` on plain lease; E4.4 conversions would probe it | verified end to end |
| 30 | §2 exception scope: nothing beyond commit's publication primitive; "and only it" enforceable | verified with [P3-2]/[P3-3] disclosed |
| 31 | only create_open (converted) and commit (carved) write the record leaf in v1_lifecycle | verified (archive move = E4.4's row) |
| 32 | §6 re-scope matches O13's dated record + C2 correction | verified against the plan |
| 33 | §7 no-production-change coherent; candidate never on main; 366/500 | verified |
| 34 | salvage note optional-not-mandated | verified (text) |

Refuted: none. Precision caveats graded: #9 → [P2-1]; #30's justification
sentence → [P3-2].

## 9. Conditions and notes, consolidated

- **Condition [P2-1]** (the GO-WITH-CONDITIONS): re-scope §2's ground
  sentence ("no reconciler can close" / "recovers from nothing") to the
  shipped tree, cross-referencing §5's option (a) as the deliberately
  deferred cure. One clause; no mechanism changes.
- [P3-2] acknowledge (or pin) the latent `create_dir_all` on the carved
  path.
- [P3-3] optional P-2 hardening against the std::fs writer class; at minimum
  the amendment may cite the tree-digest go-look as the current (property-
  free) backstop.
- [P3-4] builder note: implement the tree-wide half of P-2 with the standard
  self-excluding/split-needle dodge or scope it to
  `rewrite.rs`+`entry.rs` shapes, which the equality pins already complete.
- [P3-5] two cite-precision nits (dirt-exemption enforcing code; "EVERY v1
  abort" terminal-record corner).

Round 1 of the two-round cap. On a fold of [P2-1] this axis expects round 2
to be a text-diff confirmation only.

---

*Battery note (appended when the run completed): the full `cargo test`
battery in the evidence worktree at `c9a7303` — result recorded below by the
reviewing session before filing.*

---

## ROUND 2 — Code axis confirmation

Date: 2026-09-02. Reviewer: Code axis, round 2 (Fable), inheriting the
round-1 report above verbatim — the round-1 session ended at a harness
restart before its battery note was filled; this round finishes that round.
Object: `dev-docs/GwzM5-8R2E-RecordRootAmendment.md` as folded 2026-09-02
(§8 records the folds). Mandate: text-diff confirmation only. Read-only
throughout — no probes, no builds, no commits.

Read-only facts established for this round: gwz-core HEAD is `7f28907`
(round 1's live tree; `git status --porcelain` empty; `c9a7303` still not
an ancestor of HEAD). The amendment and both review reports are untracked in
gwz-dev, so no committed pre-fold version exists to diff against — the
confirmation is taken against round 1's own verbatim quotations of the
original clause text ([P2-1] quotes "no reconciler can close" / "it recovers
from nothing"). Two identifiers the folded text names that round 1 did not
name literally were grepped on the live tree (adversarial glance, below).
Nothing else was touched.

### Verdict: GO

Every round-1 item is discharged by the folded text; no fold fails to cure;
no fold introduces a false claim or a mechanism claim outside round 1's
verified set. No conditions. The two-round cap closes for this axis.

### Per-item table

| Item | Folded text (quoted) | Discharged? |
| --- | --- | --- |
| [P2-1] §2 ground overstated | §2 blockquote: *"every other artifact recovers FROM it and it recovers from nothing IN THE SHIPPED TREE — so the boundary's detach-then-publish replacement shape opens a discovery-dead window that no shipped reconciler closes (§1a, driven; the Code axis enumerated every discovery path — `classify_open_record`, `discover_open`, dispatch — all enumerate `.gwz/merge` only, and recovery support ignores private residue, `recovery_support.rs:360-363`). The residue is content- and discovery-sufficient for a reconciler that COULD be built; that is §5's option (a), deliberately deferred to O14's fork, not a possibility this ground denies. Atomic in-place replacement is the correct durability shape for the root as shipped."* §1a's closing now reads *"durable, invisible, and closed by no shipped reconciler."* | **YES — fully cured, ground not weakened.** Both true readings round 1 verified are now the text (no *shipped* reconciler; the enumeration is round 1's own, cite for cite), and the one reading round 1 drove false (none *could* be built) is explicitly disclaimed and routed to §5(a)/O14. The ground the exception needs was never impossibility — it is that the window is real, invisible to every shipped surface, and curable only by a deferred architecture decision; the folded sentence carries exactly that, and now pins it with the enumeration. "as shipped" on the durability-shape sentence keeps the re-scope consistent. §1a's unchanged parenthetical ("unreachable — the pair it needs is derived from the record that is gone") stays exactly true in its lifecycle scope (round 1's second true reading); its new tail aligns it with §2. |
| [P3-2] latent `create_dir_all` | §2: *"Acknowledged latent: the raw path's `create_temporary` retains an `fs::create_dir_all` (`rewrite.rs:171`) that creates nothing in any driven behavior — `read_regular` at `:89` proves the record, hence its parent, resident — and is race-conditional only against an outside actor removing `.gwz/merge` under the held lease; P-2 bounds it (§3), and E4.3-B may replace it with a refusal (§7)."* §3 P-2: *"P-2 also asserts the latent `create_dir_all` stays bounded to `create_temporary`'s shape on the commit path (or absent, if §7's refusal option is taken)."* §7: *"the ONE production-code change the bound admits, at the builder's option: replacing `create_temporary`'s `create_dir_all` with a refusal (P-1's `durable_fs` counts unaffected; the digest moves)."* | **YES — exceeds the ask.** Round 1 asked for the acknowledgment OR the P-2 extension; the fold does both and opens the §7 door round 1 noted was foreclosed. Every clause is round-1-verified: `:171`/`:89` re-confirmed on live this round; `create_dir_all` is not a P-1 token (counts unaffected); the tree digest moves on any edit. The acknowledgment sits inside the blockquote beside the `:274` "no parent creation" clause it qualifies — the right place for a disclosure. |
| [P3-3] P-1 scope = `durable_fs` class | §3 P-1: *"Scope of the class: P-1 enforces the `durable_fs` writer class; a std::fs-direct writer on the carved path is backstopped by the refreshable tree digest (`PROTECTED_SOURCE_TREE_DIGESTS`, property-free — every edit is noticed, not classified). A property pin for that class is optional hardening, not required by this amendment."* | **YES.** The silently-inherited scope round 1 flagged is now stated; the tree-digest go-look is cited as the current, property-free backstop in round 1's own terms ("states no property … refreshed on every legitimate edit"); the hardening stays optional, as recommended. Identifier confirmed (below). |
| [P3-4] P-2 self-trip dodge | §3 P-2: *"the tripwire's own needle names the door, so the absence half needs the house self-excluding dodge (a split-string needle, or the seam scan's `text[..match.start()] != "fn "` idiom at checker `:1178-1186`), or is scoped to the `rewrite.rs` + `entry.rs` shapes — the `entry.rs` byte-pin and the four equality inventories (`:1299-1381`) already make the door un-addable silently, so the tripwire adds the PROPERTY, not the only belt; the CRLF normalize is required the moment the needle is region-scoped or multi-line, and costs nothing otherwise."* The absence claim itself is now *"`rewrite_merge_store_record` absent from production sources"*. | **YES — accurate to the finding, point for point** (self-exclusion, split needle, scope-to-shapes, existing belts, CRLF condition). Two harmless precision notes, no cure required: (i) `text[..match.start()] != "fn "` is round 1's own shorthand for the checker's literal `text[max(0, match.start() - 3) : match.start()] != "fn "` (`:1180`) — the builder reads the checker; (ii) `:1299-1381` is the union of round 1's `:1299-1332` (the four ENTRY_* inventories, `entry.rs` read at `:1299`, call-graph finding closing at `:1332`) and `:1335-1381` (per-symbol/module-user reference-set equalities, `allowed_entry_users` finding at `:1378-1379`) — the label names the first, the range covers both belts; both are round-1-verified. The narrowing to "production sources" additionally removes the tripwire's own test file from the scan's referent. |
| [P3-5] two cite nits | §1a: *"(asserted at `policy.rs:33-45`; enforced at `git/gitbackend/repository_support.rs:122-151`, the untracked-exempt counting, and `preservation_image.rs:236-248`, the image exclusion)"*. §1b: *"every STANDARD-PATH v1 abort needs an admitted filesystem (the one corner: an abort of an already-terminal record responds Terminal before the service loop commits anything, `service.rs:101-108`, and carries no probe)"*. | **YES.** Both are round 1's cites verbatim; the "EVERY" is now scoped to the standard path with the terminal-record corner named. |
| §8 battery note | *"Its battery note is UNFILLED: the reviewing session ended at a harness restart before the evidence worktree's full `cargo test` completed — recorded as such, not fabricated; the candidate's own delivery battery (builder-run, green at `c9a7303`) stands as the evidence tree's record, and it is not load-bearing for this amendment, whose package carries no converted code."* | **Honest and sufficient for this axis.** The round-1 report's trailing note (above) is the placeholder with nothing beneath it — UNFILLED is the true state, and this round does not fill it either (read-only mandate; the placeholder is left as it stands). It is not load-bearing: the §1a claims the amendment stands on were driven by the candidate row's re-run (green), the review probe (green) and the checker at both trees (ok 18 / ok 19) — all filled in §7(a)(b)(d) above. A full battery at `c9a7303` would attest the *candidate's* health, which nothing in this amendment relies on (never merged; E4.3-B's package carries no converted code; the raw path never left main). The builder-run "full battery green" is an out-of-tree attestation this axis does not grade, consistent with §6's list. A reader needs nothing more for THIS amendment; the battery becomes load-bearing at E4.3-B's own delivery, under its interior review. |

### Adversarial glance — did any fold introduce an unverified mechanism claim?

No. Every factual cite in the folded text maps to round 1's verified set;
two identifiers new to the text were checked read-only on the live tree and
confirmed:

| Cite in folded text | Round-1 provenance | Status |
| --- | --- | --- |
| `recovery_support.rs:360-363` (§2) | [P2-1] enumeration | round-1 cite |
| `classify_open_record` / `discover_open` / dispatch enumerate `.gwz/merge` only (§2) | [P2-1] enumeration (`mod.rs:211-251`, `abort/mod.rs:50,:95`, `recovery.rs:18-29`, `dispatch.rs:328,:472-477`) | round-1 cite |
| checker `:1178-1186` (§3 P-2) | [P3-4] | round-1 cite; re-read: the `!= "fn "` idiom is at `:1180` |
| checker `:1299-1381` (§3 P-2) | [P3-4] `:1299-1332` + `:1335-1381`; §5 `:1295-1381` | union of round-1 cites (label note above) |
| `PROTECTED_SOURCE_TREE_DIGESTS` (§3 P-1) | [P3-3] cited the mechanism by function (`source_tree_digest` `:1025-1042`) and enforcement (`:1101-1103`), not by constant name | **confirmed this round, read-only:** constant at checker `:228`, enforced at `:1101` — same mechanism; `PROTECTED_SOURCE_DIGESTS` (`:40`, the byte-pin) is the distinct constant round 1 named for [P3-4] |
| `rewrite.rs:171` / `:89` (§2) | [P3-2] | round-1 cite; re-confirmed on live `7f28907` |
| `repository_support.rs:122-151`, `preservation_image.rs:236-248` (§1a) | [P3-5](i) | round-1 cite |
| `service.rs:101-108` (§1b) | [P3-5](ii) | round-1 cite |
| "P-1's `durable_fs` counts unaffected; the digest moves" (§7) | [P3-3]: P-1 counts `rename_durable`/`rename_noreplace`/`sync_dir` only; tree digest refreshes on any edit | derivable from round 1 |
| `bootstrap/managed.rs:44-47` allow-reason "shrink as E4.3-E4.6 convert" (§7) | State-axis fold; not in round 1 | **confirmed this round, read-only:** `#[allow(unused_imports, reason = "… the unconsumed imports shrink as E4.3-E4.6 convert")]` at `:43-47`, quote verbatim; a doc-string, not a mechanism claim |

Flags: **none.** Out-of-tree attestations added by the State-axis folds (the
tier-policy line, the two-dual-budget authority, §1c's "the estate holds no
other copy") are process records outside this axis's mandate and are not
graded here, consistent with §6.

### Closing

The [P2-1] confirmation sentence, for the record: the folded ground states
exactly the two readings round 1 verified — no reconciler ships, and the
lifecycle's own derivation of the pair dies at `read_regular` — and
explicitly disclaims the one reading round 1 drove false, so the
overstatement is cured; and because the exception never needed
impossibility, only an unshipped and non-trivial cure, the ground it stands
on is carried intact, now with the enumeration pinned into the clause.

Round 2 of 2. Code axis: **GO**. This section is the Code axis's round-2
record; the amendment's §8 (the lane owner's document) carries the pointer
and is not edited by this axis.
