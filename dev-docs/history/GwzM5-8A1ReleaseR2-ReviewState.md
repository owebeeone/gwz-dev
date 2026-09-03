# A1 release train R2 — focused State-axis review (Decision 1 Option B)

Date: 2026-08-25
Axis: **State** (focused, per `GwzM5-8A1DecisionPacket.md` §4 step 1 and
`GwzM5-8A1ReleasePlan.md` §R2.3 / §10)
Verdict: **GO with conditions** — 0 × P0, 0 × P1, 3 × P2, 9 × P3.

## 0. Object, provenance, and method

**Two-part object.**

| Part | What | Where |
| --- | --- | --- |
| (1) | The landed Decision 1 B production code | gwz-core `8e40fa8` (origin/main): `src/git/gitbackend/repository_support.rs:28-33` (`pin_creation_time_filter_neutralization`), call site A `src/git/gitbackend/repository.rs:64-74` (`create_repo`), call site B `src/git/gitbackend/transport.rs:15-43` (the clone funnel, `disable_filters(true)` at `:31`, pin at `:38`) |
| (2) | The R2 builder's gap package, uncommitted | worktree `…/scratchpad/r2-crlf`, 418 added lines, **zero production**: `src/git/tests/g01.rs` (+236), `src/git/tests/g11.rs` (+89), `.github/workflows/windows-matrix.yml` (+93) |

**Provenance — this is the first filed review of part (1).** Part (1)
landed at `9939b02` (2026-08-16, "Land the filter-policy (D1+D2) and R2-F
missing-tests packages"), whose message self-reports "D1+D2 (State review
GO…)". I independently re-ran the inventory search the R2 builder
reported: **no dev-docs file in `/Users/owebeeone/limbo/gwz-dev/dev-docs/`
contains the string `pin_creation_time_filter_neutralization`**, and no
file matches `*Filter*`/`*D1*`/`*D2*`/`*Policy*`. The claim has no backing
artifact. I therefore reviewed part (1) **as new**, not as a delta. The
same finding has a second edge, which the builder did not report and which
I record as [P2-3]: `GwzM5-8ExactEvidencePlatformAmendment.md:168` cites
"the landing review (F6)" as the source of its mechanism-precision
sentence, and `:108-121` carries a "Closure annotation … adopted decision
D1 landed" — both citing a review that does not exist as a filed artifact.

**Method.** All work ran in the builder's worktree with my own scratch
`CARGO_TARGET_DIR` (`…/scratchpad/rev-state-target`). No code edits
survive: every mutation was reverted and proven byte-exact by `shasum
-a 256` (§4.3). No git operations were performed. This report is the one
dev-docs file I wrote.

**Method caution worth recording for future reviewers (cost me two false
results).** `cargo` freshness is mtime-based and this is a single crate.
A restore performed with `cp -p` preserves the *original* (older) mtime, so
if no other file is newer than the last build, `cargo test` silently
re-runs the **previous binary** and reports a green that means nothing. I
hit this twice; both affected runs were discarded and re-executed after
`touch`-ing the restored sources, and every tail quoted below is from a
build verified to contain 1587 tests (the post-package count: 1585 passed
+ 2 ignored, matching the checkpoint's recorded baseline move).

**Sources read in full or in the cited ranges before the code:**
`GwzM5-8A1ReleasePlan.md` (§3, gate table §1);
`GwzM5-8A1DecisionPacket.md` (§1 in full — Option B mechanics `:135-155`,
§1.5 `:213-230`, the run-9 lesson `:144-146`, the F6 complaint `:225-228`);
`GwzM5-8ExactEvidencePlatformAmendment.md` (`:11-13`, `:99-142`,
`:144-179`, `:181-193`);
`GwzWindowsMatrix-Classification.md` (`:179-190` run 14, `:425-449` the
standing residual tripwire); `CurrentProgramCheckpoint.md` `:955-1049`.

---

## 1. Verdict and findings register

**GO with conditions.** The landed production code is correct, minimal,
and correctly placed; it does what Decision 1 Option B specifies and
nothing more. The gap package closes the packet's one genuinely missing
element (the un-pinned CRLF matrix sentinel) with a design that is
better than the packet asked for — two halves that must *differ*, so CI
can finally distinguish "class closed" from "class invisible". No finding
touches the pins themselves.

| ID | Sev | Finding | Owner |
| --- | --- | --- | --- |
| P2-1 | P2 | The creation-time-only structural guard silently **degrades on CRLF checkouts** — the platform it exists to protect. Measured: an injected mid-life pin is caught on LF and **passes on CRLF** (§4.3 M4). In-tree precedent fix exists at `r2d_seam_freeze.rs:219-223`. | R2 (test-only, ~3 lines) |
| P2-2 | P2 | The `crlf-sentinel` lane's reach is `workflow_dispatch`-only. **Ruling: the class must also ride `release.yml`** (§5.3). | R3 (G4 surface) |
| P2-3 | P2 | Dangling review citations: `9939b02`'s "State review GO" and the amendment's `:168` "the landing review (F6)" / `:108` closure annotation cite an artifact that does not exist. Anchor A must reconcile rather than inherit. | lane owner (docs) |
| P3-1 | P3 | `is_test_source` (`g01.rs:150-172`) classifies **147 of 681** `src/**.rs` files as test-only solely because they contain `#[cfg(test)]`; a production `core.autocrlf` writer in any of them escapes assertion 5. The clause is **not load-bearing today** (§4.4). | R2 |
| P3-2 | P3 | Assertion 5 pins `"core.autocrlf"` only; a mid-life writer that flips **`core.eol`** alone escapes. | R2 |
| P3-3 | P3 | The edge inventory omits the **stash restore** edge (`stash.rs:31-61`, default `StashApplyOptions` ⇒ filters active) and, by framing, the two Clause A recovery edges. Verdict unaffected; restate to the 12 rows of §2.3. | lane owner |
| P3-4 | P3 | `repository_support.rs:13-14` says repo-local "outranks every ambient level" — true for ambient levels, but `GIT_CONFIG_LEVEL_WORKTREE`(6)/`APP`(7) outrank `LOCAL`(5). | R2 (comment) |
| P3-5 | P3 | Lane step 2 cannot distinguish "sentinel failed" from "build failed" (both non-zero). Mitigated by step 1 running first; assert the `test result: FAILED. 0 passed; 1 failed` line for symmetry with step 1's count pin. | R2 |
| P3-6 | P3 | `contract.rs:201-208`'s default `clone_repo_with_progress` forwards to `clone_repo`; a future backend implementing only `clone_repo` inherits no pins. No live exposure (one production impl, §2.4). | forward note |
| P3-7 | P3 | Failed clone can leave a partial directory with no pins — **fail-closed today**: `ensure_clone_target_is_empty` (`verify_checkout_state.rs:54-77`) refuses a non-empty target, so a retry errors `PathCollision` rather than adopting it. Record as closed-with-mitigation. | closed |
| P3-8 | P3 | `created_repo_forward_materialization_is_blob_exact_under_ambient_autocrlf` (`g01.rs:43`) sets **no ambient** config; the hostile value is repo-local to a *different* repository. The precedence property is exercised only where the runner itself is hostile (windows-matrix, release.yml Windows leg). Name overstates on a developer host. | R2 (name/comment) |
| P3-9 | P3 | `g11.rs:48` proves the `tracked_only` sweep; the v1 preservation stash (`preservation.rs:342`) may sweep with untracked flags — same mechanism, untested. | R2-F backlog |

---

## 2. Part (1): the landed object, reviewed as new

### 2.1 The pins — content and placement

```
repository_support.rs:28  pub(super) fn pin_creation_time_filter_neutralization(repo: &git2::Repository) -> ModelResult<()> {
              :29      let mut config = repo.config().map_err(git_error)?;
              :30      config.set_bool("core.autocrlf", false).map_err(git_error)?;
              :31      config.set_str("core.eol", "lf").map_err(git_error)?;
```

**Content — correct.** Exactly the two keys the packet names
(`GwzM5-8A1DecisionPacket.md:137`), no more. `repo.config()` on a freshly
created repository returns the multi-level config whose highest present
writable level is `LOCAL`; libgit2 writes "in the highest priority level
backend" (`git2/config.h:37-39`), and the level ordering is
`PROGRAMDATA(1) < SYSTEM(2) < XDG(3) < GLOBAL(4) < LOCAL(5) <
WORKTREE(6) < APP(7)` (same header, `:53-89`). So the write lands
repo-local and outranks every **ambient** level. This is not left to
inference: `g01.rs:37-39` opens `.git/config` *directly* — not the merged
stack — and asserts both keys, which is the right way to pin it. [P3-4]
is the one precision nit: `WORKTREE`/`APP` outrank `LOCAL`, so a repo that
later enables `extensions.worktreeConfig` could override the pin. That
direction is fail-closed (re-smudge ⇒ `Ambiguous` ⇒ availability loss,
never wrong evidence) and requires deliberate user action post-birth.

**Placement — correct, and structurally unable to become mid-life.**

* Site A, `repository.rs:64-74`: `init_opts` at `:67`, pin at `:70`. The
  options carry `no_reinit(true)` (`:66`) ⇒ `GIT_REPOSITORY_INIT_NO_REINIT`
  (`repository.h:259`), so `create_repo` **errors** on an existing
  repository rather than re-pinning one. The run-9 hazard ("flipping them
  mid-life turns already-smudged worktree files into permanent
  manufactured dirt", ledger run-9 entry / helper doc `:23-27`) is
  therefore not merely avoided by discipline at this site — it is
  unreachable through it.
* Site B, `transport.rs:15-43`: `disable_filters(true)` at `:31`,
  `builder.clone(...)` at `:37`, pin at `:38`. `RepoBuilder::clone`
  performs the initial materialization *itself*, so there is no moment at
  which config could be written before that checkout. `disable_filters`
  is therefore not a stylistic choice — it is the **only** available
  mechanism for the pre-pin window, and it covers exactly that window and
  nothing else.

**Containment — correct.** `pub(super)` on a member of
`git::gitbackend::repository_support` confines the helper to
`git::gitbackend`. Both call sites are siblings inside that module. No
module outside the backend can reach it. Verified by mutation (§4.3 M2).

### 2.2 The division of labour — is it intended?

Yes, and the packet says so precisely. `GwzM5-8A1DecisionPacket.md:135-146`
specifies two *different* mechanisms for two *different* windows:

1. **The pre-pin window** (the clone funnel's initial checkout, the only
   materialization that can precede a pin) → `disable_filters(true)`.
2. **Every later worktree write in a gwz-born repository** → neutralized
   by the repo-local pins, because repo-local config outranks the host.

The implementation matches that division exactly. The packet's §0
executive-summary row ("pin … **and clone filters-off at every gwz
materialization edge**", `:34`) is the imprecise sentence, and gate G2
(`GwzM5-8A1ReleasePlan.md:40`) inherits it verbatim. Read literally, G2
demands `disable_filters` on nine or more edges; that would be *wrong* —
disabling filters on `fast_forward`, `switch_branch`, `checkout_commit`,
`checkout_branch`, `reset_hard` and the two `merge_prepared` edges would
break the porcelain-parity the amendment deliberately preserved
(`GwzM5-8ExactEvidencePlatformAmendment.md:89-95`, "untouched … porcelain
parity retained"). **Restate G2** — proposed text in §7, anchor B slot.

### 2.3 The complete production worktree-writing inventory

The builder's report was not filed to disk and is not in my object, so I
could not read its §3 nine-row table. I built the inventory independently
from `grep` over `src/**` (excluding test sources) for every libgit2 call
that can write worktree bytes. It has **twelve** rows, not nine:

| # | Edge | Filter state | Covered by |
| --- | --- | --- | --- |
| E1 | `transport.rs:22-37` clone funnel initial checkout | **filters OFF** (`:31`) | D1 B — the pre-pin window |
| E2 | `repository.rs:64-74` `create_repo` init | no content exists | D1 B — pins written at `:70` |
| E3 | `repository.rs:85` `reset_hard` (`git_reset` HARD, `None` opts) | filtered | birth pins |
| E4 | `repository.rs:110-114` `checkout_commit` | filtered | birth pins |
| E5 | `repository.rs:135-139` `checkout_branch` | filtered | birth pins |
| E6 | `refs.rs:33-40` `fast_forward` | filtered (`.safe()`) | birth pins |
| E7 | `refs.rs:134-139` `switch_branch` | filtered | birth pins |
| E8 | `merge_prepared.rs:253-255` checked upstream FF | filtered (`.safe()`) | birth pins |
| E9 | `merge_prepared.rs:328` post-true-merge `checkout_tree` | filtered (`None`) | birth pins |
| E10 | `merge_recovery.rs:166-177` `abort_merge` | **filters OFF** (`:174`) | Clause A + D2 refusal (`:165`) |
| E11 | `merge_recovery.rs:246-252` `set_branch_target_checked` | **filters OFF** (`:250`) | Clause A + D2 refusal (`:244`) |
| E12 | `stash.rs:18` + `preservation.rs:342` → `git_stash_save` internal reset | filters ACTIVE, **no lever** | birth pins (proven, `g11.rs:48`) |
| E13 | `stash.rs:31-61` `stash_apply` / `stash_pop` (default `StashApplyOptions`, `stash_support.rs:18-26`) | filters ACTIVE | birth pins (pop leg proven, `g11.rs:100-115`) |

Every coverage claim above is **true**. The nine-row framing the mandate
attributes to the builder ("only the clone funnel's initial checkout is
filters-off; the other eight rely on the birth pins") maps onto E1 + {E3…E9,
E12} and is *substantively* right for the D1-B question — but it is not a
complete inventory of production worktree writers, because it omits E13
(the stash **restore** path, which writes worktree bytes through active
filters) and, on the "only the clone funnel is filters-off" clause, is
false as stated: E10/E11 are also filters-off, from Clause A. Neither
omission changes any verdict — E13 is covered by the pins and is in fact
exercised by the new g11 test's pop leg — but a future reader must not
conclude the restore path was never considered. **[P3-3]: restate to the
twelve rows above at landing.**

### 2.4 The funnel-routing claim

Verified complete. All four production clone sites call
`backend.clone_repo_with_progress`:

```
src/workspace_ops/handle_init_from_sources.rs:137
src/workspace_ops/handle_materialize.rs:265
src/workspace_ops/handle_materialize.rs:491
src/workspace_ops/handle_repo_lifecycle.rs:87
```

`Git2Backend` delegates both `clone_repo` and `clone_repo_with_progress`
explicitly (`gitbackend.rs:75-76`) into `transport::clone_repo{,_with_progress}`,
and `transport::clone_repo` (`:7-13`) itself funnels into the progress
variant. There is exactly **one** production `impl GitBackend`
(`gitbackend.rs:68`); the only other is `workspace_ops/tests/g01/tracking_backend.rs:92`,
a test double. So the funnel claim holds today with no exception.

Two riders, both correctly non-blocking:

* **[P3-6]** `contract.rs:199-208`'s default `clone_repo_with_progress`
  forwards to `clone_repo`. A future backend implementing only
  `clone_repo` would inherit the default and bypass the pins unless its
  own `clone_repo` pins. No live exposure; worth one sentence in the
  trait doc.
* **[P3-7]** A clone that fails mid-materialization can leave a partial,
  un-pinned directory. This is **fail-closed today** and I verified the
  mechanism: `transport.rs:21` calls `ensure_clone_target_is_empty`
  (`verify_checkout_state.rs:54-77`), which refuses any non-empty
  directory with `PathCollision`. A retry therefore errors rather than
  adopting the un-pinned residue. Additionally, whatever *was* written
  went through the filters-off checkout, so it is blob-exact anyway.
  Record as closed-with-mitigation.

---

## 3. The ten review-focus items, adjudicated

The mandate enumerates items 2-10 explicitly; item 1 is inferred from the
checkpoint's R2 delivery record (`CurrentProgramCheckpoint.md:1020-1026`)
as the pins-and-funnel audit.

**(1) Pins content/placement + the funnel claim and its two riders —
ACCEPTED.** §2.1, §2.3, §2.4 above. Verdict: correct; riders ruled
[P3-6]/[P3-7], the second one closed by an existing guard the builder's
note did not credit.

**(2) Is the division of labour intended; should G2 be restated? —
ACCEPTED, and YES.** §2.2. The contract is by design and matches packet
`:135-146`; gate G2's phrasing is inherited from the packet's own §0
summary row and is imprecise in a way that, read literally, would demand a
change that breaks porcelain parity. Restate at landing (§7).

**(3) Repo-local hostile config vs `GIT_CONFIG_GLOBAL` — ACCEPTED, and
stronger than the builder argued.** The builder's stated grounds
(`g01.rs:96-105`) are precedence, hermeticity and thread-safety
(edition-2024 `set_var` unsafety under a parallel suite). All three hold.
I add a decisive fourth the builder did not state: **libgit2 ignores
`GIT_CONFIG_GLOBAL` on gwz's open path.** `config_path_global`
(`libgit2/src/libgit2/repository.c:1390-1401`) consults the environment
only when `repo->use_env` is set, and `use_env` is set only by
`GIT_REPOSITORY_OPEN_FROM_ENV` (`:1130`). gwz opens with plain
`git2::Repository::open` (`repository_support.rs:3-5`), so a fixture using
`GIT_CONFIG_GLOBAL` would be **inert** for every in-process checkout.
Repo-local is not merely equivalent-or-stronger; it is the only form that
works. Note the CI lane correctly uses `git config --global` (which writes
`$HOME/.gitconfig`, found by `git_config__find_global()` with no env
dependence), not the env var — that distinction is right and should be
preserved if the step is ever refactored. One residual, [P3-8]: because
the hostile value lives in a *different* repository from the pinned one,
the unit tests never actually exercise pin-vs-ambient precedence; only a
hostile *runner* does. The test name should say so.

**(4) The structural guard's brittleness — NOT ACCEPTED AS WRITTEN
[P2-1].** The guard is a genuinely good idea and four of its five
assertions are mutation-proven (§4.3). But `function_slice`
(`g01.rs:173-184`) terminates a function body on the literal `"\n}\n"`
(`:179`) and, on failure to find it, **silently falls back to the whole
remaining file** (`:181`). The three sources it slices are read with
`include_str!` (`:225`, `:234`, `:255`), which receives **working-tree**
bytes. On a Windows checkout of this repo those bytes are CRLF — `src/**`
is not pinned by `.gitattributes` (only `scripts/retained_readers/**` and
`protocol/**` are), the windows-2022 image resolves `core.autocrlf=true`
at system level (the lane's own comment, `windows-matrix.yml:87-89`), and
the ledger's **run 14** entry records this exact class already biting once
(`GwzWindowsMatrix-Classification.md:179-190`). The builder's comment
claims the naive parse would "fail loudly rather than silently pass"
(`g01.rs:175-177`); the measured behaviour is the opposite. §4.3 M4 shows
the same real defect — the pin relocated out of `create_repo` into the
mid-life `reset_hard` path — caught on LF and **passing on CRLF**. The
fix is three characters of precedent already in this tree:
`r2d_seam_freeze.rs:219-223` normalizes with `.replace("\r\n", "\n")` and
says why. Apply the same to the three `include_str!` sites. This does not
block G2 (it degrades an extra guard, it does not weaken the pins), but it
must not ship as a guard the program believes is enforcing on Windows.

**(5) `is_test_source`'s `#[cfg(test)]` hole — CONFIRMED, and it is dead
weight [P3-1].** Clause 3 (`g01.rs:167-172`) declares any file containing
`#[cfg(test)]` to be test-only. Measured: **147 of 681** `.rs` files under
`src/` are production-named files that clause 3 alone reclassifies. Any of
them could add a production `core.autocrlf` write and assertion 5 would
not see it. I also measured that the clause is **not load-bearing**: every
current `"core.autocrlf"` writer other than `repository_support.rs` is
already test-by-path or test-by-name (`diff/render/tests.rs`,
`diff/tests/fixture.rs`, `diff/tests/workspace_fixture.rs`,
`git/tests/g01.rs`, `git/tests/g12.rs`, `git/tests/g15.rs`,
`git/tests/g15/root_preservation/support.rs`,
`workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/phases.rs`,
`workspace_ops/tests/g02.rs`). Deleting clause 3 costs nothing and closes
the hole. Pair with [P3-2]: assertion 5 pins only `"core.autocrlf"`, so a
production writer flipping `core.eol` alone is invisible; today only
`repository_support.rs` and `g01.rs` mention `"core.eol"`, so widening the
assertion to both keys is free.

**(6) `#[ignore]` as the expected-fail carrier — WORKS, but it is the
weaker of two available carriers.** The delivered form is sound: the test
asserts a deliberately false claim, `#[ignore]` keeps it out of the
ordinary suite, and the lane inverts the exit code. I rehearsed it end to
end and it behaves exactly as designed (§5.2). Two costs are intrinsic to
the carrier: the lane must invert an exit code (and `cargo test` also
exits non-zero on a *build* failure — [P3-5], mitigated only by step 1
running first), and the sentinel cannot ride any existing `cargo test`
surface, which is what creates item 7's problem. **`#[should_panic(expected
= "CRLF class sentinel PASSED")]` is strictly stronger**: the fixture
guarantees a live smudge source on every platform (repo-local
`autocrlf=true`; my macOS run proves the smudge is live off-Windows), so
such a test is GREEN everywhere today and goes RED exactly when the class
dies — with no exit inversion, no bespoke lane, and automatic presence in
`windows-matrix`, `platform-matrix` **and `release.yml`**. The ledger's
F6 complaint is "no failing sentinel anywhere" in the sense of *nothing
pages when the class regresses*; `should_panic` satisfies that substance.
Recommended as the preferred remediation for [P2-2], not required.

**(7) The sentinel's `workflow_dispatch`-only reach — RULING: the class
must ALSO ride `release.yml`. [P2-2]** Full ruling and routing in §5.3.

**(8) Windows first-dispatch expectation for the stash test — LOW risk,
and specifically not the two known first-dispatch classes.** I checked
both: (a) the run-10 identity class (`config value 'user.name' was not
found`) does **not** apply — `commit_file` uses an explicit
`git2::Signature::now("GWZ Test", …)` (`g01.rs:830`) and `stash_push`
resolves its signature through `merge_signature`, which falls back to
`Signature::now("gwz", "gwz@localhost")` when `repo.signature()` fails
(`merge_support.rs:271-276`); (b) the run-14 `include_str!` CRLF class does
not apply to `g11.rs:48` (it reads no source text) — it applies to the
*structural guard*, which is [P2-1]. Residual risk is ordinary Windows
file semantics on a single-file write/read, and the rest of `g11` already
runs green on Windows (matrix run 16, 1399/4/1, failures confined to the
namespace fault matrix). **Pre-attribution for the next dispatch:**
`g11::stash_round_trip_on_a_born_repo_is_a_filter_no_op` expected GREEN;
`g01::filter_neutralization_pin_is_reachable_only_from_the_two_creation_time_sites`
expected GREEN **but vacuously so** until [P2-1] lands — that pass must
not be read as evidence the guard works on Windows.

**(9) The count-pinned anti-vacuity assertion — CORRECT AND NECESSARY,
empirically.** I probed the premise directly rather than accepting it:

```
##### STEP 1 vacuity probe: one typo'd test name #####
test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 1587 filtered out; finished in 0.00s
cargo exit on a filter matching NOTHING = 0  (this is why the count assertion is load-bearing)
```

libtest exits 0 when `--exact` matches nothing, so without
`grep -qE '^test result: ok\. 6 passed; 0 failed'` a renamed or deleted
sentinel would pass the lane silently — the very vacuity the fixture pins
created. The assertion is load-bearing and the in-workflow comment saying
"do not relax it" (`windows-matrix.yml:96-99`) is correct. The hard-coded
6 is right for `windows-2022` (the sixth, `g12`'s doctrine sentinel, is
`#[cfg(windows)]` — `g12.rs:1033`); on a Unix host only five exist, which
is why my verbatim rehearsal trips the count (§5.2) — expected, not a
defect.

**(10) The stale amendment header — CONFIRMED.**
`GwzM5-8ExactEvidencePlatformAmendment.md:11-13` still reads "The two OPEN
DECISIONS in Clause A's scope limits … remain tracked review debts —
acceptance does not close them", while its own body carries "*(Closure
annotation, 2026-08-16, adopted decision D1 landed …)*" (`:108-121`) and
"**DISCHARGED 2026-08-16 by adopted decision D2 (A′ refined refusal,
landed in the filter-policy package)**" (`:159-161`). The header is stale
against the body; the builder's reading is factually accurate. The
correction is not merely a date edit — see [P2-3] and §7 anchor A.

---

## 4. Part (2): the gap tests

### 4.1 Green scopes (my runs, this worktree, clean rebuild)

```
===== g01 =====
test result: ok. 28 passed; 0 failed; 1 ignored; 0 measured; 1558 filtered out; finished in 0.52s
===== g11 =====
test result: ok. 9 passed; 0 failed; 0 ignored; 0 measured; 1578 filtered out; finished in 0.11s
===== g12 =====
test result: ok. 20 passed; 0 failed; 0 ignored; 0 measured; 1567 filtered out; finished in 0.42s
```

Binary total 1587 tests (1585 + 2 ignored) — matches the recorded baseline
move. `cargo fmt --check` clean; `CLIPPY_CONF_DIR="$PWD" cargo clippy
--all-targets --all-features -- -D warnings` exit 0. The workflow parses
as valid YAML with jobs `['matrix', 'crlf-sentinel']`.

### 4.2 The un-pinned sentinel's RED evidence — re-executed, not accepted

```
running 1 test
test git::tests::g01::crlf_sentinel_unpinned_worktree_materializes_blob_exact ... FAILED
thread '…::crlf_sentinel_unpinned_worktree_materializes_blob_exact' panicked at src/git/tests/g01.rs:320:5:
assertion `left == right` failed: CRLF class sentinel PASSED, which means it stopped being a sentinel: an
un-pinned worktree materialized blob-exact, so this environment no longer has a live smudge source and every
CRLF assertion in this suite is now vacuous
  left: [108, 105, 110, 101, 49, 13, 10, 108, 105, 110, 101, 50, 13, 10]
 right: [108, 105, 110, 101, 49, 10, 108, 105, 110, 101, 50, 10]
test result: FAILED. 0 passed; 1 failed; 0 ignored; 0 measured; 1586 filtered out; finished in 0.01s
```

`left` is `line1\r\nline2\r\n`, `right` is `line1\nline2\n`. The sentinel
fails CRLF-vs-LF as claimed. The failure message is well-chosen: it tells
the future reader what a *pass* would mean.

### 4.3 Mutation re-verification of the structural guard

Four mutations, each applied to the landed production code, then reverted
and proven byte-exact. Pre/post `shasum -a 256` for all four touched
files:

```
ea34a847cfdb87e1d8fbb0962d57bd3156addf5a45c3b89a59cdf82cb53e8204  src/git/gitbackend/refs.rs
800c99e64c455bb283532034ffee2ab32b2ce8d4ba0fccde36472c8dbd592b21  src/git/gitbackend/repository.rs
d7ae7d788b6b2a0d47c30d4b2a0d5b6ad3833dac45e7da291805b6eee8806892  src/git/gitbackend/repository_support.rs
b34e2157de7e5b115f8b1db60ad0e9625bff717e22de3963cedd60d9e2143b07  src/git/gitbackend/transport.rs
```

identical before and after; `git status --short` ends at the builder's
three modified files and `git diff --stat` at `3 files changed, 418
insertions(+)`.

**M1 — a third call site (a real mid-life pin injected into
`refs.rs::fast_forward`, after its checkout):** assertion 1 trips.

```
panicked at src/git/tests/g01.rs:209:5:
assertion `left == right` failed: creation-time-only guard: the filter-neutralization pin gained or lost a
reference. A new call site must be proven to run at repository CREATION, before any filtered materialization
(run-9 lesson), or Decision 1 Option B is re-opened.
  left: ["git/gitbackend/refs.rs", "git/gitbackend/repository.rs", "git/gitbackend/repository_support.rs", "git/gitbackend/transport.rs", "git/tests/g01.rs"]
 right: ["git/gitbackend/repository.rs", "git/gitbackend/repository_support.rs", "git/gitbackend/transport.rs", "git/tests/g01.rs"]
```

**M2 — containment widened (`pub(super)` → `pub(crate)`):** assertion 2
trips.

```
panicked at src/git/tests/g01.rs:226:5:
creation-time-only guard: the pin helper must stay `pub(super)` (contained inside git::gitbackend)
```

**M3 — `checkout.disable_filters(true);` deleted from the clone funnel:**
assertion 4 trips **and** the pre-existing landed test (from `9939b02`)
trips, which independently proves that landed test is non-vacuous.

```
test git::tests::g01::filter_neutralization_pin_is_reachable_only_from_the_two_creation_time_sites ... FAILED
test git::tests::g01::clone_funnel_materializes_blob_exact_and_pins_filter_neutralization ... FAILED
panicked at src/git/tests/g01.rs:262:5:
creation-time-only guard: the clone funnel's initial materialization must run with filters disabled
panicked at src/git/tests/g01.rs:623:5:
assertion `left == right` failed: gwz clone funnel must materialize exact blob bytes
test result: FAILED. 0 passed; 2 failed; 0 ignored; 0 measured; 1585 filtered out; finished in 0.07s
```

**M4 — the finding [P2-1], measured.** The pin is *moved* out of
`create_repo` into `reset_hard` — i.e. it becomes a genuine mid-life pin
on an already-materialized worktree, the exact run-9 hazard. Same
mutation, two working-tree encodings:

```
===== VARIANT 1: LF checkout =====
test git::tests::g01::filter_neutralization_pin_is_reachable_only_from_the_two_creation_time_sites ... FAILED
panicked at src/git/tests/g01.rs:246:10:
creation-time-only guard: create_repo must pin at creation
test result: FAILED. 0 passed; 1 failed; 0 ignored; 0 measured; 1586 filtered out; finished in 0.04s

===== VARIANT 2: same mutation, CRLF working tree =====
test git::tests::g01::filter_neutralization_pin_is_reachable_only_from_the_two_creation_time_sites ... ok
test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 1586 filtered out; finished in 0.08s
```

Mechanism: with CRLF bytes, `function_slice`'s `"\n}\n"` needle never
matches, the fallback at `g01.rs:181` widens the slice from `create_repo`'s
body to the entire rest of the file, and the `init < pin` ordering check
then finds the *relocated* pin further down and passes. The guard's
scoping is lost precisely on Windows.

### 4.4 Quantified evidence for [P3-1]

Walking `src/**` with the guard's own classifier rules: 681 `.rs` files
total; **147** are production-named files that `is_test_source` returns
true for *solely* because they contain `#[cfg(test)]` (e.g.
`checked_artifact/admission/driver.rs`, `checked_artifact/authority.rs`,
`checked_artifact/capability/pre_catalog/provider.rs`, …). None currently
writes `core.autocrlf`, so the assertion is sound today and the clause is
removable at zero cost.

### 4.5 The g11 stash round-trip

`g11.rs:48-115` is the strongest test in the package. It is a
control/subject pair in one test: the CONTROL (`:56-75`, adopted-style
repo) asserts that `git_stash_save`'s internal reset re-materializes the
swept file as `line1\r\nline2\r\n`, so the SUBJECT assertion
(`:77-92`, gwz-born, blob-exact) cannot pass for the wrong reason. It then
adds a return leg (`:94-115`): `stash_pop` restores the stashed bytes
verbatim and the entry is gone — which, per §2.3, is the only coverage
E13 (the filters-active restore path) has anywhere. Both arms pass here.
This is the right shape for every test in this class and should be the
pattern for any future CRLF assertion.

---

## 5. The `crlf-sentinel` lane

### 5.1 Shape and topology audit

`windows-matrix.yml:69-138`: `runs-on: windows-2022`; steps = checkout,
toolchain, cache, "Un-pin the host filter config", "Pinned lane … (must be
GREEN)", "Un-pinned sentinel … (must be RED)". The design principle — one
job running **both halves and requiring them to differ** — is the correct
answer to the ledger's F6 complaint, and is a better artifact than the
packet asked for: it separates "the class is closed" from "CI is blind to
the class", which is the distinction the run-7 fixture pins destroyed.

The in-workflow §17 topology audit (`:55-68`) is accurate and I verified
it: every command resolves inside the checkout (`rustup`; `git config
--global`, which writes `$HOME/.gitconfig` on an ephemeral runner and
reads nothing from the workspace; `cargo test` with in-repo `Cargo.toml`,
`include_str!` targets and `env!("CARGO_MANIFEST_DIR")`). No
`scripts/checks/*` driver is invoked and nothing reads `ROOT.parent`, so
this lane carries **none** of the workspace-root-reading blocker class
that un-wired `check_m4_scenario_map.py` (run `32749441874`). That audit
is what makes my §5.3 ruling wire-safe.

One correctness point in the lane's favour that deserves recording: the
step uses `git config --global` (writing the real file, which libgit2
finds through `git_config__find_global()`), **not** `GIT_CONFIG_GLOBAL`
(which libgit2 ignores unless the repo was opened `FROM_ENV` — §3 item 3).
If that step is ever "simplified" to an env var it becomes a no-op.

### 5.2 Rehearsal (mine, macOS host)

```
##### STEP 1 verbatim (macOS; g12 arm cfg(windows) => 5 of 6 exist) #####
test result: ok. 5 passed; 0 failed; 0 ignored; 0 measured; 1582 filtered out; finished in 0.07s
cargo exit=0
==> STEP 1 exits 1 here (host-count 5 != lane-count 6); on windows-2022 all six exist

##### STEP 1 unix-adapted (5-test count, same shape) #####
test result: ok. 5 passed; 0 failed; 0 ignored; 0 measured; 1582 filtered out; finished in 0.07s
cargo exit=0
==> ADAPTED STEP 1 GREEN (lane logic sound)

##### STEP 2 verbatim (un-pinned sentinel must be RED) #####
  left: [108, 105, 110, 101, 49, 13, 10, 108, 105, 110, 101, 50, 13, 10]
 right: [108, 105, 110, 101, 49, 10, 108, 105, 110, 101, 50, 10]
test result: FAILED. 0 passed; 1 failed; 0 ignored; 0 measured; 1586 filtered out; finished in 0.01s
sentinel failed as required — the CRLF class is visible in this lane
```

I did **not** run `git config --global core.autocrlf true` — that would
mutate the operator's own `$HOME/.gitconfig`. The omission does not weaken
the rehearsal: every fixture in the lane carries its hostile config
repo-locally, which is why it reproduces off-Windows at all. Both halves
behave as designed; the lane logic is sound.

### 5.3 Item-7 ruling — `release.yml`

**Ruling: YES. The CRLF class must also ride `release.yml`. [P2-2],
routed to R3's G4 surface, landing before the tag. It is NOT a condition
of G2** — G2 inherits the packet's words "add one un-pinned CRLF lane (or
expected-fail sentinel) **to the windows matrix**"
(`GwzM5-8A1DecisionPacket.md:225-227`), and the builder did exactly that.

Reasoning:

1. **What already rides `release.yml`.** `release.yml:89-90` runs `cargo
   test --locked` on both `ubuntu-22.04` and `windows-2022` at the
   published tag. So four of the six pinned tests, plus `g12`'s doctrine
   sentinel, already execute at the tag on Windows — and non-vacuously,
   because each carries its own live CONTROL arm. That is more coverage
   than the completeness review assumed.
2. **What does not.** The `#[ignore]`d un-pinned sentinel (the
   class-visibility proof), and the count assertion (the anti-rename,
   anti-deletion pin). Those are exactly the two things that convert "the
   suite is green" into "the suite is green *and* still watching".
3. **Why the gap matters at the tag.** The release plan makes the
   published tag the moment of truth (`§R5.3`: "On the published tag,
   `release.yml` verify must go green on both OS legs"), and G4 exists
   specifically because a checker that is green locally but un-wired from
   the release lane is the `32749441874` failure class (activation record
   §17). A sentinel reachable only by manual dispatch of a
   `workflow_dispatch`-only workflow is that class.

**Routing, concretely — two steps, not a job.** Add to the *existing*
`verify` job in `release.yml`, gated `if: runner.os == 'Windows'`, placed
**after** the existing `Test` step: (i) the count-pinned exact-name run of
the six pinned tests, (ii) the `--ignored --exact` sentinel run requiring
a non-zero exit. Reuse the job's checkout and toolchain — marginal runner
cost is two `cargo test` invocations against an already-built target, vs.
a whole duplicate `windows-2022` job. Setting `core.autocrlf=true` at
global level there is safe: the runner image already resolves it true at
system level, so the existing suite is already green under that ambient.

**Preferred alternative that subsumes half of this:** convert the
un-pinned sentinel to `#[should_panic]` (§3 item 6). It then rides
`release.yml`, `platform-matrix.yml` and `windows-matrix.yml`
automatically, and only step (i) — the count pin — needs adding. Either
route discharges [P2-2]; the lane owner picks.

---

## 6. The NEW EVIDENCE claim — the exposure is non-Windows-only *in
practice*

**Assessment: the claim is correct, materially new, and slightly
understated.**

The packet already asserted the principle — "The exposure is not
Windows-only in principle … host `autocrlf=true` is just the dominant
Windows instance" (`GwzM5-8A1DecisionPacket.md:51-55`). What did not
exist before this package is an **executed measurement** off Windows. The
g11 CONTROL arm supplies it: on macOS, `git_stash_save`'s internal reset
re-materialized the swept tracked file as `line1\r\nline2\r\n`
(`g11.rs:67-75`; the test passes, and that assertion is what it passes
*on*). The un-pinned sentinel supplies a second, independent macOS
measurement of the same class through the ordinary forward-checkout edge
(§4.2, `left: [… 13, 10 …]`).

Two refinements to how the claim should be recorded:

* **Understatement.** The stash arm is the first executed proof for
  *exposure (1)* — the `stash_save` filtered reset — off Windows. The
  sentinel arm is the first executed proof for *exposure (2)* off
  Windows. Both halves of the standing tripwire are now measured on a
  non-Windows host, not just one.
* **Precision.** The driver here is **config**, not OS: repo-local
  `core.autocrlf=true`. So the accurate statement is "the class follows
  the *filter configuration*, not the platform; Windows is where that
  configuration is the default". Do not let the ledger record it as
  "macOS is exposed" — a macOS user who never sets `autocrlf` is not.
  The genuinely platform-independent residual remains the attribute-driven
  one (`eol=crlf`, `ident`, foreign `filter=`), which Option B does not
  touch and which the packet freezes as permanent fail-closed doctrine
  (`:150-155`, `:222-225`).

---

## 7. The four annotation texts (anchors A-D) — signing statement

**Scope limit, stated plainly.** The builder's report §8 was not filed to
disk and is not in my object; I could not read the verbatim anchor texts.
I therefore rule on the **claims the mandate attributes to them**, and I
give the lane owner mechanically checkable conditions. **I cannot sign
wording I have not seen.** Where a condition below is met by the delivered
text, my GO covers it; where it is not, the sentence must not be applied
without a re-read.

**Anchor A — the amendment header (`:11-13`) is stale against its own
body. FACTUALLY ACCURATE; sign, subject to two conditions.**
The claim is verified (§3 item 10). Conditions:

* **A-i.** The replacement header must date the closures to the **landing
  commit** (`9939b02`, 2026-08-16) and must **not** assert that the
  packet-mandated focused State reviews preceded them. They did not: the
  reviews were filed 2026-08-25 as `GwzM5-8A1ReleaseR1-ReviewState.md`
  (D2/A′) and this document (D1/B).
* **A-ii. [P2-3]** The body's dangling citations must be corrected in the
  same pass, not inherited: `:168` attributes its mechanism-precision
  sentence to "the landing review (F6)", and `:108-121` is styled as a
  "Closure annotation" of a review round — neither exists as a filed
  artifact. Re-point both to the reviews that now exist, or mark the
  sentences as lane-owner findings recorded at landing. A frozen contract
  text citing a non-existent review is exactly the ledger drift that let
  the release plan re-schedule already-landed work as unstarted.

I would **not** sign any sentence of the form "the OPEN DECISIONS were
closed by reviewed decision on 2026-08-16" without A-i's qualification.

**Anchor B — (slot inferred: the gate-G2 / release-plan restatement). NOT
SIGNED — text not seen.** My independent ruling on the substance is §2.2
and §3 item 2, and I supply the restatement I *would* sign, for the lane
owner to compare against the delivered text:

> **G2** — Decision 1 **B** landed: repo-local birth-time
> `core.autocrlf=false` + `core.eol=lf` pins at **both** gwz
> repository-creation edges (`create_repo`; the single production clone
> funnel), with the clone funnel's initial checkout additionally running
> **filters-off** to cover the one window that precedes its pin. Every
> later worktree-writing edge in a gwz-born repository — forward
> checkouts, resets, fast-forwards, merge materialization, and libgit2's
> internal `stash_save`/`stash_apply` resets — is neutralized **by the
> repo-local pins**, not by `disable_filters`; the two recovery-grade
> edges remain filters-off under Clause A. Plus the un-pinned CRLF matrix
> sentinel.

**Anchors C and D — "the born-repo half of the stash exposure is CLOSED by
the pins, with the g11 test as proof". SUBSTANTIVELY ACCURATE; sign,
subject to three conditions.**
Mechanism check: `git_stash_save`'s internal reset re-materializes swept
tracked files through the active filter list and exposes no checkout
options (amendment `:181-188`); with repo-local `autocrlf=false` +
`eol=lf` the crlf filter is identity, so the sweep is a byte no-op. That
is sound, and it is now measured non-vacuously (§4.5). Conditions:

* **C-i — the attribute qualifier is mandatory.** "Closed" is true only
  for the **config-driven** filter class. Attribute-driven smudge
  (`eol=crlf`, `ident`, foreign `filter=`) still reaches `stash_save`
  inside a gwz-born repo; the pins do not touch attributes. The packet
  already freezes this as permanent fail-closed doctrine (`:150-155`,
  `:222-225`) and the ledger entry must carry it. An unqualified "exposure
  (1) is closed for gwz-born repos" is a sentence **I would not sign**.
* **C-ii — scope the closure to gwz-*born*.** `create_repo`- and
  clone-funnel-created repositories only. Adopted worktrees and
  workspaces materialized by pre-B gwz versions are unchanged, and their
  remedy remains the post-A1 `renormalize` command. (The ledger's existing
  `:437-449` update already words exposure (2) this way; C/D must match
  that register for exposure (1).)
* **C-iii — name the evidence honestly.** The proof is executed on
  macOS/Linux only as of this review; `g11.rs:48` has never run on
  Windows. The mechanism is platform-independent (config-driven), so
  "closed by construction, proven non-vacuously off-Windows, Windows
  execution riding the next matrix dispatch" is the accurate form.
  "Proven on Windows" is not yet true.
* Optional but recommended: cite `g11.rs:94-115` explicitly as the
  coverage for the **restore** leg (`stash_apply`/`stash_pop`), which is
  otherwise an untested edge (§2.3 E13).

**Sentences I would not sign, in summary:** any claim that a filed review
backed the 2026-08-16 closures; any unqualified "exposure (1) is closed"
without the attribute residual; any claim of Windows-executed proof for
`g11.rs:48`; and gate-G2 text that reads as "`disable_filters` at every
materialization edge".

---

## 8. G2 verdict, and the sentinel-discharge ruling

### 8.1 Is G2 satisfied?

**YES — at `8e40fa8` + the builder's gap package, under the conditions
below.** Decomposing the gate:

| G2 element | State at `8e40fa8` | With the gap package |
| --- | --- | --- |
| birth-time `core.autocrlf=false` + `core.eol=lf` pins | **LANDED, correct** (§2.1); mutation-proven tests | unchanged |
| filters-off clone checkout (the pre-pin window) | **LANDED, correct** (`transport.rs:31`); mutation-proven (§4.3 M3) | unchanged |
| "at every gwz materialization edge" | satisfied **as the packet means it** (§2.2); G2's literal phrasing is wrong and must be restated | unchanged |
| the un-pinned CRLF matrix sentinel | **ABSENT** | **PRESENT** (`windows-matrix.yml:69-138`), rehearsed, RED-proven |

Conditions on the GO:

* **G2-c1 (evidence).** The `crlf-sentinel` lane must actually execute at
  the release candidate — i.e. the G5 `windows-matrix` dispatch at the RC
  sha must show the pinned half `ok. 6 passed; 0 failed` and the un-pinned
  half failing. Until then G2 rests on a Unix rehearsal (§5.2). This is a
  recording obligation on R5.2, not extra work.
* **G2-c2 ([P2-1]).** The structural guard's `include_str!` CRLF
  normalization lands, **or** the guard's doc comment is amended to state
  that its scoping assertions are LF-host-only. It must not ship as a
  guard the program believes enforces on Windows.
* **G2-c3 ([P2-3]).** Anchor A applies with conditions A-i/A-ii; anchors
  C/D with C-i/C-ii/C-iii (§7).
* **G2-c4.** Gate G2's text is restated per §7 anchor-B slot at landing.
* **[P2-2] is explicitly NOT a G2 condition** — it is a G4/R3 obligation,
  recorded here so it cannot be lost between phases.

The nine P3s are landing-quality items; none blocks.

### 8.2 Is the packet's sentinel requirement now discharged at the CI level?

**YES at the matrix level; NO at the always-on/release level — and the
builder's "CI-level completion" framing is correct.**

The completeness review had been treating `g12`'s
`doctrine_sentinel_adopted_crlf_worktree_classifies_ambiguous_in_the_reverse_observer`
as the sentinel. That conflates two different instruments:

* **`g12`'s doctrine sentinel** pins the *doctrine* — an adopted CRLF
  worktree classifies `Ambiguous` in the reverse observer. It is
  `#[cfg(windows)]` and it goes red when the **raw-byte doctrine**
  changes. It says nothing about whether CI can still *see* the CRLF
  class, and it is one of the tests that would keep passing in a world
  where the smudge source vanished.
* **The `crlf-sentinel` lane** pins the *environment* — the class is
  still live and the green half is therefore a real result. It is the
  first artifact in this program that can distinguish a closed class from
  an invisible one, which is precisely the ledger's F6 complaint
  (`GwzM5-8ExactEvidencePlatformAmendment.md:188-193`, "no failing
  sentinel anywhere — CI is structurally blind to it") and the packet's
  §1.5 ask (`:225-228`).

So the packet's ask — "end the CI blindness by adding one un-pinned CRLF
lane (or expected-fail sentinel) to the windows matrix" — is **discharged
by this package and by nothing that preceded it**. The residual is reach,
not existence: `windows-matrix.yml` is `workflow_dispatch:`-only
(`:6-7`), so the discharge is manual-dispatch-scoped until [P2-2] lands.

---

## 9. What this review did not cover

* The builder's report §3/§8 verbatim texts (not filed to disk; §2.3 and
  §7 state how I compensated).
* Decision 2 / A′ — the object of `GwzM5-8A1ReleaseR1-ReviewState.md`.
  I read the two D2 preflight call sites only to classify edges E10/E11.
* Windows execution of anything. Every tail above is macOS
  (`darwin 25.5.0`, rustc per `rust-toolchain.toml`). The Windows arms of
  `g11.rs:48`, the structural guard, and the `crlf-sentinel` lane are
  unproven until the next dispatch; §3 item 8 pre-attributes them.
* `tests/protocol.rs` — 2 pre-existing local failures from a local
  `taut-proto` ≠ 0.8.1, proven on pristine `8e40fa8` and not this
  package's; CI installs the pinned version.

---

**Verdict: GO with conditions** — G2 is satisfied at `8e40fa8` + the R2
gap package subject to G2-c1…G2-c4; [P2-2] routes to R3's G4 and must land
before the tag.
