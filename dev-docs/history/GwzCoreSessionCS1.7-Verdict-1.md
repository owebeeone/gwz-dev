# Core session plan CS1.7 (the conditional-compilation boundary check) — second review verdict

Date: 2026-09-28. Status: **accepted at the ten gwz-core files listed below, on gwz-core `bd538656`, gwz-cli `ebbea902` and gwz-py `0b535dc5`, after [Consistency-1](GwzCoreSessionCS1.7-ReviewConsistency-1.md) and [Safety-1](GwzCoreSessionCS1.7-ReviewSafety-1.md) reported GO; this accepts step CS1.7 as implemented only**. The post-GO corrections below were applied afterwards in two small patches, and both reviewers confirmed every item; the files that ship are the ones in "Final state after the post-GO corrections". The step is uncommitted. Acceptance authorizes no commit, push or release: the operator decides those.

The object was the implementation after the [first remediation plan](GwzCoreSessionCS1.7-RemPlan.md), which answered the [first verdict](GwzCoreSessionCS1.7-Verdict.md)'s NO-GO in one patch. Both round-1 reviewers re-verdicted it with their context intact.
- This was round 2 of the two-round cap.
- Each reviewer verified the ten files and the HEADs at the start and the end.
- Each reran its round-1 counterexamples and the step's tests (75 tests across the check modules).
- Each confirmed, by its own scan of the HEAD blobs, that the allowlist matches the trees exactly: 432 keys and 437 occurrences.
- The Safety report was held outside `dev-docs` until the Consistency report had finished.

| Axis | Verdict | Prior findings closed | New findings |
| --- | --- | --- | --- |
| Consistency | GO | C-P2-1, C-P2-2, C-P3-1 (as dispositioned), C-P3-2, C-P3-3 | P3-4, P3-5 |
| Safety | GO | P2-1, P3-1 (as dispositioned), P3-2 to P3-5, P3-6 (as dispositioned) | P3-7, P3-8, P3-9 |

## The accepted object

| File | SHA-256 |
| --- | --- |
| `scripts/checks/check_cfg_boundaries.py` | `f3b86f078c071d970b2fe79c344ddb7845bff02421f0d18968983bee249a9992` |
| `scripts/checks/test_check_cfg_boundaries.py` | `28da2ef33bd5e9cacba0c3f45d5a1b7be1c72ec720ac1dd417122f9da7e1c989` |
| `scripts/checks/cfg_boundaries_allowlist.json` | `b1e17eb879fa6dfd78afb1e37798655cd4e5085c432969153cf3212d0863e65e` |
| `scripts/run_tests.py` | `35f7983b6484d3960568fc2ec31b1256282caf613194d89c33ec45fe83106f6f` |
| `scripts/release.py` | `b6f5631fa36d356876b5ec60b83b961779769314b439e1c8153099fde151ccc5` |
| `scripts/checks/test_release_boundary.py` | `0c2ece8039145b1087d1a7158f2b7b9173f20f7fb4e7f17c02ebb70ffcfa719a` |
| `.github/workflows/release.yml` | `fc802595078269851eb0a8b25e65b3e52667c9e951a0ad58ac120e3372a62e5d` |
| `.github/workflows/platform-matrix.yml` | `77d9a2630395e372bf546a42717b484796929d8785020b87b295062e2658938d` |
| `.github/workflows/windows-matrix.yml` | `e289c9d9070b83461599eca05e9b22919d08cd7073485203090e8fdcdd043a32` |
| `.github/workflows/checked-artifact-boundary.yml` | `4973732fc0823938cbfa6659f5a5bd5c6ef7d642bd5fbeee6a13e06b1e5531dd` |

The inventory is 437 occurrences under 432 keys:
- 264 items;
- 173 statements: 147 expression statements, 24 `let` and 2 statement macros.

By repository, gwz-core has 435, gwz-cli 2 and gwz-py 0.

## Post-GO corrections

Both reviewers cleared their new P3s to land after the GO. They were applied to the accepted object in a follow-up patch, and each reviewer confirmed its own items on the corrected tree, as TR1.3's post-GO corrections were.

| ID | Correction | Closure test |
| --- | --- | --- |
| C-P3-5, S-P3-9 | Blind convergence. A statement directly after an inner attribute (`#![…]`) at the top of a block is scanned: the start rule also accepts a `]` that closes an inner attribute. | `fn f() { #![allow(unused)] #[cfg(unix)] let a = 1; #[cfg(unix)] g(a); }` flags `let a` and `g(a)`. `mod m { #![cfg(windows)] use a::I; }` still flags nothing. |
| C-P3-4 | `--shrink-from` compares totals per repository, attributes and item, summed across paths. A key is ADDED only when that triple's total exceeds the base's total. Moving listed debt between files, as a movement-only split does, then passes. The docstring says so. | Base `{src/a.rs: use x::Y, 1}`: current `{src/b.rs: use x::Y, 1}` exits 0; both files exit 1; `{src/b.rs: use x::Y, 2}` alone exits 1. |
| S-P3-7 | `--shrink-from` also fails when the scanned scope narrows. That covers a repository in the base that is absent now, a repository whose `path` changed, and a base root no longer covered by the current roots. | Dropping gwz-cli from `repos`, or narrowing gwz-core's roots from `.` to `src`, fails, naming the change. Adding a repository or a root, or an unchanged `repos`, passes. |
| S-P3-8 | **Choice: bracedness is decided by the statement's first token, not by any interior brace**, as the rule's first sentence requires.<br>• A statement passes as braced only when it starts with `{`, `if`, `match`, `loop`, `while`, `for`, `unsafe`, `async`, a label, or is a brace-delimited macro.<br>• Any other statement that reaches `;` is keyed by its rendered text, with `{ }` groups stepped over.<br>• A match arm that follows a block-bodied arm is still not a statement, since no `;` ends it before the match body closes. | Safety's eight snippets flag: `state = State { mode };`, `total = if a { 1 } else { 2 };`, the `match` form, `return S { a: 1 };`, `x = { compute() };`, `*slot = S { a: 1 };`, the braced closure and `n += { 1 };`. The braced forms and statement-position `cfg_if!` still pass. The two match-arm sites (`crates/refcopy/src/native/attempt.rs:83`, `src/git/tests/g15/root_preservation/stash.rs:391`) are not flagged. The inventory still matches the HEAD blobs; both reviewers found no such statement in the trees. |

## Final state after the post-GO corrections

1. **The first patch** applied the four corrections above: `check_cfg_boundaries.py` and its tests only.
   - Both reviewers confirmed their items: [Consistency-1a](GwzCoreSessionCS1.7-ReviewConsistency-1a.md) and [Safety-1a](GwzCoreSessionCS1.7-ReviewSafety-1a.md), filed verbatim.
   - The Safety reviewer found two new P3s in that patch: an `impl` or `extern` block in a function body keyed as a statement (a false positive), and `async { … }.await;` passing as braced (a false negative).
   - It asked for the summing trade-off to be recorded: a removal in one file and an identical addition elsewhere in the same repository pass, because a movement-only split looks the same.
   - The Consistency reviewer asked for the allowlist's `rule` sentence to be reworded.
2. **The second patch** fixed both new P3s, recorded the trade-off in the docstring and in the `rule` text, and reworded the `rule`: the checker, its tests, and the allowlist's line 2 only. The entries and `repos` are byte-identical.
   - It also amends S-P3-8's list above: `async` is no longer a braced start, since an `async` block is an expression without a block, like a closure.
   - `impl` and `extern` blocks in a function body are braced items, as the docstring already said.
   - Both reviewers confirmed: [Consistency-1b](GwzCoreSessionCS1.7-ReviewConsistency-1b.md) and [Safety-1b](GwzCoreSessionCS1.7-ReviewSafety-1b.md), filed verbatim.
   - Neither found a new defect.

On the final tree:
- 83 tests pass across the five check modules;
- the check reports "1199 files, 437 listed occurrences (gwz-core 435, gwz-cli 2, gwz-py 0); nothing new";
- each reviewer's own scan of the HEAD blobs matches the allowlist exactly: 432 keys and 437 occurrences.

| File | SHA-256 |
| --- | --- |
| `scripts/checks/check_cfg_boundaries.py` | `735bd38987001999c4860b45047bd1f5314a244da66934df4abc8c383ed6bb67` |
| `scripts/checks/test_check_cfg_boundaries.py` | `422f0310f39d8e6d954423a0d6ef7f96382b68d2a9383774a11852263cf1fde9` |
| `scripts/checks/cfg_boundaries_allowlist.json` | `96c7b365493aa30b239f7f3704b5972ef083e46ceff3ab0033d8af004366e923` |
| `scripts/run_tests.py` | `35f7983b6484d3960568fc2ec31b1256282caf613194d89c33ec45fe83106f6f` |
| `scripts/release.py` | `b6f5631fa36d356876b5ec60b83b961779769314b439e1c8153099fde151ccc5` |
| `scripts/checks/test_release_boundary.py` | `0c2ece8039145b1087d1a7158f2b7b9173f20f7fb4e7f17c02ebb70ffcfa719a` |
| `.github/workflows/release.yml` | `fc802595078269851eb0a8b25e65b3e52667c9e951a0ad58ac120e3372a62e5d` |
| `.github/workflows/platform-matrix.yml` | `77d9a2630395e372bf546a42717b484796929d8785020b87b295062e2658938d` |
| `.github/workflows/windows-matrix.yml` | `e289c9d9070b83461599eca05e9b22919d08cd7073485203090e8fdcdd043a32` |
| `.github/workflows/checked-artifact-boundary.yml` | `4973732fc0823938cbfa6659f5a5bd5c6ef7d642bd5fbeee6a13e06b1e5531dd` |

The items under "Carried, not this step's" below still stand.

## Carried, not this step's

- **Sibling coverage in CI.** The check runs over gwz-cli and gwz-py only locally, until their own CI runs it with gwz-core beside them. That is a follow-up task, filed once CS1.7 is committed and on gwz-core's `main` (C-P3-1, S-P3-6).
- **Plan text.** Two sentences for the session plan: the exclusion, with counts, of fields, variants, match arms, parameters, call arguments, struct-expression fields and tail expressions; and that sibling coverage is local-only. They land with TR1.4b's post-GO corrections.
- **Carried from the first verdict:**
  - the duplicated lexer, now with two divergences from `check_process_globals.py`'s;
  - the process-globals allowlist's own growth gap;
  - `release.py --no-test` skipping the lexical checks.
- **Residual notes for later:**
  - The pull-request comparison uses the base branch's tip, not the merge base. A pull request behind a `main` whose list has since shrunk sees ADDED until it rebases.
  - A rename of the allowlist would open one "no base" pass.
  - An unreadable directory is skipped by `os.walk`.
  - Statement keys render `=>` as `= >`, and drop a 1-tuple's comma.

## Round count

| Round | Consistency | Safety | Blocking findings |
| --- | --- | --- | --- |
| 1 | NO-GO (2 P2, 3 P3) | NO-GO, escalated (1 P2, 6 P3) | C-P2-1 and S-P2-1 converged blind; C-P2-2 |
| 2 | GO (2 new P3) | GO (3 new P3) | none |

No finding was classified as architectural.
