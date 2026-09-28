# Core session plan CS1.7 (the conditional-compilation boundary check) — first remediation plan

Date: 2026-09-28. Status: **remediation plan for [the first verdict](GwzCoreSessionCS1.7-Verdict.md); applied to the step's files in one patch.**

Every finding of the two reports gets exactly one disposition: [Consistency](GwzCoreSessionCS1.7-ReviewConsistency.md) C-P2-1, C-P2-2 and C-P3-1 to C-P3-3, and [Safety](GwzCoreSessionCS1.7-ReviewSafety.md) S-P2-1 and S-P3-1 to S-P3-6.
- All findings are accepted. None is disputed.
- **Scope choice.** The two blocking findings are corrected as their reviewers specified, and the rule is not narrowed. Both reviewers named the alternative, an operator ruling that "declaration" means "item". The lane owner does not take it: `let` is a declaration statement in Rust's own grammar, and the rule's first sentence ("an explicit enclosing boundary") covers expression statements too.
- Where a reviewer left a choice open, the choice is stated. Three change something a reviewer attacked in round 1, and the re-verdict must check each against the old counterexamples:
  - statements are keyed by their rendered text;
  - macro items are keyed the same way;
  - the new `--shrink-from` mode runs in the boundary job.
- Files: the ten of the object. The allowlist is regenerated over the extended scope. No plan text changes in this patch: the plan is the object of TR1.4b's dual review, which is running. The two plan sentences below (S-P3-1 and C-P3-1) land with TR1.4b's remediation or its post-GO corrections.

## 1. Blocking findings

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| C-P2-1, S-P2-1 | Blind convergence. **Choice: the scan covers statements, keyed by their rendered text.** A conditional attribute placed directly on a `;`-terminated statement in a block (a function or closure body, or any block that is not an item list) is an occurrence, and the check flags it:<br>• a `let` statement, keyed `let <rendered pattern>`: its pattern's tokens before `=`, `:` or `;`, so `let mode`, `let _` or `let (a, b)`;<br>• an expression statement or statement macro that has no `{` at depth 0 before its `;`, keyed by the whole statement up to its `;`, rendered with the existing renderer, whose whitespace and trailing-comma normalization the keys must survive.<br>Braced statements still pass as explicit boundaries: a block, `if`, `match`, `loop`, `while`, `for`, `unsafe { }`, and a `cfg_if!` in statement position. Every existing occurrence in the three trees is inventoried as migration debt (§1.3), taken at the same HEADs. The docstring's scope paragraph states the true scope (see S-P3-1), and the false justification goes. | `check_cfg_boundaries.py`, its tests, `cfg_boundaries_allowlist.json` | Consistency's snippet `fn f() { #[cfg(not(unix))] let mode = 0; #[cfg(unix)] apply(mode); #[cfg(unix)] assert_eq!(mode, 0); }` gives three occurrences. The same three lines inside `cfg_if::cfg_if! { if #[cfg(unix)] { … } }` in a fn body give none. Safety's replay pairs (the `let mode` before and after) each flag. `#[cfg(unix)] { let m = 1; }` and `#[cfg(unix)] if x { y(); }` give none. A reformatted statement (line breaks, trailing comma) keeps its key. The allowlist matches the HEAD blobs exactly, and the working-tree run reports nothing new. |
| C-P2-2 | As specified. In `end()`, a `{` whose previous token is `<`, `=` or `,` in an item's header is stepped over as a matched group, as `(` and `[` are. In `in_item_list()`'s backward walk, a `}` whose matching `{` follows `<`, `=` or `,` is jumped over, not taken as a boundary. | `check_cfg_boundaries.py`, its tests | Consistency's four snippets flag `struct D`, `fn f`, `struct W` and `delegate!`. `#[cfg(unix)] const X: u8 = { 3 };` still keys `const X`, and `#[cfg(unix)] fn g() -> Foo<N> { … }` still passes. |

## 2. Nonblocking findings

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| S-P3-1 | **Choice: record the exclusion, with counts.** The Consistency axis ruled that fields, variants, match arms and parameters are outside the rule: they are neither items nor statements, and no explicit boundary can hold them in Rust. So they are not inventoried. The docstring and the allowlist's `rule` text state the exclusion, its reason and the counts at the inventory's HEADs (Safety's 31 fields and variants, 8 match arms and 4 parameters, recounted by the implementer). The plan's §3.0 gains the same sentence with TR1.4b's remediation. | docstring, allowlist `rule` | The docstring and the `rule` text name the four positions and their counts. `enum E { #[cfg(unix)] A }`, `struct S { #[cfg(unix)] a: u8 }`, a conditional match arm and a conditional parameter still give none (test kept). |
| C-P3-1, S-P3-6 | Blind convergence. **Choice: record the gap now, and close it in the siblings' CI after this step lands.** Pinning the siblings in gwz-core's boundary job, as the transport check does, validates the sibling entries only against the pinned commits: a new bare `cfg` in a sibling commit would still pass until the pin moves. The check belongs in gwz-cli's and gwz-py's own CI, run with gwz-core checked out beside them. That needs this check on gwz-core's `main`, so it is a follow-up task once CS1.7 is committed and pushed. Until then, sibling coverage is local-only: `run_tests.py` in a workspace fails closed without `--skip-cfg-siblings`. The docstring and the allowlist's `rule` text say so, and the plan's CS1.7 exit gains the sentence with TR1.4b's remediation. | docstring, allowlist `rule` | The docstring and the `rule` text state that sibling coverage is local-only until the siblings' CI runs the check. The follow-up task is filed when CS1.7 is committed. |
| C-P3-2 | A macro item whose path starts with `::` is recognized. The leading `::` is included in its key. | `check_cfg_boundaries.py`, its tests | `#[cfg(test)] ::probe::fixture!(x);` is flagged, with `::probe::fixture!` in its key. |
| C-P3-3, S-P3-5 | Blind convergence. The lexer's identifier class becomes `(?:r\#)?[^\W\d]\w*`. The divergence from `check_process_globals.py`'s lexer is carried (Verdict, "Carried"). | `check_cfg_boundaries.py`, its tests | `#[cfg(unix)] mod über;`, `#[cfg(unix)] const ünit: u8 = 0;` and `#[cfg(unix)] type Ärger = u8;` are flagged. |
| S-P3-2 | `--skip-repo` is refused, exit 1, for a repository whose `path` is `.`, the check's own. A run that scans no file fails. | `check_cfg_boundaries.py`, its tests | `--skip-repo gwz-core` exits 1, naming the rule. A fixture whose every repository is skipped exits 1. |
| S-P3-3 | **Choice: a `--shrink-from BASE` mode, run in the boundary job.**<br>• It compares the current allowlist with a base allowlist file, and fails on any key absent from the base, or any count larger than the base's.<br>• A base that does not exist passes with a message, which happens only once: the change that creates the list.<br>• The boundary job reads the base from `git show <base>:scripts/checks/cfg_boundaries_allowlist.json`. On `pull_request` the base is the pull request's base SHA; on `push` it is `github.event.before`, when that is not all zeros. The job has `fetch-depth: 0`.<br>• The docstring's "so the list only shrinks" names the mode that enforces it. | `check_cfg_boundaries.py`, its tests, `checked-artifact-boundary.yml` | An added key fails, and so does a count raised from 14 to 15. A count lowered from 14 to 13, or a removed key, passes. A missing base passes with its message. A workflow-text test asserts that the boundary job runs `--shrink-from` on both events. |
| S-P3-4 | Resolved by C-P2-1's keying. A macro invocation item is keyed by its whole rendered invocation, so the `delegate!` entry with count 14 splits into its 14 invocations. The docstring states exactly what a key covers: the whole rendered text for `use` items, macro items and statements; the kind and name for `type`, `const`, `static`, `fn`, `struct` and `mod`, so a change to such an item's type or body keeps its key. | `check_cfg_boundaries.py`, its tests, allowlist | Safety's two `delegate!` texts give different keys, and a fixture with one swapped body reports NEW and STALE. |

## 3. Residual notes taken

- **Fail-closed tests.** Tests pin the paths that fail closed only through an uncaught exception today: a malformed allowlist, one missing `repos`, and an undecodable file. Each exits nonzero (Safety §3).
- **Docstring completeness.** The docstring records that directory symlinks are not followed, and that the hidden `.github/bootstrap-crate` stubs are unscanned (Safety §2).

## 4. Re-verdict

The implementer applies all of the above as one patch and reports:
- the new SHA-256 of every file;
- the unit-test results;
- the check's full run and its `--skip-repo` sibling run;
- the new inventory's totals by kind and repository;
- an independent confirmation that the allowlist matches the HEAD blobs.

The same two reviewers then re-verdict with their context intact:
- **Consistency** checks C-P2-1, C-P2-2 and its P3s against its counterexamples, and the statement keying.
- **Safety** checks S-P2-1 and its P3s against its replays, and the `--shrink-from` wiring.

Each files its report as `-ReviewConsistency-1.md` or `-ReviewSafety-1.md`, with a table closing each prior finding.
