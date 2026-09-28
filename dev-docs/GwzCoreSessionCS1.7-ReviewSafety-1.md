# Core session plan CS1.7 (conditional-compilation boundary check) — SAFETY-AXIS REVIEW, ROUND 2

**Review object:** the ten uncommitted gwz-core files at the SHA-256 values in `scratchpad/cs17r2-object.sha256` (new: `scripts/checks/check_cfg_boundaries.py`, `scripts/checks/test_check_cfg_boundaries.py`, `scripts/checks/cfg_boundaries_allowlist.json`; modified: `scripts/run_tests.py`, `scripts/release.py`, `scripts/checks/test_release_boundary.py`, `.github/workflows/release.yml`, `platform-matrix.yml`, `windows-matrix.yml`, `checked-artifact-boundary.yml`). Four changed since round 1 (the check, its tests, the allowlist, `checked-artifact-boundary.yml`); six unchanged. gwz-core HEAD `bd53865690ab4c179babfac15e051b69423a5cff`; status: uncommitted step implementation, first revision; date 2026-09-28.

**Baseline:** gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5`, verified by `git rev-parse HEAD` at the start and the end; `shasum -a 256 -c cs17r2-object.sha256` reported all ten OK at both points; `cs17-r0-to-r1.diff` (1079 lines) and `cs17-r1.diff` (1738 lines) matched their recorded SHA-256 (`648c8d99…`, `e2ff93ad…`). Read this round: `GwzCoreSessionCS1.7-Verdict.md`, `-RemPlan.md`, the round-1 Consistency report, the implementer's choices note, the revised check and tests in full, the revised allowlist header and the `--shrink-from` workflow step. The round-1 allowlist copy used as a `--shrink-from` base (`scratchpad/cfg_boundaries_allowlist.round1.json`) hashes to `e86010a5…`, the round-1 object's allowlist. No file was written, no build run, no git mutation.

**Axis:** SAFETY — what the check permits to go wrong. Independent, adversarial, read-only; context intact from round 1; the Consistency round-1 report read only after this round opened. Filed verbatim by the lane owner.

**Verdict: GO** — 0 P0, 0 P1, 0 P2, 3 P3 (all new, all bounded). Every round-1 finding is closed.

| Prior finding | Status | Evidence |
| --- | --- | --- |
| P2-1 statements outside the scan | CLOSED | Every round-1 replay now flags before or after the deletion (§2, table): `let mode` → `let path`; `let mode` → `apply(path, mode)`; `assert_eq!(mode(), 0o755)` → `finish()`; `use …PermissionsExt` → `let m`; the `handles.rs` pair `let executable`/`let executable` → `finish(executable)`; a `let h` inside a disabled `cfg_if!` else-arm in a fn body → `close(h)`. 173 statements inventoried (147 expression statements, 24 `let`, 2 statement macros). My independent scan of the HEAD blobs (`git show HEAD:<path>` for every tracked `.rs` under the roots, 1191 files) matches the allowlist exactly: 432 keys, 437 occurrences, none missing, none extra, no count mismatch. 50 tests OK. |
| P3-1 fields, variants, arms, parameters | CLOSED (recorded, the alternative correction I named) | Docstring lines 29–34 and the allowlist `rule` text state the exclusion, its reason and the counts (31 fields/variants, 7 struct-expression fields, 8 arms, 2 parameters, 2 call arguments, 4 tail expressions). A4/A5/A6/A10 still give `[]` by design; `test_fields_variants_arms_and_parameters_are_out_of_scope` pins it. |
| P3-2 `--skip-repo` of the own repository | CLOSED | `--skip-repo gwz-core` → `refused; its path is the check's own repository…`, exit 1; the same with `--root ../gwz-core`; a run that scans no file fails (`scan()` line 383–384); tests `test_the_checks_own_repository_cannot_be_skipped`, `test_a_run_that_scans_no_file_fails`. |
| P3-3 nothing enforces "only shrinks" | CLOSED (a residual in the new mode is P3-7) | `--shrink-from BASE` (lines 442–458) and the boundary-job step on `pull_request` and non-initial `push`. Against the round-1 copy: 183 ADDED, 0 RAISED, exit 1; against itself exit 0; missing base exit 0 with its message; `/dev/null`, a directory and `""` exit 1; `ShrinkFrom` and `ShrinkWorkflow` tests. |
| P3-4 macro items keyed by path only | CLOSED | Key is the whole rendered invocation (`macro_item`, lines 275–285): the two `delegate!` texts of round 1 now differ; the count-14 entry is 14 keys; `test_a_swapped_macro_body_is_new_and_stale` reports NEW and STALE. |
| P3-5 non-ASCII-initial names | CLOSED | `(?P<id>(?:r\#)?[^\W\d]\w*)` (line 91); all five round-1 snippets flag (`fn ärger`, `type Ärger`, `mod ärger`, `const ÄRGER`, `struct Ärger`); `test_non_ascii_names_are_identifiers`. |
| P3-6 no CI checks gwz-cli or gwz-py | CLOSED as a recorded decision; the coverage itself is deferred | Docstring lines 66–69 and the `rule` text state that sibling coverage is local-only until the siblings' own CI runs the check, a follow-up once CS1.7 is on gwz-core's `main` (RemPlan C-P3-1/S-P3-6). No wiring change, by disposition; `run_tests.py` in a workspace still fails closed without `--skip-cfg-siblings`. |

## 0. Evidence base (what you read and ran, with outputs)

- `python3.13 -m unittest scripts/checks/test_check_cfg_boundaries.py scripts/checks/test_release_boundary.py` → `Ran 50 tests in 2.406s OK`.
- `python3.13 scripts/checks/check_cfg_boundaries.py` → `1199 files, 437 listed occurrences (gwz-core 435, gwz-cli 2, gwz-py 0); nothing new`, exit 0, 2.28 s wall (1191 tracked files plus the eight untracked CS1.4/CS1.5 files, which add no occurrence). `--skip-repo gwz-cli --skip-repo gwz-py` → `1037 files, 435 listed`. Two `--list` runs hashed identically (`0e8a7109…`, 432 lines).
- Exit codes: `--skip-repo gwz-core` 1; `--root ../gwz-core --skip-repo gwz-core` 1; `--shrink-from <round-1 copy>` 1 (183 ADDED, 0 RAISED); `--shrink-from <self>` 0; `--shrink-from /nonexistent/base.json` 0 with `no base allowlist … only the change that creates the list has none`; `--shrink-from /dev/null` 1 (`base: cannot read allowlist`); `--shrink-from <directory>` 1; `--shrink-from ""` 1 (`Is a directory: '.'`); `--list` 0.
- Allowlist: 432 entries, 437 occurrences; by target: `mod` 118, `use` 107, expression statements 147, `let` 24, macro items and statement macros 17, `fn` 11, `type` 6, `const` 4, `struct` 2, `static` 1; no key contains a newline; longest key 325 characters. `repos` unchanged.
- In-memory `analyze()` on 60 constructed snippets: the twelve round-1 replays, the seven round-1 P3 counterexamples, and new attacks on the statement rule (11), the statement-start rule (9), the header-brace rule (12) and key stability (8). Outputs quoted in §1 and §2.
- Tree-wide classification of every conditional attribute the revised `target()` still returns `None` for at a non-item position: 46 braced statements (pass by design), 99 + 9 + 2 `cfg_if!` arm headers, 43 fields/variants/arguments after a comma, 5 arms or tail expressions ending at `}`, and 2 hits my classifier labelled "assignment with a depth-0 brace" that inspection shows are match arms following a block-bodied arm (`crates/refcopy/src/native/attempt.rs:83`, `src/git/tests/g15/root_preservation/stash.rs:391`). No statement in the trees follows an inner attribute.
- `shrink()` driven in memory with a patched `load_allowlist` (no files written): removing a repository with its entries → `errors=[]`; narrowing gwz-core's roots with entries dropped → `errors=[]`; an added key → `ADDED …` (control). 32 of gwz-core's 430 entries lie outside `src/`.
- The workflow step (`checked-artifact-boundary.yml`, "Conditional-compilation allowlist only shrinks"): `if:` covers `pull_request` and `push` with a non-zero `before`; `BASE_SHA` is `base.sha` on a pull request, else `before`; `git rev-parse --verify "$BASE_SHA^{commit}"` runs first; `git cat-file -e` gates `git show` into `$RUNNER_TEMP`; then `--shrink-from "$base"`; no `shell:` (GitHub's default `bash -e {0}`), no pipe, no `continue-on-error`; before the Rust install; the checkout has `fetch-depth: 0`.

## 1. Findings (severity-ordered)

### P3-7 — `--shrink-from` compares entries only, so a change that narrows the scanned scope passes it

- **Root cause:** `shrink()` (`scripts/checks/check_cfg_boundaries.py:442-458`) loads both allowlists but reads only their entries; the `repos` dicts (paths and roots) are discarded.
- **Violated invariant:** the mode's stated purpose, "the list only shrinks" (docstring 53–56; RemPlan S-P3-3): the *debt* may only shrink. Coverage shrinking is the opposite of that intent.
- **Reproduction (in memory):** base `repos` {gwz-core, gwz-cli} with one entry each; current `repos` {gwz-core} with gwz-cli's entry removed → `shrink()` returns `errors=[]`, summary `1 entries, none added and no count raised over the base's 2`. Base roots `['.']`, current `['src']` with entries kept → `errors=[]`. Control, a key added → `ADDED …`.
- **Impact:** a change that drops `gwz-cli` from `repos`, or narrows gwz-core's roots from `.` to `src`, passes the only-shrinks gate; the normal check then reports the stranded entries as STALE (32 gwz-core entries sit outside `src/`), and removing them reads as debt repaid. Every step is visible in the allowlist diff, so this is a review-only guarantee again for scope, not a silent hole; exposure requires a deliberate edit.
- **Required correction:** in `shrink()`, fail when a repository present in the base is absent, when a repository's `path` changes, or when any base root is no longer covered by the current roots (`REMOVED repo …`, `NARROWED repo: root …`).
- **Regression test:** the two scenarios above fail; adding a repository or a root, and an unchanged `repos`, pass.

### P3-8 — An unbraced statement whose expression contains a `{` at depth 0 passes

- **Root cause:** `target()` (lines 311–312) keys a statement only when `end(k)` reaches `;`; `end()` (lines 238–248) returns at the first depth-0 `{` that is not a const-generic block, so an assignment, `return` or compound assignment whose right-hand side is a struct literal, block, `if`, `match` or braced closure is treated as braced. This implements the RemPlan's mechanism ("no `{` at depth 0 before its `;`") and the docstring (lines 20–21), but not the RemPlan's pass-list, which names only "a block, `if`, `match`, `loop`, `while`, `for`, `unsafe { }`" and statement-position `cfg_if!`.
- **Violated invariant:** AGENTS.md's first sentence, "Conditional compilation must have an explicit enclosing boundary": in `#[cfg(unix)] state = State { mode };` the braces belong to the right-hand expression, not to the statement, so nothing encloses the conditional statement.
- **Reproduction:** each of `state = State { mode };`, `total = if a { 1 } else { 2 };`, `total = match a { 0 => 1, _ => 2 };`, `return S { a: 1 };`, `x = { compute() };`, `*slot = S { a: 1 };`, `handler = move |x| { go(x) };`, `n += { 1 };` under `#[cfg(unix)]` in a fn body → `[]`. Controls: `slot.set(S { a: 1 });` → `slot.set(S{a:1})` and `let s = S { a: 1 };` → `let s` are flagged. Replay: after deleting `state = State { mode };` the attribute lands on `next();` → `next()` is flagged, so the after-state is caught unless the successor is itself braced or in this class.
- **Impact:** bounded. No such statement exists in the trees (the two classifier hits are match arms); the deletion hazard is caught in the common case by the successor; the class is ordinary Rust (`return S { .. }` in a platform-specific constructor), so it is a future false negative, not a present one.
- **Required correction:** decide bracedness by the statement's first token, not by any interior brace: a statement starting with `{`, `if`, `match`, `loop`, `while`, `for`, `unsafe`, `async`, a label, or a brace-delimited macro passes; any other statement that reaches `;` is keyed by its rendered text with `{ }` groups stepped over (`end(k, braces=True)`), so `state = State{mode}` becomes a key.
- **Regression test:** the eight snippets flag; the seven braced forms and statement-position `cfg_if!` still give `[]`; `let s = S { a: 1 };` keeps `let s`.

### P3-9 — A statement that follows an inner attribute is not recognised as a statement

- **Root cause:** the statement-start rule (`target()` line 304) accepts a statement only when the token before its attributes is `;`, `{` or `}`; after `#![allow(unused)]` that token is `]`.
- **Violated invariant:** the docstring's scope: "an unbraced statement in a block … a `let`, or an expression statement" — the position is unrestricted.
- **Reproduction:** `fn f() {\n    #![allow(unused)]\n    #[cfg(unix)]\n    let x = 1;\n    go(x);\n}` → `[]`; without the inner attribute → `[('#[cfg(unix)]', 'let x')]`.
- **Impact:** bounded; inner attributes at the top of a block are rare and none precedes a conditional statement in the trees; the false negative is silent.
- **Required correction:** also accept `start - 1 == ']'` when its matching `[` is preceded by `!` and `#` (an inner attribute).
- **Regression test:** the snippet yields `let x`; `struct S { #[cfg(unix)] a: u8 }` and a conditional match arm still yield `[]`.

## 2. Invariant analysis (attacks that held, with evidence)

**cfa14b8 replays on the revised module (before → after deleting the conditional line and leaving its attribute):**

| Position | Before | After | Outcome |
| --- | --- | --- | --- |
| A1 module `use` → `use` | `use crate::filesystem::FileSystem` | `use std::ffi::OsStr` | NEW + STALE |
| A2 module `use` → braced `fn` | `use a::B` | `[]` | STALE forces the edit; result is rule-compliant (the rule's boundary, as in round 1) |
| A3 `let` → expression statement | `let mode` | `apply(path, mode)` | NEW + STALE |
| A3b `let` → `let` (Windows-only break shape) | `let mode` | `let path` | NEW + STALE |
| A3c `handles.rs` pair, `not(unix)` line deleted | `let executable` ×2 | `let executable`, `finish(executable)` | NEW + STALE |
| A7 `use` in a disabled `cfg_if!` arm | `use …OsStrExt` | `use std::ffi::OsStr` | NEW + STALE |
| A8 statement macro → statement | `assert_eq!(mode(), 0o755)` | `finish()` | NEW + STALE |
| A9 fn-body `use` → `let` | `use …PermissionsExt` | `let m` | NEW + STALE |
| A11 `let` in a disabled arm in a fn body → statement | `let h` | `close(h)` | NEW + STALE |
| A12 expression statement → `return` | `super::fault::fault(Fault::Rename)` | `return 1` | NEW + STALE |
| A4/A5/A6/A10 field, variant, arm, parameter | `[]` | `[]` | recorded exclusion (P3-1 disposition) |

**Statement keys (choice: rendered text).** Two different statements share a key only where the renderer normalises away a difference: `foo((a,))` and `foo((a))` both key `foo((a))`, and `delegate!(a(),)`/`delegate!(a())` both key `delegate!(a())` (documented; a modification of an existing placement, never a new one); `let` keys are the pattern only, so a changed right-hand side keeps its key (documented). Identical statements count. A key depends only on the statement's own tokens: `end()` stops at the statement's `;`, edits to neighbours cannot change it, and a construct inserted before it can only turn it STALE (fail-closed, P3-9's `]` case included). rustfmt layouts key identically (`go(\n a,\n b,\n)` = `go(a, b)`); `=>` renders `= >` and `-> u8` renders `->u8` consistently. Two `--list` runs are byte-identical. A multi-line string literal inside a statement puts a newline in the key (none in the trees); JSON carries it, `--list`'s line format would not — cosmetic.

**`--shrink-from` in the boundary job.** A base commit the clone lacks fails at `git rev-parse --verify` under `bash -e` (a force-pushed `before` included: red, not open). The first push of a branch is skipped by the zeros guard; the trigger is `push: branches: [main]`. A fork pull request's `base.sha` lies on the base repository's branch, which `fetch-depth: 0` fetches. A base without the file passes once with its message; a base branch that never carried the file (a pull request into a feature branch) passes there, and the merge into `main` is compared on `push` against `before`, which carries it. A malformed, unreadable, directory or empty-string base exits 1; a malformed current allowlist exits 1. `$base` is a fresh `$RUNNER_TEMP` path, never a symlink. The comparison is by full key including path, so a moved file's entries read as ADDED — strict and fail-closed (see §3). Entries only: P3-7.

**Macro-item keys.** 14 `delegate!` keys in `src/git/gitbackend.rs`; the round-1 swap gives different keys; a leading `::` path is keyed (`::probe::fixture!(x)`); `macro_rules! noop ( … );` and `probe![windows]` keyed by rendered text.

**Header-brace rule (choice 1).** Held on: a rustfmt `where` clause ending in a comma before a body (`fn`, `struct`, `impl` with a macro item inside → only `delegate!(z)`); `struct D<const N: usize = { 2 }>;` → `struct D`; `fn f() -> Foo<{ N }, { M }>;` → `fn f`; `impl Foo<{ N }> for Bar { delegate!… }` → flagged; a bare `use` inside a where-comma body; `const X: u8 = { 3 };` → `const X`; `static T = ({ 1 }, 2);` then `use` → both; an array of blocks in a fn body then a statement → flagged; match-arm block bodies followed by commas then `trait T { fn g(); }` → `fn g`; `struct S<const N: usize = { 2 }> { … }` and `fn g() -> Foo<{ N }> { … }` pass; `g::<{ N }>();` → `g::<{N}>()`. No new false negative found.

**Statement-start rule (choice 3).** Held after a labeled block, a `let … else`, an `if` without `;`, a braced macro statement, a where-clause fn item, at the start of a closure body, and inside a match-arm block; struct-expression fields, arms and variants are refused as intended. An arm following a block-bodied arm satisfies the start rule but ends at `{`, so it is neither flagged nor mis-keyed; no `;` can occur at depth 0 in a match body. Inner attribute: P3-9.

**Fail-closed paths (choice 10).** Malformed, non-object, unreadable and missing allowlists, and an undecodable `.rs`, each exit 1 with a message (`UNREADABLE core/src/bad.rs` pinned by `test_a_malformed_allowlist_or_an_undecodable_file_fails_closed`); a refused skip prints no SKIPPED GATE. An unreadable *directory* is skipped silently by `os.walk`'s default `onerror=None` — nil exposure in CI checkouts (§3).

**Own-repository refusal (choice 9).** `--skip-repo gwz-core` refused for `.` and via `--root ../gwz-core` (resolved-path comparison); "no Rust file was scanned" when everything is skipped or the roots hold no `.rs`; an allowlist edit cannot evade it (a changed `path` for gwz-core either resolves to the root and is refused, or is MISSING, or strands 435 STALE entries).

**Unchanged wiring.** `run_tests.py`, `release.py`, its test and the three other workflows are byte-identical to round 1; the round-1 analysis stands (check first, unconditional, `check=True`; release `verify` jobs unpiped; matrix jobs fail closed through the summarize step). Cost: 2.28 s; tests 2.4 s.

## 3. Risks and next action

- **Moves under `--shrink-from`.** A `git mv` of a file holding debt entries (e.g. `src/git/gitbackend.rs`, about 40) makes every entry ADDED at the new path; the mover must migrate the debt or split the change. This is fail-closed and consistent with "only shrinks", but Phases 2–6 move files. If that proves too heavy, a "moved" allowance (same repo, attrs and item; total count not raised) is a bounded refinement — an operator choice, not a defect.
- **Allowlist rename.** The step hard-codes `scripts/checks/cfg_boundaries_allowlist.json` at the base; a future rename opens a one-transition "no base" pass for pull requests based on older `main`. Note it if the file ever moves.
- **Unreadable directory** silently unscanned (`os.walk` default); pass `onerror` to fail, when convenient.
- **Sibling coverage** stays local-only until the follow-up in gwz-cli's and gwz-py's CI (P3-6 disposition); file that task when CS1.7 is committed.
- **Carried:** `release.py --no-test` skips this and the other lexical checks (round 1 §3; Verdict "Carried").
- **Next action:** land the step. P3-7, P3-8 and P3-9 are each a few lines with the regression tests named above and can go in a post-GO correction or the next step's gate; none blocks.
